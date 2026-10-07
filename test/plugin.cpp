//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Test.h"
#include "conf/ConfFile.h"
#include "editor/TextEditor.h"
#include "plugins/Plugin.h"
#include <QTemporaryDir>

class TestPlugin : public QObject {
  Q_OBJECT

private slots:
  void confFileErrors();
  void confFileNoNativeCode();
  void noNativeCode();
  void readOnlyIo();
  void utf8Library();
  void hunk();
  void staleObject();
  void wrongObject();
  void argumentErrors();
  void optionDefaults();
  void traceback();
  void nilError();

private:
  PluginRef createPlugin(const QByteArray &hunk);
  void setText(TextEditor &editor);

  QTemporaryDir mDir;
};

void TestPlugin::confFileErrors() {
  QDir dir(mDir.path());
  QVERIFY(ConfFile("theme = {", dir).parse("theme").isEmpty());
  QVERIFY(ConfFile("error('failed')", dir).parse("theme").isEmpty());
  QVERIFY(ConfFile("return 42", dir).parse().isEmpty());
  QVERIFY(ConfFile("", dir).parse().isEmpty());

  QVariantMap map = ConfFile("return {[1.5] = 'x', a = 'b'}", dir).parse();
  QCOMPARE(map, QVariantMap({{"a", "b"}}));
}

void TestPlugin::confFileNoNativeCode() {
  QFile module(mDir.filePath("confmodule.lua"));
  QVERIFY(module.open(QFile::WriteOnly));
  module.write("return {value = 'loaded'}");
  module.close();

  QVariantMap map = ConfFile("return {cpath = package.cpath,"
                             "        loadlib = type(package.loadlib),"
                             "        module = require('confmodule').value}",
                             QDir(mDir.path()))
                        .parse();
  QCOMPARE(map.value("cpath"), QVariant(""));
  QCOMPARE(map.value("loadlib"), QVariant("nil"));
  QCOMPARE(map.value("module"), QVariant("loaded"));
}

void TestPlugin::readOnlyIo() {
  QString path = mDir.filePath("data.txt");
  QFile data(path);
  QVERIFY(data.open(QFile::WriteOnly));
  data.write("one\ntwo\n");
  data.close();

  PluginRef plugin = createPlugin("local path = [[" + path.toUtf8() + "]]" + R"(
    assert(io.popen == nil, "popen is available")
    assert(io.output == nil and io.write == nil, "output is available")
    assert(require("io").popen == nil, "require returns the full library")

    local lines = {}
    for line in io.lines(path) do lines[#lines + 1] = line end
    assert(#lines == 2 and lines[2] == "two", "lines failed")

    local file = assert(io.open(path))
    assert(file:read("l") == "one", "read failed")
    file:close()
    assert(io.open(path, "rb")):close()
    assert(io.open(path .. ".missing") == nil, "missing file opened")

    assert(not pcall(io.open, path, "w"), "write mode allowed")
    assert(not pcall(io.open, path, "r+"), "update mode allowed")
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));

  QVERIFY(data.open(QFile::ReadOnly));
  QCOMPARE(data.readAll(), QByteArray("one\ntwo\n"));
}

void TestPlugin::utf8Library() {
  PluginRef plugin = createPlugin(R"(
    assert(utf8.len("\u{e6}\u{f8}\u{e5}") == 3, "utf8.len failed")
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
}

void TestPlugin::noNativeCode() {
  QFile module(mDir.filePath("pluginmodule.lua"));
  QVERIFY(module.open(QFile::WriteOnly));
  module.write("return {value = 'loaded'}");
  module.close();

  PluginRef plugin = createPlugin(R"(
    assert(package.cpath == "", "cpath is set")
    assert(package.loadlib == nil, "loadlib is available")
    assert(require("pluginmodule").value == "loaded")
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
}

void TestPlugin::hunk() {
  PluginRef plugin = createPlugin(R"(
    for _, line in ipairs(hunk:lines()) do
      if line:origin() == "+" then
        local lexeme = line:lexemes()[1]
        line:add_error("err", lexeme:pos() + 1, #lexeme:text() - 1)
      end
    end
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  setText(editor);
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
  QCOMPARE(editor.diagnostics(0).size(), 0);
  QCOMPARE(editor.diagnostics(1).size(), 1);
  QCOMPARE(editor.diagnostics(1).first().range.pos, 1);
  QCOMPARE(editor.diagnostics(1).first().range.len, 4);
}

void TestPlugin::staleObject() {
  PluginRef plugin = createPlugin(R"(
    if saved then
      saved:text()
    end
    saved = hunk:lines()[1]
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  setText(editor);
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
  QVERIFY(!plugin->hunk(&editor));
  QVERIFY(plugin->errorString().contains("outside of the hunk() call"));
}

void TestPlugin::wrongObject() {
  PluginRef plugin = createPlugin("hunk:lines()[1].text(hunk)");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  setText(editor);
  QVERIFY(!plugin->hunk(&editor));
  QVERIFY(plugin->errorString().contains("Line expected"));
}

void TestPlugin::argumentErrors() {
  PluginRef plugin = createPlugin(R"lua(
    local function check(f, expected)
      local ok, msg = pcall(f)
      assert(not ok, "no error, expected: " .. expected)
      assert(msg:find(expected, 1, true), msg)
    end

    check(function() opts:define_integer("k", "t", "x") end,
          "bad argument #3 to 'define_integer' (number expected, got string)")
    check(function() opts.value("k") end,
          "bad argument #1 to 'value' (Options expected, got string)")
    check(function() opts:define_list("k", "t", {"a", {}}) end,
          "bad argument #3 to 'define_list' (list of strings expected)")
    check(function() opts:value("missing") end, "invalid option 'missing'")
    check(function() hunk:lines()[1]:add_error("err", "x", 1) end,
          "bad argument #2 to 'add_error' (number expected, got string)")
  )lua");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  setText(editor);
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
}

void TestPlugin::optionDefaults() {
  PluginRef plugin = createPlugin(R"(
    opts:define_boolean("b", "B")
    opts:define_integer("i", "I")
    opts:define_string("s", "S")
    opts:define_list("l", "L", {"a", "b"})
    assert(opts:value("b") == false, "boolean default")
    assert(opts:value("i") == 0, "integer default")
    assert(opts:value("s") == "", "string default")
    assert(opts:value("l") == 1, "list default")
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY2(plugin->hunk(&editor), qPrintable(plugin->errorString()));
}

void TestPlugin::traceback() {
  // The hunk body starts on line 5 of the generated script.
  PluginRef plugin = createPlugin(R"(
    local function fail() error("boom") end
    fail()
  )");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY(!plugin->hunk(&editor));
  QString msg = plugin->errorString();
  QVERIFY2(msg.section('\n', 0, 0).endsWith(":6: boom"), qPrintable(msg));
  QVERIFY2(msg.contains("stack traceback:"), qPrintable(msg));
  QVERIFY2(msg.contains(":7: in "), qPrintable(msg));
}

void TestPlugin::nilError() {
  PluginRef plugin = createPlugin("error(nil)");
  QVERIFY2(plugin->isValid(), qPrintable(plugin->errorString()));

  TextEditor editor;
  QVERIFY(!plugin->hunk(&editor));
  QVERIFY(!plugin->isValid());
}

PluginRef TestPlugin::createPlugin(const QByteArray &hunk) {
  static int count = 0;
  QFile file(mDir.filePath(QString("test%1.lua").arg(++count)));
  if (!file.open(QFile::WriteOnly))
    return PluginRef();

  file.write("function kinds (kinds, opts)\n"
             "  kinds:define_error('err', 'Err', 'msg', 'desc', true)\n"
             "end\n"
             "function hunk (hunk, opts)\n" +
             hunk + "\nend\n");
  file.close();

  return PluginRef(new Plugin(file.fileName(), git::Repository()));
}

void TestPlugin::setText(TextEditor &editor) {
  editor.setText("context\nadded\n");
  editor.markerAdd(1, TextEditor::Addition);
}

TEST_MAIN(TestPlugin)

#include "plugin.moc"

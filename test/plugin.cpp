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
  void hunk();
  void staleObject();
  void wrongObject();

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

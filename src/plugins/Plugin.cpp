//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "Plugin.h"
#include "qtsupport.h"
#include "conf/LuaLibs.h"
#include "conf/Settings.h"
#include "editor/TextEditor.h"
#include "git/Config.h"
#include <QCoreApplication>
#include <QTextStream>

#include <stdexcept>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace {

const QString kKeyFmt = "plugins.%1.%2";
const QString kSubkeyFmt = "plugins.%1.%2.%3";

int optionsScriptDir(lua_State *L);
int optionsDefineBoolean(lua_State *L);
int optionsDefineInteger(lua_State *L);
int optionsDefineString(lua_State *L);
int optionsDefineList(lua_State *L);
int optionsValue(lua_State *L);

int kindsDefineNote(lua_State *L);
int kindsDefineWarning(lua_State *L);
int kindsDefineError(lua_State *L);

int hunkLines(lua_State *L);
int hunkLexer(lua_State *L);
int hunkTabWidth(lua_State *L);

int lineText(lua_State *L);
int lineOrigin(lua_State *L);
int lineLexemes(lua_State *L);
int lineAddError(lua_State *L);
int lineColumn(lua_State *L);
int lineColumnPos(lua_State *L);

int lexemeEq(lua_State *L);
int lexemePos(lua_State *L);
int lexemeText(lua_State *L);
int lexemeKind(lua_State *L);
int lexemeIsKind(lua_State *L);

const luaL_Reg kOptionsFuncs[] = {{"script_dir", &optionsScriptDir},
                                  {"define_boolean", &optionsDefineBoolean},
                                  {"define_integer", &optionsDefineInteger},
                                  {"define_string", &optionsDefineString},
                                  {"define_list", &optionsDefineList},
                                  {"value", &optionsValue},
                                  {nullptr, nullptr}};

const luaL_Reg kKindsFuncs[] = {{"define_note", &kindsDefineNote},
                                {"define_warning", &kindsDefineWarning},
                                {"define_error", &kindsDefineError},
                                {nullptr, nullptr}};

const luaL_Reg kHunkFuncs[] = {{"lines", &hunkLines},
                               {"lexer", &hunkLexer},
                               {"tab_width", &hunkTabWidth},
                               {nullptr, nullptr}};

const luaL_Reg kLineFuncs[] = {
    {"text", &lineText},       {"origin", &lineOrigin},
    {"lexemes", &lineLexemes}, {"add_error", &lineAddError},
    {"column", &lineColumn},   {"column_pos", &lineColumnPos},
    {nullptr, nullptr}};

const luaL_Reg kLexemeFuncs[] = {
    {"__eq", &lexemeEq},   {"pos", &lexemePos},        {"text", &lexemeText},
    {"kind", &lexemeKind}, {"is_kind", &lexemeIsKind}, {nullptr, nullptr}};

// Hunk, Line and Lexeme objects are only valid during the hunk() call that
// created them, which is tracked by the plugin's generation.
struct HunkData {
  TextEditor *editor;
  quint64 generation;
};

struct LineData {
  TextEditor *editor;
  quint64 generation;
  int line;
};

struct LexemeData {
  TextEditor *editor;
  quint64 generation;
  int line;
  int pos;
  int len;
  int style;
};

Plugin *plugin(lua_State *L) {
  return static_cast<Plugin *>(lua_touserdata(L, lua_upvalueindex(1)));
}

void setMetatable(lua_State *L, Plugin *plugin, const char *name,
                  const luaL_Reg functions[]) {
  if (luaL_newmetatable(L, name)) {
    lua_pushvalue(L, -1); // metatable
    lua_setfield(L, -2, "__index");
    lua_pushlightuserdata(L, plugin);
    luaL_setfuncs(L, functions, 1);
  }

  lua_setmetatable(L, -2);
}

void createInstance(lua_State *L, Plugin *plugin, const char *name,
                    const luaL_Reg functions[]) {
  lua_newuserdatauv(L, 0, 0);
  setMetatable(L, plugin, name, functions);
}

template <typename T>
void createData(lua_State *L, Plugin *plugin, const char *name,
                const luaL_Reg functions[], const T &data) {
  *static_cast<T *>(lua_newuserdata(L, sizeof(T))) = data;
  setMetatable(L, plugin, name, functions);
}

template <typename T> T *checkData(lua_State *L, const char *name) {
  T *data = static_cast<T *>(luaL_checkudata(L, 1, name));
  if (data->generation != plugin(L)->generation())
    luaL_error(L, "%s used outside of the hunk() call that created it", name);

  return data;
}

// Lua errors longjmp past C++ destructors, so read all arguments before
// creating any C++ objects.
bool optBoolean(lua_State *L, int arg, bool def) {
  if (lua_isnoneornil(L, arg))
    return def;

  luaL_checktype(L, arg, LUA_TBOOLEAN);
  return lua_toboolean(L, arg);
}

int optionsScriptDir(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  lua_pushstring(L, plugin(L)->scriptDir().toUtf8());
  return 1;
}

int optionsDefineBoolean(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  const char *key = luaL_checkstring(L, 2);
  const char *text = luaL_checkstring(L, 3);
  bool value = optBoolean(L, 4, false);
  plugin(L)->defineOption(key, Plugin::Boolean, text, value);

  return 0;
}

int optionsDefineInteger(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  const char *key = luaL_checkstring(L, 2);
  const char *text = luaL_checkstring(L, 3);
  lua_Integer value = luaL_optinteger(L, 4, 0);
  plugin(L)->defineOption(key, Plugin::Integer, text, value);

  return 0;
}

int optionsDefineString(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  const char *key = luaL_checkstring(L, 2);
  const char *text = luaL_checkstring(L, 3);
  const char *value = luaL_optstring(L, 4, "");
  plugin(L)->defineOption(key, Plugin::String, text, value);

  return 0;
}

int optionsDefineList(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  const char *key = luaL_checkstring(L, 2);
  const char *text = luaL_checkstring(L, 3);
  luaL_checktype(L, 4, LUA_TTABLE);
  lua_Integer index = luaL_optinteger(L, 5, 1);

  lua_Integer count = 0;
  while (lua_rawgeti(L, 4, count + 1) != LUA_TNIL) {
    if (!lua_isstring(L, -1))
      luaL_argerror(L, 4, "list of strings expected");

    lua_pop(L, 1);
    ++count;
  }
  lua_pop(L, 1); // nil

  QStringList opts;
  for (lua_Integer i = 1; i <= count; ++i) {
    lua_rawgeti(L, 4, i);
    opts.append(lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  plugin(L)->defineOption(key, Plugin::List, text, index, opts);

  return 0;
}

int optionsValue(lua_State *L) {
  luaL_checkudata(L, 1, "Options");
  const char *key = luaL_checkstring(L, 2);
  if (!plugin(L)->optionKeys().contains(key))
    luaL_error(L, "invalid option '%s'", key);

  QVariant value = plugin(L)->optionValue(key);

  switch (plugin(L)->optionKind(key)) {
    case Plugin::Boolean:
      lua_pushboolean(L, value.toBool());
      break;

    case Plugin::List:
    case Plugin::Integer:
      lua_pushinteger(L, value.toInt());
      break;

    case Plugin::String:
      lua_pushstring(L, value.toString().toUtf8());
      break;
  }

  return 1;
}

int defineDiagnostic(lua_State *L, Plugin::DiagnosticKind kind) {
  luaL_checkudata(L, 1, "Kinds");
  const char *key = luaL_checkstring(L, 2);
  const char *name = luaL_checkstring(L, 3);
  const char *msg = luaL_checkstring(L, 4);
  const char *desc = luaL_checkstring(L, 5);
  bool enabled = optBoolean(L, 6, false);
  plugin(L)->defineDiagnostic(key, kind, name, msg, desc, enabled);

  return 0;
}

int kindsDefineNote(lua_State *L) { return defineDiagnostic(L, Plugin::Note); }

int kindsDefineWarning(lua_State *L) {
  return defineDiagnostic(L, Plugin::Warning);
}

int kindsDefineError(lua_State *L) {
  return defineDiagnostic(L, Plugin::Error);
}

int hunkLines(lua_State *L) {
  HunkData *hunk = checkData<HunkData>(L, "Hunk");

  // Create lines table.
  int count = hunk->editor->lineCount();
  lua_createtable(L, count, 0);
  for (int i = 0; i < count; ++i) {
    createData(L, plugin(L), "Line", kLineFuncs,
               LineData{hunk->editor, hunk->generation, i});
    lua_rawseti(L, -2, i + 1);
  }

  return 1;
}

int hunkLexer(lua_State *L) {
  HunkData *hunk = checkData<HunkData>(L, "Hunk");
  lua_pushstring(L, hunk->editor->lexer().toUtf8());
  return 1;
}

int hunkTabWidth(lua_State *L) {
  HunkData *hunk = checkData<HunkData>(L, "Hunk");
  lua_pushinteger(L, hunk->editor->tabWidth());
  return 1;
}

int lineText(lua_State *L) {
  LineData *line = checkData<LineData>(L, "Line");
  QByteArray text = line->editor->getLine(line->line);
  lua_pushlstring(L, text.constData(), text.size());
  return 1;
}

int lineOrigin(lua_State *L) {
  LineData *line = checkData<LineData>(L, "Line");
  TextEditor *editor = line->editor;

  QByteArray marker = " ";
  int markers = editor->markerGet(line->line);
  if (markers & (1 << TextEditor::Addition)) {
    marker = "+";
  } else if (markers & (1 << TextEditor::Deletion)) {
    marker = "-";
  }

  lua_pushstring(L, marker);
  return 1;
}

void addLexeme(lua_State *L, const LineData *line, int index, int pos, int len,
               int style) {
  createData(
      L, plugin(L), "Lexeme", kLexemeFuncs,
      LexemeData{line->editor, line->generation, line->line, pos, len, style});
  lua_rawseti(L, -2, index);
}

int lineLexemes(lua_State *L) {
  LineData *line = checkData<LineData>(L, "Line");
  TextEditor *editor = line->editor;

  // Create lexemes table.
  lua_newtable(L);
  int max = editor->lineEndPosition(line->line);
  int pos = editor->positionFromLine(line->line);
  if (pos == max)
    return 1;

  // Ensure styled to end of line.
  int endStyled = editor->endStyled();
  if (max > endStyled)
    editor->colourise(endStyled, max);

  int count = 0;
  int start = pos;
  int style = editor->styleAt(pos);
  for (int i = pos + 1; i < max; ++i) {
    int nextStyle = editor->styleAt(i);
    if (nextStyle != style) {
      addLexeme(L, line, ++count, start - pos, i - start, style);
      start = i;
      style = nextStyle;
    }
  }

  addLexeme(L, line, ++count, start - pos, max - start, style);

  return 1;
}

int lineAddError(lua_State *L) {
  LineData *data = checkData<LineData>(L, "Line");
  TextEditor *editor = data->editor;
  int line = data->line;
  const char *keyArg = luaL_checkstring(L, 2);
  int pos = luaL_checkinteger(L, 3) - 1;
  int len = luaL_checkinteger(L, 4);
  const char *replacementArg = luaL_optstring(L, 5, nullptr);
  QString key = keyArg;
  QString replacement = replacementArg;

  // Check if this error is enabled.
  if (!plugin(L)->isEnabled(key))
    return 0;

  // Add diagnostic.
  QString msg = plugin(L)->diagnosticMessage(key);
  QString desc = plugin(L)->diagnosticDescription(key);
  TextEditor::DiagnosticKind kind =
      static_cast<TextEditor::DiagnosticKind>(plugin(L)->diagnosticKind(key));
  editor->addDiagnostic(line, {kind, msg, desc, {pos, len}, replacement});

  return 0;
}

int lineColumn(lua_State *L) {
  LineData *line = checkData<LineData>(L, "Line");
  TextEditor *editor = line->editor;

  int pos = editor->positionFromLine(line->line) + luaL_checkinteger(L, 2) - 1;
  lua_pushinteger(L, editor->column(pos) + 1);
  return 1;
}

int lineColumnPos(lua_State *L) {
  LineData *line = checkData<LineData>(L, "Line");
  TextEditor *editor = line->editor;

  int pos = editor->findColumn(line->line, luaL_checkinteger(L, 2) - 1);
  lua_pushinteger(L, pos - editor->positionFromLine(line->line) + 1);
  return 1;
}

int lexemeEq(lua_State *L) {
  auto *lhs = static_cast<LexemeData *>(luaL_testudata(L, 1, "Lexeme"));
  auto *rhs = static_cast<LexemeData *>(luaL_testudata(L, 2, "Lexeme"));
  lua_pushboolean(L,
                  lhs && rhs && lhs->line == rhs->line && lhs->pos == rhs->pos);
  return 1;
}

int lexemePos(lua_State *L) {
  lua_pushinteger(L, checkData<LexemeData>(L, "Lexeme")->pos + 1);
  return 1;
}

int lexemeText(lua_State *L) {
  LexemeData *lexeme = checkData<LexemeData>(L, "Lexeme");
  QByteArray text =
      lexeme->editor->getLine(lexeme->line).mid(lexeme->pos, lexeme->len);
  lua_pushlstring(L, text.constData(), text.size());
  return 1;
}

QByteArray kind(TextEditor *editor, int style) {
  // Scintillua names styles like "whitespace.lua" or "comment.documentation".
  // Plugins only care about the base kind.
  QByteArray name = editor->nameOfStyle(style);
  int dot = name.indexOf('.');
  return dot < 0 ? name : name.left(dot);
}

int lexemeKind(lua_State *L) {
  LexemeData *lexeme = checkData<LexemeData>(L, "Lexeme");
  lua_pushstring(L, kind(lexeme->editor, lexeme->style));
  return 1;
}

int lexemeIsKind(lua_State *L) {
  LexemeData *lexeme = checkData<LexemeData>(L, "Lexeme");
  const char *name = luaL_checkstring(L, 2);
  lua_pushboolean(L, kind(lexeme->editor, lexeme->style) == name);
  return 1;
}

int traceback(lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (!msg)
    msg =
        lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));

  luaL_traceback(L, L, msg, 1);
  return 1;
}

// Call the function below nargs arguments, adding a traceback to any error.
int pcall(lua_State *L, int nargs) {
  int base = lua_gettop(L) - nargs;
  lua_pushcfunction(L, &traceback);
  lua_insert(L, base);
  int status = lua_pcall(L, nargs, 0, base);
  lua_remove(L, base);
  return status;
}

} // namespace

Plugin::Plugin(const QString &file, const git::Repository &repo,
               QObject *parent)
    : QObject(parent), mRepo(repo), L(luaL_newstate()) {
  QFileInfo info(file);
  mDir = info.dir().path();
  mName = info.baseName();

  // Print error messages to the console.
  connect(this, &Plugin::error, [](const QString &msg) {
    QTextStream(stderr) << "plugin error: " << msg << Qt::endl;
  });

  // Load a limited set of libraries
  openRestrictedLibs(L);
  openReadOnlyIo(L);

  // Add script dir to path.
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path");
  QByteArray path = lua_tostring(L, -1);
  lua_pop(L, 1); // path
  lua_pushstring(L, path + ";" + mDir.toUtf8() + "/?.lua");
  lua_setfield(L, -2, "path");
  lua_pop(L, 1); // package

  // Load script.
  if (luaL_loadfile(L, file.toLocal8Bit()) || pcall(L, 0)) {
    setError(lua_tostring(L, -1));
    return;
  }

  // Read options.
  if (lua_getglobal(L, "options")) {
    // Create options table.
    createInstance(L, this, "Options", kOptionsFuncs);

    // Call options.
    if (pcall(L, 1)) {
      setError(lua_tostring(L, -1));
      return;
    }

  } else {
    lua_pop(L, 1); // nil
  }

  // Read kinds.
  if (!lua_getglobal(L, "kinds")) {
    setError("global 'kinds' function not found");
    return;
  }

  // Create kinds and options tables.
  createInstance(L, this, "Kinds", kKindsFuncs);
  createInstance(L, this, "Options", kOptionsFuncs);

  // Call kinds.
  if (pcall(L, 2))
    setError(lua_tostring(L, -1));
}

Plugin::~Plugin() { lua_close(L); }

bool Plugin::isValid() const { return mError.isEmpty(); }

QString Plugin::name() const { return mName; }

QString Plugin::scriptDir() const { return mDir; }

QString Plugin::errorString() const { return mError; }

bool Plugin::isEnabled() const {
  for (const QString &key : mDiagnostics.keys()) {
    if (isEnabled(key))
      return true;
  }

  return false;
}

bool Plugin::isEnabled(const QString &key) const {
  bool enabled = mDiagnostics.value(key).enabled;
  return config().value<bool>(kSubkeyFmt.arg(mName, key, "enabled"), enabled);
}

void Plugin::setEnabled(const QString &key, bool enabled) {
  if (enabled != isEnabled(key))
    config().setValue(kSubkeyFmt.arg(mName, key, "enabled"), enabled);
}

void Plugin::defineOption(const QString &key, OptionKind kind,
                          const QString &text, const QVariant &value,
                          const QStringList &opts) {
  mOptions.insert(key, {kind, text, value, opts});
}

void Plugin::setOptionValue(const QString &key, const QVariant &value) {
  switch (optionKind(key)) {
    case Boolean:
      config().setValue(kKeyFmt.arg(mName, key), value.toBool());
      break;

    case List:
    case Integer:
      config().setValue(kKeyFmt.arg(mName, key), value.toInt());
      break;

    case String:
      config().setValue(kKeyFmt.arg(mName, key), value.toString());
      break;
  }
}

QStringList Plugin::optionKeys() const { return mOptions.keys(); }

QString Plugin::optionText(const QString &key) const {
  return mOptions.value(key).text;
}

QVariant Plugin::optionValue(const QString &key) const {
  QVariant value = mOptions.value(key).value;
  switch (optionKind(key)) {
    case Boolean:
      return config().value<bool>(kKeyFmt.arg(mName, key), value.toBool());

    case List:
    case Integer:
      return config().value<int>(kKeyFmt.arg(mName, key), value.toInt());

    case String:
      return config().value<QString>(kKeyFmt.arg(mName, key), value.toString());
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(optionKind(key))));
}

Plugin::OptionKind Plugin::optionKind(const QString &key) const {
  return mOptions.value(key).kind;
}

QStringList Plugin::optionOpts(const QString &key) const {
  return mOptions.value(key).opts;
}

void Plugin::defineDiagnostic(const QString &key, DiagnosticKind kind,
                              const QString &name, const QString &msg,
                              const QString &desc, bool enabled) {
  mDiagnostics.insert(key, {kind, name, msg, desc, enabled});
}

void Plugin::setDiagnosticKind(const QString &key, DiagnosticKind kind) {
  config().setValue<int>(kSubkeyFmt.arg(mName, key, "kind"), kind);
}

QStringList Plugin::diagnosticKeys() const { return mDiagnostics.keys(); }

Plugin::DiagnosticKind Plugin::diagnosticKind(const QString &key) const {
  DiagnosticKind defaultKind = mDiagnostics.value(key).kind;
  return static_cast<DiagnosticKind>(
      config().value<int>(kSubkeyFmt.arg(mName, key, "kind"), defaultKind));
}

QString Plugin::diagnosticName(const QString &key) const {
  return mDiagnostics.value(key).name;
}

QString Plugin::diagnosticMessage(const QString &key) const {
  return mDiagnostics.value(key).message;
}

QString Plugin::diagnosticDescription(const QString &key) const {
  return mDiagnostics.value(key).description;
}

bool Plugin::hunk(TextEditor *editor) const {
  if (!lua_getglobal(L, "hunk")) {
    const_cast<Plugin *>(this)->setError("global 'hunk' function not found");
    return false;
  }

  // Create hunk object.
  createData(L, const_cast<Plugin *>(this), "Hunk", kHunkFuncs,
             HunkData{editor, mGeneration});

  // Create options table.
  createInstance(L, const_cast<Plugin *>(this), "Options", kOptionsFuncs);

  // Call hunk function, then invalidate the objects it was given.
  bool failed = pcall(L, 2);
  ++mGeneration;
  if (failed) {
    const_cast<Plugin *>(this)->setError(lua_tostring(L, -1));
    return false;
  }

  return true;
}

QList<PluginRef> Plugin::plugins(const git::Repository &repo) {
  QList<PluginRef> plugins;
  QDir dir = Settings::pluginsDir();
  for (const QString &name : dir.entryList({"*.lua"}, QDir::Files))
    plugins.append(PluginRef(new Plugin(dir.filePath(name), repo)));

  QDir user = Settings::userDir();
  if (user.cd("plugins")) {
    for (const QString &name : user.entryList({"*.lua"}, QDir::Files))
      plugins.append(PluginRef(new Plugin(user.filePath(name), repo)));
  }

  return plugins;
}

git::Config Plugin::config() const {
  return mRepo.isValid() ? mRepo.appConfig() : git::Config::appGlobal();
}

void Plugin::setError(const QString &err) {
  lua_pop(L, 1); // error or nil
  mError = err;
  emit error(err);
}

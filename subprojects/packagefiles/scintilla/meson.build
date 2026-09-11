project(
    'scintilla',
    'cpp',
    version: '5.6.6',
    default_options: ['cpp_std=c++17'],
)

qt6 = import('qt6')

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Core5Compat'])

# Public headers plus the internal dirs the sources include from each other.
scintilla_inc = include_directories(
    'include',
    'src',
    'qt/ScintillaEdit',
    'qt/ScintillaEditBase',
)

# The Q_OBJECT classes are the three Qt binding headers.
scintilla_moc = qt6.preprocess(
    moc_headers: files(
        'qt/ScintillaEditBase/ScintillaQt.h',
        'qt/ScintillaEditBase/ScintillaEditBase.h',
        'qt/ScintillaEdit/ScintillaEdit.h',
        'qt/ScintillaEdit/ScintillaDocument.h',
    ),
    include_directories: scintilla_inc,
    dependencies: qt6_dep,
)

# The Qt platform layer plus the subset of core sources Pawmmit actually uses.
scintilla_sources = files(
    'qt/ScintillaEdit/ScintillaDocument.cpp',
    'qt/ScintillaEdit/ScintillaEdit.cpp',
    'qt/ScintillaEditBase/PlatQt.cpp',
    'qt/ScintillaEditBase/ScintillaQt.cpp',
    'qt/ScintillaEditBase/ScintillaEditBase.cpp',
    'src/AutoComplete.cxx',
    'src/CallTip.cxx',
    'src/CaseConvert.cxx',
    'src/CaseFolder.cxx',
    'src/CellBuffer.cxx',
    'src/ChangeHistory.cxx',
    'src/CharacterCategoryMap.cxx',
    'src/CharacterType.cxx',
    'src/CharClassify.cxx',
    'src/ContractionState.cxx',
    'src/DBCS.cxx',
    'src/Decoration.cxx',
    'src/Document.cxx',
    'src/Editor.cxx',
    'src/EditModel.cxx',
    'src/EditView.cxx',
    'src/Geometry.cxx',
    'src/Indicator.cxx',
    'src/KeyMap.cxx',
    'src/LineMarker.cxx',
    'src/MarginView.cxx',
    'src/PerLine.cxx',
    'src/PositionCache.cxx',
    'src/RESearch.cxx',
    'src/RunStyles.cxx',
    'src/ScintillaBase.cxx',
    'src/Selection.cxx',
    'src/Style.cxx',
    'src/UndoHistory.cxx',
    'src/UniConversion.cxx',
    'src/UniqueString.cxx',
    'src/ViewStyle.cxx',
    'src/XPM.cxx',
)

# public_args must also reach every consumer that includes Scintilla headers;
# private_args only affect building the library itself.
scintilla_public_args = ['-DSCINTILLA_QT', '-DSCI_LEXER']
scintilla_private_args = ['-DSCI_EMPTYCATALOGUE', '-DMAKING_LIBRARY']

scintilla_lib = static_library(
    'scintilla',
    scintilla_sources + scintilla_moc,
    include_directories: scintilla_inc,
    cpp_args: scintilla_public_args + scintilla_private_args,
    dependencies: qt6_dep,
)

scintilla_dep = declare_dependency(
    link_with: scintilla_lib,
    include_directories: scintilla_inc,
    compile_args: scintilla_public_args,
    dependencies: qt6_dep,
)

meson.override_dependency('scintilla', scintilla_dep)

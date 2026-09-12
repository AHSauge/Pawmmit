project(
    'lexilla',
    'cpp',
    version: '5.5.3',
    default_options: ['cpp_std=c++17'],
)

# Lexilla builds against Scintilla's public headers (ILexer.h, Sci_Position.h, ...).
scintilla_dep = dependency('scintilla')

lexilla_inc = include_directories('lexlib', 'include')

# Only the lexer framework: Pawmmit lexes through Scintillua, so none of the
# bundled per-language .cxx lexers are compiled.
lexilla_sources = files(
    'src/Lexilla.cxx',
    'lexlib/Accessor.cxx',
    'lexlib/CharacterCategory.cxx',
    'lexlib/CharacterSet.cxx',
    'lexlib/DefaultLexer.cxx',
    'lexlib/LexerBase.cxx',
    'lexlib/LexerModule.cxx',
    'lexlib/LexerSimple.cxx',
    'lexlib/PropSetSimple.cxx',
    'lexlib/StyleContext.cxx',
    'lexlib/WordList.cxx',
)

lexilla_lib = static_library(
    'lexilla',
    lexilla_sources,
    include_directories: lexilla_inc,
    dependencies: scintilla_dep,
)

lexilla_dep = declare_dependency(
    link_with: lexilla_lib,
    include_directories: lexilla_inc,
    dependencies: scintilla_dep,
)

meson.override_dependency('lexilla', lexilla_dep)

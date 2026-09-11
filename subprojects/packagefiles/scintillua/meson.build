project(
    'scintillua',
    'cpp',
    version: '6.7',
    default_options: ['cpp_std=c++17'],
)

# lexilla_dep transitively carries Scintilla's include/ (ILexer.h etc.).
lexilla_dep = dependency('lexilla')
lua_dep = dependency('lua')
lpeg_dep = dependency('lpeg')

# Static, linked against the parent project's own scintilla/lexilla/lua/lpeg,
# unlike upstream's shared build with vendored deps.
scintillua_lib = static_library(
    'scintillua',
    'Scintillua.cxx',
    dependencies: [lexilla_dep, lua_dep, lpeg_dep],
)

scintillua_dep = declare_dependency(
    link_with: scintillua_lib,
    include_directories: include_directories('.'),
    variables: {
        # Absolute path to the bundled Lua lexers; src/conf bakes this in so a
        # dev build loads them without an install step.
        'lexers_dir': meson.current_source_dir() / 'lexers',
    },
)

meson.override_dependency('scintillua', scintillua_dep)

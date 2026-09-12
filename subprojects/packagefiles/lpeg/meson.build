project(
    'lpeg',
    'c',
    version: '1.1.0',
    default_options: ['c_std=c99'],
)

# lpeg is a C extension for Lua and needs the Lua headers to build.
lua_dep = dependency('lua')

# The library sources only; lpeg.c / makefile are the standalone-build glue.
lpeg_lib = static_library(
    'lpeg',
    files(
        'lpcap.c',
        'lpcode.c',
        'lpprint.c',
        'lpcset.c',
        'lptree.c',
        'lpvm.c',
    ),
    dependencies: lua_dep,
)

lpeg_dep = declare_dependency(
    link_with: lpeg_lib,
    include_directories: include_directories('.'),
    dependencies: lua_dep,
)

meson.override_dependency('lpeg', lpeg_dep)

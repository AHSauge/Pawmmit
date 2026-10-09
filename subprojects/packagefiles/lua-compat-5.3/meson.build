project(
    'lua-compat-5.3',
    'c',
    version: '0.15.1',
    license: 'MIT',
    default_options: ['c_std=c99'],
)

lua_dep = dependency('luajit')

# A prefix makes the shims real functions in this library instead of static
# copies in every file that includes compat-5.3.h.
compat53_args = ['-DCOMPAT53_PREFIX=compat53']

# lutf8lib.c provides the utf8 library as luaopen_compat53_utf8.
compat53_lib = static_library(
    'compat53',
    files('c-api/compat-5.3.c', 'lutf8lib.c'),
    c_args: compat53_args,
    dependencies: lua_dep,
)

compat53_dep = declare_dependency(
    link_with: compat53_lib,
    include_directories: include_directories('c-api'),
    compile_args: compat53_args,
    dependencies: lua_dep,
)

meson.override_dependency('lua-compat-5.3', compat53_dep)

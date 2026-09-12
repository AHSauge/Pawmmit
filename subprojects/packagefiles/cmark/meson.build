project('cmark', 'c', version: '0.31.1', default_options: ['c_std=c99'])

# src/cmark_export.h stands in for upstream's generated export header;
# CMARK_STATIC_DEFINE makes every branch but the first one moot.
version_conf = configuration_data()
version_conf.set('PROJECT_VERSION_MAJOR', 0)
version_conf.set('PROJECT_VERSION_MINOR', 31)
version_conf.set('PROJECT_VERSION_PATCH', 1)
cmark_version_h = configure_file(
    input: 'src/cmark_version.h.in',
    output: 'cmark_version.h',
    configuration: version_conf,
)

# '.' too: cmark_version.h lands in the build root, not src/.
cmark_inc = include_directories('.', 'src')
cmark_static_define = ['-DCMARK_STATIC_DEFINE']

cmark_sources = files(
    'src/blocks.c',
    'src/buffer.c',
    'src/cmark.c',
    'src/cmark_ctype.c',
    'src/commonmark.c',
    'src/houdini_href_e.c',
    'src/houdini_html_e.c',
    'src/houdini_html_u.c',
    'src/html.c',
    'src/inlines.c',
    'src/iterator.c',
    'src/latex.c',
    'src/man.c',
    'src/node.c',
    'src/references.c',
    'src/render.c',
    'src/scanners.c',
    'src/utf8.c',
    'src/xml.c',
)

cmark_lib = static_library(
    'cmark',
    cmark_sources,
    cmark_version_h,
    include_directories: cmark_inc,
    c_args: cmark_static_define,
)

cmark_dep = declare_dependency(
    link_with: cmark_lib,
    include_directories: cmark_inc,
    compile_args: cmark_static_define,
)

# The `cmark` command-line tool - Pawmmit renders its bundled markdown docs
# with it at build time.
cmark_exe = executable(
    'cmark',
    'src/main.c',
    cmark_version_h,
    include_directories: cmark_inc,
    c_args: cmark_static_define,
    link_with: cmark_lib,
)

meson.override_dependency('libcmark', cmark_dep)
meson.override_find_program('cmark', cmark_exe)

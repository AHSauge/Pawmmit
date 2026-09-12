project('zip', 'c', version: '0.3', default_options: ['c_std=c99'])

# kuba--/zip: one translation unit wrapping miniz, no upstream Meson support.
zip_c_args = []
if host_machine.system() != 'windows'
    zip_c_args += '-D_GNU_SOURCE'
endif

zip_inc = include_directories('src')

zip_lib = static_library(
    'zip',
    'src/zip.c',
    c_args: zip_c_args,
    include_directories: zip_inc,
)

zip_dep = declare_dependency(
    link_with: zip_lib,
    include_directories: zip_inc,
)

meson.override_dependency('zip', zip_dep)

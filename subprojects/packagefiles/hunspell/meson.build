project('hunspell', 'cpp', version: '1.7.2', default_options: ['cpp_std=c++11'])

# hunvisapi.h.in's only substitution only matters for the shared/non-static
# branch of the macro, which HUNSPELL_STATIC (set below) always bypasses.
visapi_conf = configuration_data()
visapi_conf.set('HAVE_VISIBILITY', '1')
hunvisapi_h = configure_file(
    input: 'src/hunspell/hunvisapi.h.in',
    output: 'hunvisapi.h',
    configuration: visapi_conf,
)

hunspell_inc = include_directories('src/hunspell')

# HUNSPELL_STATIC must reach consumers too: hunvisapi.h picks dllimport vs.
# nothing for LIBHUNSPELL_DLL_EXPORTED based on it (matters on Windows).
hunspell_args = ['-DHUNSPELL_STATIC']

# Only the core library; src/parsers/ is the standalone CLI's format converters.
hunspell_sources = files(
    'src/hunspell/affentry.cxx',
    'src/hunspell/affixmgr.cxx',
    'src/hunspell/csutil.cxx',
    'src/hunspell/filemgr.cxx',
    'src/hunspell/hashmgr.cxx',
    'src/hunspell/hunspell.cxx',
    'src/hunspell/hunzip.cxx',
    'src/hunspell/phonet.cxx',
    'src/hunspell/replist.cxx',
    'src/hunspell/suggestmgr.cxx',
)

hunspell_lib = static_library(
    'hunspell',
    hunspell_sources,
    hunvisapi_h,
    include_directories: hunspell_inc,
    cpp_args: hunspell_args + ['-DBUILDING_LIBHUNSPELL'],
)

hunspell_dep = declare_dependency(
    link_with: hunspell_lib,
    include_directories: hunspell_inc,
    compile_args: hunspell_args,
)

meson.override_dependency('hunspell', hunspell_dep)

indexer_src = files(
    'common.cpp',
    'diffgrabber.cpp',
    'idstorage.cpp',
    'indexer.cpp',
    'lexerpool.cpp',
    'main.cpp',
    'map.cpp',
    'reduce.cpp',
    'resultwriter.cpp',
)

indexer_deps = [pawmmit_common, index_dep]
indexer_kwargs = {}

if is_win
    indexer_deps += cc.find_library('dbghelp')
elif is_mac
    indexer_kwargs += {
        'install_rpath': '@loader_path/../Frameworks',
        'build_rpath': qt_build_rpath,
    }
elif get_option('bundled')
    indexer_kwargs += {'install_rpath': '$ORIGIN'}
endif
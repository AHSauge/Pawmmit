indexer_src = files(
    'main.cpp',
    'indexer.cpp',
    'diffgrabber.cpp',
    'map.cpp',
    'reduce.cpp',
    'resultwriter.cpp',
    'common.cpp',
    'lexerpool.cpp',
    'idstorage.cpp',
)

indexer_deps = [pawmmit_common, index_dep]
indexer_kwargs = {}

if is_win
    indexer_deps += cc.find_library('dbghelp')
elif is_mac
    indexer_kwargs += {'install_rpath': '@loader_path/../Frameworks'}
else
    indexer_kwargs += {'install_rpath': '$ORIGIN'}
endif

# Installed next to the main executable on every platform (Contents/MacOS on
# macOS) - the app spawns it as a sibling.
executable(
    'pawmmit-indexer',
    indexer_src,
    cpp_args: ['-DPAWMMIT_VERSION="' + meson.project_version() + '"'],
    dependencies: indexer_deps,
    install: true,
    install_dir: app_bindir,
    kwargs: indexer_kwargs,
)

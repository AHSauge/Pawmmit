plugins_moc = qt6.preprocess(
    moc_headers: files(
        'Plugin.h',
    ),
    dependencies: [pawmmit_common],
)

plugins_src = [
    'Plugin.cpp',
]

plugins_lib = static_library(
    'plugins',
    plugins_src + plugins_moc,
    dependencies: [pawmmit_common, editor_dep, git_dep, lua_dep],
)

plugins_dep = declare_dependency(
    link_with: plugins_lib,
    include_directories: include_directories('.'),
)

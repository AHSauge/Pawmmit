pawmmitUtil_lib = static_library('pawmmitUtil', ['Debug.cpp', 'Path.cpp'], dependencies: pawmmit_common)

pawmmitUtil_dep = declare_dependency(
    link_with: pawmmitUtil_lib,
    include_directories: include_directories('.'),
)
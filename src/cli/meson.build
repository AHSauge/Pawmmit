cli_deps = [qt6_dep]
cli_src = ['Installer.cpp']

if is_mac
    cli_deps += dependency('appleframeworks', modules: ['Security'])
    cli_src += ['Installer_mac.mm']  # Objective-C++
elif not is_win
    cli_src += ['Installer_linux.cpp']
endif
# Windows: Installer.cpp only.

cli_lib = static_library('cli', cli_src, dependencies: cli_deps)

cli_dep = declare_dependency(
    link_with: cli_lib,
    include_directories: include_directories('.'),
)

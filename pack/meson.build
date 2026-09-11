iconset = meson.project_source_root() / 'rsrc' / 'Pawmmit.iconset'

# Linux desktop icons (hicolor theme). macOS uses the bundled .icns; Windows the
# .ico compiled into the executable.
if not is_mac and not is_win
    foreach size : ['16', '32', '64', '128', '256', '512']
        install_data(
            iconset / 'icon_' + size + 'x' + size + '.png',
            install_dir: get_option('datadir') / 'icons' / 'hicolor' / (size + 'x' + size) / 'apps',
            rename: 'pawmmit.png',
        )
    endforeach
    install_data(
        iconset / 'pawmmit_logo.svg',
        install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps',
        rename: 'pawmmit.svg',
    )
endif

install_data(
    meson.project_source_root() / 'LICENSE.md',
    install_dir: get_option('datadir') / 'licenses' / 'Pawmmit',
    rename: 'LICENSE',
)

# qt.conf tells a bundled Qt where to find its plugins/translations.
if is_mac
    install_data(
        meson.project_source_root() / 'conf' / 'qt.conf',
        install_dir: resources_dir,
    )
elif is_win
    install_data(
        meson.project_source_root() / 'conf' / 'qt.conf',
        install_dir: get_option('bindir'),
    )
endif

# macOS/Windows: bundle Qt and wrap the install tree into a .dmg / NSIS .exe
# at `meson install` time. See pack/deploy.py.
if is_mac or is_win
    meson.add_install_script(
        find_program('python3', 'python'),
        meson.current_source_dir() / 'deploy.py',
        host_sys,
        meson.project_version(),
        get_option('dev_build'),
        meson.project_source_root(),
    )
endif

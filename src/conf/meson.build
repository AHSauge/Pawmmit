conf_moc = qt6.preprocess(
    moc_headers: files(
        'Setting.h',
        'Settings.h',
        'RecentRepositories.h',
        'RecentRepository.h',
    ),
    dependencies: [qt6_dep, pawmmit_common],
)

conf_src = [
    'ConfFile.cpp',
    'Setting.cpp',
    'Settings.cpp',
    'RecentRepository.cpp',
    'RecentRepositories.cpp',
]

scintillua_lexers_src = scintillua_dep.get_variable(internal: 'lexers_dir')

# The plain macros are install locations (resources_dir / l10n_install_dir come
# from the root meson.build, platform-dependent); the SRC_ variants point back
# at the live source/build tree so an uninstalled build still finds resources.
# NOTE: Slash conversion needed for MSVC
conf_defines = [
    '-DCONF_DIR="' + resources_dir + '"',
    '-DSRC_CONF_DIR="' + (meson.project_source_root() / 'conf').replace('\\', '/') + '"',
    '-DL10N_DIR="' + l10n_install_dir + '"',
    '-DSRC_L10N_DIR="' + (meson.project_build_root() / 'l10n').replace('\\', '/') + '"',
    '-DSCINTILLUA_LEXERS_DIR="' + (resources_dir / 'lexers') + '"',
    '-DSRC_SCINTILLUA_LEXERS_DIR="' + scintillua_lexers_src.replace('\\', '/') + '"',
]

conf_lib = static_library(
    'conf',
    conf_src + conf_moc,
    cpp_args: conf_defines,
    dependencies: [pawmmit_common, pawmmitUtil_dep, translation_dep, lua_dep])

conf_dep = declare_dependency(
    link_with: conf_lib,
    include_directories: include_directories('.'),
    dependencies: [pawmmitUtil_dep, translation_dep],
)

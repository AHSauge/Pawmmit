qt_test_dep = dependency('qt6', modules: ['Test'])
zip_dep = dependency('zip')  # kuba--/zip, via subprojects/zip.wrap

# SSH_CONFIG is used by the individual tests too, so they
# have to propagate; TESTREPOSITORIES_PATH is only read by Test.cpp.
# NOTE: Slash conversion needed for MSVC
testlib_public_args = [
    '-DSSH_CONFIG="' + meson.current_source_dir().replace('\\', '/') + '"',
]

# Qt's test headers hide keyClick, qWaitForWindowActive and friends unless the
# module macros are defined, which the macOS Qt lookup doesn't do. This is a
# suspected inconsistency in Meson's handling of Qt frameworks on macOS.
if host_machine.system() == 'darwin'
    testlib_public_args += ['-DQT_GUI_LIB', '-DQT_WIDGETS_LIB']
endif

testlib = static_library(
    'testlib',
    files('Test.cpp'),
    cpp_args: testlib_public_args
        + ['-DTESTREPOSITORIES_PATH="' + (meson.current_source_dir() / 'testRepos').replace('\\', '/') + '"'],
    dependencies: [pawmmit_common, pawmmitUtil_dep, qt_test_dep, zip_dep, git_dep]
        + (host_machine.system() in ['darwin', 'windows'] ? [] : [dependency('qt6', modules: ['DBus'])]),
)

testlib_dep = declare_dependency(
    link_with: testlib,
    compile_args: testlib_public_args,
    dependencies: [pawmmit_common, pawmmitUtil_dep, qt_test_dep, zip_dep, git_dep],
)

tests = [
    'bare_repo',
    'init_repo',
    'merge',
    'external_tools_dialog',
    'branches_panel',
    'editor',
    'index',
    'line_endings',
    'log',
    'main_window',
    'new_branch_dialog',
    'sanity',
    'Diff',
    'EditorLineInfos',
    'CommitAuthorCommitter',
    'rebase',
    'TreeView',
    'Submodule',
    'referencelist',
    'amend',
    'SshConfig',
    'store',
    'fileContextMenu',
    'Setting',
    'commitMessageTemplate',
    'commitEditor',
    'clone_dialog',
    'language_combo_box',
    'theme_dialog',
    'button_tips',
    'RepositoryWatcher',
    'RepoViewAutoRefresh',
    'PathFilter',
    'RepoViewStaging',
]
if host_machine.system() != 'windows'
    tests += 'Filter'
endif
if get_option('ci_tests')
    tests += 'config'
endif

test_env = environment()
test_env.set('QT_QPA_PLATFORM', 'offscreen')

# Meson adds no rpath for Qt frameworks, so the tests can't load them from the
# build directory without one.
test_rpath = ''
if host_machine.system() == 'darwin' and _qtpaths.found()
    _libs = run_command(_qtpaths, '--query', 'QT_INSTALL_LIBS', check: false)
    if _libs.returncode() == 0
        test_rpath = _libs.stdout().strip()
    endif
endif

# Every test .cpp ends with #include "<name>.moc" except this one.
tests_without_moc = ['Diff']

# Both reach a real git remote, unavailable in offline build environments.
tests_needing_network = ['branches_panel', 'referencelist']

foreach name : tests
    sources = [files(name + '.cpp')]
    if name not in tests_without_moc
        sources += qt6.preprocess(
            moc_sources: files(name + '.cpp'),
            dependencies: [pawmmit_common, qt_test_dep],
        )
    endif
    exe = executable(
        'test_' + name,
        sources,
        cpp_args: app_public_args,
        link_whole: pawmmit_module_libs,
        dependencies: [testlib_dep] + pawmmit_link_deps,
        build_rpath: test_rpath,
        build_by_default: false,
    )
    suites = []
    if name in tests_needing_network
        suites = ['needs_network']
    endif
    test(name, exe, env: test_env, suite: suites, timeout: 300)
endforeach

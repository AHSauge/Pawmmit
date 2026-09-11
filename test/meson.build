qt_test_dep = dependency('qt6', modules: ['Test'])
zip_dep = dependency('zip')  # kuba--/zip, via subprojects/zip.wrap

# SSH_CONFIG and GIT_EXECUTABLE are used by the individual tests too, so they
# have to propagate; TESTREPOSITORIES_PATH is only read by Test.cpp.
testlib_public_args = [
    '-DSSH_CONFIG="' + meson.current_source_dir() + '"',
    '-DGIT_EXECUTABLE="' + find_program('git').full_path() + '"',
]

testlib = static_library(
    'testlib',
    files('Test.cpp'),
    cpp_args: testlib_public_args
        + ['-DTESTREPOSITORIES_PATH="' + meson.current_source_dir() / 'testRepos' + '"'],
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
]
if host_machine.system() != 'windows'
    tests += 'Filter'
endif
if get_option('ci_tests')
    tests += 'config'
endif

test_env = environment()
test_env.set('QT_QPA_PLATFORM', 'offscreen')

# Every test .cpp ends with #include "<name>.moc" except this one.
tests_without_moc = ['Diff']

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
        build_by_default: false,
    )
    # fileContextMenu opens a real context menu, which doesn't work under
    # offscreen QPA on Windows; CI excludes this suite there.
    suites = name == 'fileContextMenu' ? ['no_win32_offscreen'] : []
    test(name, exe, env: test_env, suite: suites, timeout: 300)
endforeach

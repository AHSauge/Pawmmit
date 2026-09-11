watcher_impl = {
    'darwin': 'RepositoryWatcher_mac.cpp',
    'windows': 'RepositoryWatcher_win.cpp',
    'linux': 'RepositoryWatcher_linux.cpp',
}.get(host_machine.system(), 'RepositoryWatcher_qt.cpp')

# The Q_OBJECT class (RepositoryWatcherPrivate) lives in the platform .cpp,
# which ends with #include "<name>.moc".
watcher_moc = qt6.preprocess(
    moc_sources: files(watcher_impl),
    dependencies: [qt6_dep, pawmmit_common],
)

watcher_deps = [pawmmit_common, git_dep]
if host_machine.system() == 'darwin'
    watcher_deps += dependency('appleframeworks', modules: ['CoreServices'])
endif

watcher_lib = static_library(
    'watcher',
    files('RepositoryWatcher.cpp', watcher_impl) + watcher_moc,
    dependencies: watcher_deps,
)

watcher_dep = declare_dependency(
    link_with: watcher_lib,
    include_directories: include_directories('.'),
)

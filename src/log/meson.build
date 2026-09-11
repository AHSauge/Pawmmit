loglib_moc = qt6.preprocess(
    moc_headers: files(
        'LogDelegate.h',
        'LogEntry.h',
        'LogModel.h',
        'LogView.h',
    ),
    dependencies: [pawmmit_common],
)

loglib_src = [
    'LogDelegate.cpp',
    'LogEntry.cpp',
    'LogModel.cpp',
    'LogView.cpp',
]

loglib_lib = static_library(
    'loglib',
    loglib_src + loglib_moc,
    dependencies: [pawmmit_common],
)

loglib_dep = declare_dependency(
    link_with: loglib_lib,
    include_directories: include_directories('.'),
)

cred_src = [
    'Cache.cpp',
    'CredentialHelper.cpp',
    'GitCredential.cpp',
    'Store.cpp',
]

cred_lib = static_library(
    'cred',
    cred_src,
    dependencies: [pawmmit_common, conf_dep, git_dep])

cred_dep = declare_dependency(
    link_with: cred_lib,
    include_directories: include_directories('.'),
)
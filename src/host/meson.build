host_config = configuration_data()
host_config.set_quoted('GITHUB_CLIENT_ID', get_option('github_client_id'))
host_config.set_quoted('GITHUB_CLIENT_SECRET', get_option('github_client_secret'))
host_config.set_quoted('GITEA_CLIENT_ID', get_option('gitea_client_id'))
host_config.set_quoted('GITEA_CLIENT_SECRET', get_option('gitea_client_secret'))

host_config_h = configure_file(
    output: 'host_config.h',
    configuration: host_config,
)

host_moc = qt6.preprocess(
    moc_headers: files(
        'Account.h',
        'Accounts.h',
        'Beanstalk.h',
        'Bitbucket.h',
        'GitHub.h',
        'Gitea.h',
        'GitLab.h',
        'Repository.h',
    ),
    dependencies: [qt6_dep, pawmmit_common],
)

host_src = [
    'Account.cpp',
    'Accounts.cpp',
    'Beanstalk.cpp',
    'Bitbucket.cpp',
    'GitHub.cpp',
    'Gitea.cpp',
    'GitLab.cpp',
    'Repository.cpp',
]

host_lib = static_library(
    'host',
    host_src + host_moc + [host_config_h],
    dependencies: [pawmmit_common, conf_dep, cred_dep])

host_dep = declare_dependency(
    link_with: host_lib,
    include_directories: include_directories('.'),
)

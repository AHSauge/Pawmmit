languages = [
    'de',
    'ru',
    'en',
    'es',
    'ja',
    'pt',
    'pt_BR',
    'sv',
    'zh_CN',
]


# Expand languages.cpp.inc into languages.cpp with the language table.
system_lang_key = 'System'
supported_languages = '{"' + system_lang_key + '", "' + system_lang_key + '"},'
ts_files = []
foreach language : languages
    ts_files += 'pawmmit_' + language + '.ts'
    supported_languages += ' {"' + language + '", "' + language + '"},'
endforeach

languages_conf = configuration_data()
languages_conf.set('SYSTEM_LANG_KEY', system_lang_key)
languages_conf.set('SUPPORTED_LANGUAGES', supported_languages)

languages_cpp = configure_file(
    input: 'languages.cpp.inc',
    output: 'languages.cpp',
    configuration: languages_conf,
)

# compile_translations only wraps lrelease (.ts -> .qm); regenerating the .ts
# files from source needs lupdate, wired as an explicit run_target below.
qm_files = qt6.compile_translations(
    ts_files: ts_files,
    build_by_default: true,
    install: true,
    install_dir: l10n_install_dir,
)

if get_option('update_translations')
    lupdate = find_program('lupdate6', 'lupdate', required: true)
    # Point lupdate at the source tree root; it recurses on its own.
    run_target(
        'update_translations',
        command: [
            lupdate,
            meson.project_source_root() / 'src',
            '-ts', files(ts_files),
        ],
    )
endif

translation_lib = static_library(
    'translation',
    languages_cpp,
    dependencies: qt6_dep,
)

translation_dep = declare_dependency(
    link_with: translation_lib,
    include_directories: include_directories('.'),
)

# Ship Qt's own base translations alongside ours, for the languages we support
# and only when that language's file is actually installed on the build host.
foreach language : languages
    qtbase_qm = qt_translations_dir / 'qtbase_' + language + '.qm'
    if fs.exists(qtbase_qm)
        install_data(qtbase_qm, install_dir: l10n_install_dir)
    endif
endforeach

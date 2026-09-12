editor_moc = qt6.preprocess(
    moc_headers: files(
        'TextEditor.h',
    ),
    dependencies: [pawmmit_common],
)

editor_src = [
    'TextEditor.cpp',
]

editor_lib = static_library(
    'editor',
    editor_src + editor_moc,
    dependencies: [pawmmit_common, lexilla_dep, scintilla_dep, scintillua_dep, ui_dep],
)

editor_dep = declare_dependency(
    link_with: editor_lib,
    include_directories: include_directories('.'),
    # TextEditor.h includes Scintilla headers, so anything using editor_dep
    # (ui, plugins) needs the Scintilla stack on its include path too.
    dependencies: [lexilla_dep, scintilla_dep, scintillua_dep],
)

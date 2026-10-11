PRAGMA user_version = 1;

CREATE TABLE commits (
    id          INTEGER PRIMARY KEY,
    oid         BLOB NOT NULL UNIQUE,
    time        INTEGER NOT NULL,
    tz_offset   INTEGER NOT NULL,
    email       TEXT NOT NULL
) STRICT;
CREATE INDEX commits_time ON commits(time);
CREATE INDEX commits_email ON commits(email);
CREATE INDEX commits_local_time ON commits(time + tz_offset);

CREATE TABLE paths (
    id          INTEGER PRIMARY KEY,
    path        TEXT NOT NULL UNIQUE,
    file        TEXT NOT NULL
) STRICT;
CREATE INDEX paths_file ON paths(file);

CREATE TABLE commits_paths (
    path_id     INTEGER NOT NULL,
    commit_id   INTEGER NOT NULL,
    PRIMARY KEY (path_id, commit_id)
) WITHOUT ROWID, STRICT;
CREATE INDEX commits_paths_commit ON commits_paths(commit_id);

CREATE VIRTUAL TABLE fts USING fts5(
    author,
    message_ident, message_string,
    scope_ident, scope_comment, scope_string,
    addition_ident, addition_comment, addition_string,
    deletion_ident, deletion_comment, deletion_string,
    content='', contentless_delete=1,
    detail=full,
    tokenize='pawmmit'
);

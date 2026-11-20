Libgit2 in Pawmmit
==================================
Libgit2 v2 is still not released. For that reason, Pawmmit is still compatible with v1.9. However, for optimal performance, it's **strongly** recommended to use the bundled v2.0.0-rc.1 as it contains several performance patches **substantially** increasing the performance. This is especially important when working with large repositories.

The following patches and corresponding upstream PRs are applied

|Patch #|PR|Description|
|----|----|----|
|libgit2-0001|https://github.com/libgit2/libgit2/pull/7357|Speeds up revwalk by using bloom filter. This improves operations on commit lists filtered by path|
|libgit2-0002|TBD|Speeds up topological revwalk by using generation number v1 data|
|libgit2-0003|https://github.com/libgit2/libgit2/pull/6863|Speeds up tree-to-tree diffs|
|libgit2-0004|https://github.com/libgit2/libgit2/pull/7410|Improves various operations by keeping most recently used elements in cache|
|libgit2-0005|https://github.com/libgit2/libgit2/pull/7446|Speeds up pushing if there are many refs|
|libgit2-0006|https://github.com/libgit2/libgit2/pull/7472|Up-to 30x performance boost to rename detection where a file was never moved|
|libgit2-0007|https://github.com/libgit2/libgit2/pull/7471|Up-to 1000x performance boost for rename detection of move-without-modify scenarios|
|libgit2-0008|https://github.com/libgit2/libgit2/pull/7484|Improves stash save performance|

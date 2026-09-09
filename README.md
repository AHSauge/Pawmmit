[![Pawmmit Status](https://github.com/Pawmmit/Pawmmit/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/Pawmmit/Pawmmit/actions/workflows/build.yml)

<!-- <a href="https://flathub.org/apps/details/com.github.Pawmmit.Pawmmit">
<img
    src="https://flathub.org/assets/badges/flathub-badge-i-en.png"
    alt="Download Pawmmit on Flathub"
    width="240px"/>
</a> -->

Pawmmit
==================================

Pawmmit is a graphical Git client designed to help you understand and manage your source code history. The [latest stable release](https://github.com/Pawmmit/Pawmmit/releases/latest)
is available either as pre-built flatpak for Linux, 32 / 64 binary for Windows, macOS,
or can be built from source by following the directions [below](https://github.com/Pawmmit/Pawmmit#how-to-build).

The [latest development version](https://github.com/Pawmmit/Pawmmit/releases/tag/development) is available pre-built as well.

Pawmmit is a fork of [Gittyup](https://github.com/Murmele/Gittyup), which is a
continuation of the [GitAhead](https://github.com/gitahead/gitahead) client.

![Pawmmit](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/main_dark_orig.png)

Table of contents
=================
<!--ts-->
- [Pawmmit](#pawmmit)
- [Table of contents](#table-of-contents)
  - [Features](#features)
  - [How to Get Help](#how-to-get-help)
  - [Build Environment](#build-environment)
  - [Dependencies](#dependencies)
  - [How to Build](#how-to-build)
    - [A Convenient Shell Script for Ubuntu is available here, and will install all the necessary prerequisites, and build a release version for immediate use.](#a-convenient-shell-script-for-ubuntu-is-available-here-and-will-install-all-the-necessary-prerequisites-and-build-a-release-version-for-immediate-use)
  - [How to Install](#how-to-install)
  - [How to Contribute](#how-to-contribute)
  - [License](#license)
<!--te-->

Features
---------------
To get an overview of the current features please have a look at the [GitHub Page](https://pawmmit.github.io/Pawmmit/)

How to Get Help
---------------

Ask questions about building or using Pawmmit on
[Stack Overflow](http://stackoverflow.com/questions/tagged/pawmmit) by
including the `pawmmit` tag. Remember to search for existing questions
before creating a new one.

Report bugs in Pawmmit by opening an issue in the
[issue tracker](https://github.com/Pawmmit/Pawmmit/issues).
Remember to search for existing issues before creating a new one.

Build Environment
-----------------

* C++11 compiler
  * Windows - MSVC >= 2017 recommended
  * Linux - GCC >= 6.2 recommended
  * macOS - Xcode >= 10.1 recommended
* CMake >= 3.19
* Ninja (optional)

Dependencies
------------

External dependencies can be satisfied by system libraries or installed
separately. Included dependencies are submodules of this repository. Some
submodules are optional or may also be satisfied by system libraries.

**External Dependencies**

* Qt (required >= 6.6)

**Included Dependencies**

* libgit2 (required)
* cmark (required)
* git (only needed for the credential helpers)
* libssh2 (needed by `libgit2` for SSH support)
* openssl (needed by `libssh2` and `libgit2` on some platforms)

Note that building `OpenSSL` on Windows requires `Perl` and `NASM`.

How to Build
------------

**Initialize Submodules**

    git submodule init
    git submodule update --depth 1

**Build OpenSSL**

    # Start from root of pawmmit repo.
    cd dep/openssl/openssl

Windows:

    perl Configure VC-WIN64A
    nmake

macOS (Intel):

    ./Configure darwin64-x86_64-cc no-shared
    make
    
macOS (Apple Silicon)

    ./Configure darwin64-arm64-cc no-shared
    make
    
Linux:

    ./config -fPIC
    make

**Configure Build**

    # Start from root of pawmmit repo.
    mkdir -p build/release
    cd build/release
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../..

If you have Qt installed in a non-standard location, you may have to
specify the path to Qt by passing `-DCMAKE_PREFIX_PATH=<path-to-qt>`
where `<path-to-qt>` points to the Qt install directory that contains
`bin`, `lib`, etc.

**Build**
```
    ninja
```
    
### A Convenient Shell Script for Ubuntu is available [here](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/pack/buildUbuntu.sh), and will install all the necessary prerequisites, and build a release version for immediate use.

How to Install
-----------------

TBD (build from source for now)

How to Contribute
-----------------

We welcome contributions of all kinds, including bug fixes, new features,
documentation and translations. By contributing, you agree to release
your contributions under the terms of the license.

Contribute by following the typical
[GitHub workflow](https://docs.github.com/en/get-started/quickstart/github-flow)
for pull requests. Fork the repository and make changes on a new named
branch. Create pull requests against the `main` branch. Follow the
[seven guidelines](https://chris.beams.io/posts/git-commit/) to writing a
great commit message.

Prior to committing a change, please use `cl-fmt.sh` to ensure your code
adheres to the formatting conventions for this project. You can also use the
`setup-env.sh` script to install a pre-commit hook which will automatically
run `clang-format` against all modified files.

Prior to pushing a change, please ensure you run the unit tests to avoid any
regressions. These are run using `ctest` in `<build-dir>`.

License
-------

This project is licensed under the GNU General Public License, version 3 or
(at your option) any later version. See [LICENSE.md](LICENSE.md) for the full
text.

Portions originate in Gittyup and its predecessor GitAhead, which were
published under the MIT license. That MIT notice is retained in
[NOTICE.md](NOTICE.md) and continues to apply to the pre-existing code; the
combined work is distributed under the GPL as stated above.

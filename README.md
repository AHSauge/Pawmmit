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

* C++17 compiler
  * Windows - MSVC >= 2019 recommended
  * Linux - GCC >= 9 / Clang >= 10 recommended
  * macOS - Xcode >= 12 recommended
* [Meson](https://mesonbuild.com) >= 1.1
* Ninja
* Python 3

Dependencies
------------

**Must be provided by the system** (install via your package manager /
Homebrew / vcpkg) - Meson can't build these itself:

* Qt (required >= 6.7)
* libgit2 (>= 1.9) - its CMake build doesn't translate to Meson cleanly
  enough to fall back on, so this is the one library every platform needs a
  real package for. On macOS, `brew install libgit2`; on Windows,
  `vcpkg install libgit2[ssh]` (see `.github/workflows/build.yml`).

On Debian/Ubuntu, for example:

    sudo apt install meson ninja-build pkg-config python3 \
        qt6-base-dev qt6-tools-dev libqt6core5compat6-dev libgit2-dev

**Fetched automatically** as Meson subprojects when no system package is
found (needs network on first configure; see `subprojects/*.wrap`) - install
the system package instead if you'd rather not build these from source:

* libssh2, hunspell (>= 1.7), cmark (library + the `cmark` command-line
  tool), lua (>= 5.3)
* scintilla, lexilla, scintillua, lpeg
* zip (test suite only)

How to Build
------------

    meson setup build
    meson compile -C build

For an optimized build pass `--buildtype=release` to `meson setup`. If Qt is in
a non-standard location, put its `bin` directory on `PATH` or set
`PKG_CONFIG_PATH` / `CMAKE_PREFIX_PATH` so Meson can find it.

**Run the tests**

    meson test -C build

**Install**

    meson install -C build --destdir <staging-dir>

On macOS and Windows `meson install` also runs the packaging step
(`pack/deploy.py`): it bundles Qt with `macdeployqt` / `windeployqt` and writes
a `.dmg` / NSIS `.exe` into `build/pack/`.

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

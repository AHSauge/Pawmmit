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
is available pre-built for Linux (Flatpak / AppImage), Windows (64-bit) and macOS,
or can be built from source by following the directions [below](https://github.com/Pawmmit/Pawmmit#how-to-build).

The [latest development version](https://github.com/Pawmmit/Pawmmit/releases/tag/development) is available pre-built as well.

Pawmmit is a fork of [Gittyup](https://github.com/Murmele/Gittyup), which is a
continuation of the [GitAhead](https://github.com/gitahead/gitahead) client. The rationale behind the fork is provided [here](docs/rationale.md)

![Pawmmit](https://raw.githubusercontent.com/Pawmmit/Pawmmit/main/rsrc/screenshots/main_dark_orig.png)

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
  - [How to Install](#how-to-install)
  - [How to Contribute](#how-to-contribute)
  - [License](#license)
<!--te-->

Features
---------------
To get an overview of the current features please have a look at the [GitHub Page](https://pawmmit.github.io/Pawmmit/)

How to Get Help
---------------

Ask questions about building or using Pawmmit in the
[discussions](https://github.com/Pawmmit/Pawmmit/discussions) section on GitHub.
Remember to search for existing questions before creating a new one.

Report bugs in Pawmmit by opening an issue in the
[issue tracker](https://github.com/Pawmmit/Pawmmit/issues).
Remember to search for existing issues before creating a new one.

Build Environment
-----------------

* C++17 compiler
  * Windows - MSVC >= 2019 recommended
  * Linux - GCC >= 9 / Clang >= 10 recommended
  * macOS - Xcode >= 12 recommended
* [Meson](https://mesonbuild.com) >= 1.3
* Ninja
* Python 3
* For AppImage:
  * `appstreamcli`

Dependencies
------------

* Required dependencies (must be installed via your package manager / Homebrew / vcpkg):
  * Qt (>= 6.7) (Core5Compat and Linguist tools are required)
    * Debian (or DEB based distros): `apt install qt6-base-dev qt6-tools-dev qt6-l10n-tools libqt6core5compat6-dev`
    * Fedora (RPM based distros): `dnf install qt6-qtbase-devel qt6-qttools-devel qt6-linguist qt6-qt5compat-devel`
  * libgit2 (>= 1.9)
    * macOS: `brew install libgit2`
    * Windows: `vcpkg install libgit2[ssh]`
    * Debian (or DEB based distros): `apt install libgit2-dev`
    * Fedora (RPM based distros): `dnf install libgit2-devel`
* Optionally installed dependencies (system package is chosen over bundling):
  * libssh2
  * hunspell (>= 1.7)
  * cmark (library + the `cmark` command-line tool)
  * lua (>= 5.3)
    * Optionally LuaJIT (>=2.1) can be used by passing `-Dluajit=enabled` to `meson setup`. lua-compat-5.3 is bundled to provide some of the Lua 5.3 C API.
* Bundled dependencies:
  * lexilla
  * scintilla
  * scintillua
  * lpeg
  * zip (test suite only)

For packaging without network access, the optional dependencies that aren't installed and the bundled dependencies need to be downloaded in advance. Run `meson subprojects download` beforehand, or pre-populate `subprojects/packagecache` with the archives (see `com.github.Pawmmit.Pawmmit.yml` for example).

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

How to Install
-----------------

See [releases](https://github.com/Pawmmit/Pawmmit/releases), or build from source.

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
regressions. These are run using `meson test -C <build-dir>`.

License
-------

This project is licensed under the GNU General Public License, version 3 or
(at your option) any later version. See [LICENSE.md](LICENSE.md) for the full
text.

Portions originate in Gittyup and its predecessor GitAhead, which were
published under the MIT license. That MIT notice is retained in
[NOTICE.md](NOTICE.md) and continues to apply to the pre-existing code; the
combined work is distributed under the GPL as stated above.

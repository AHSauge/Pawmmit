#!/usr/bin/env python3
"""Post-install packaging: bundles Qt and writes a .dmg (macOS) or NSIS .exe
(Windows) into <build>/pack/. Run via meson.add_install_script()."""
import os
import shutil
import subprocess
import sys
from pathlib import Path

# argv: <platform> <version> <dev_build> <source_root>
PLATFORM, VERSION, DEV_BUILD, SOURCE_ROOT = sys.argv[1:5]
SOURCE_ROOT = Path(SOURCE_ROOT)
PREFIX = Path(os.environ["MESON_INSTALL_DESTDIR_PREFIX"])
BUILD_ROOT = Path(os.environ["MESON_BUILD_ROOT"])
PACK_DIR = BUILD_ROOT / "pack"
PACK_DIR.mkdir(parents=True, exist_ok=True)

BUILD_TAG = "-dev" if DEV_BUILD else ""


def which(*names):
    for n in names:
        p = shutil.which(n)
        if p:
            return p
    sys.exit(f"pack/deploy.py: none of {names} found on PATH")


def run(*cmd):
    print("pack:", " ".join(map(str, cmd)), flush=True)
    subprocess.run(list(map(str, cmd)), check=True)


def deploy_macos():
    app = PREFIX / "pawmmit.app"
    if not app.is_dir():
        sys.exit(f"pack/deploy.py: {app} not found")
    macdeployqt = which("macdeployqt6", "macdeployqt")
    extra = []
    for helper in ("pawmmit-indexer", "pawmmit-relauncher"):
        h = app / "Contents" / "MacOS" / helper
        if h.exists():
            extra.append(f"-executable={h}")
    run(macdeployqt, app, *extra)

    dmg = PACK_DIR / f"Pawmmit-{VERSION}{BUILD_TAG}.dmg"
    if dmg.exists():
        dmg.unlink()
    run(
        which("hdiutil"), "create",
        "-volname", "Pawmmit",
        "-srcfolder", app,
        "-ov", "-format", "UDZO",
        dmg,
    )
    print(f"pack: wrote {dmg}")


def deploy_windows():
    exe = PREFIX / "bin" / "pawmmit.exe"
    if not exe.exists():
        sys.exit(f"pack/deploy.py: {exe} not found")
    windeployqt = which("windeployqt6", "windeployqt")
    run(windeployqt, "--release", "--dir", PREFIX / "bin", exe)
    for helper in ("pawmmit-indexer.exe", "pawmmit-relauncher.exe"):
        h = PREFIX / "bin" / helper
        if h.exists():
            run(windeployqt, "--release", "--dir", PREFIX / "bin", h)

    shutil.copy2(SOURCE_ROOT / "rsrc" / "vcredist_x64.exe", PREFIX / "vcredist_x64.exe")

    nsi = PACK_DIR / "pawmmit.nsi"
    template = (SOURCE_ROOT / "pack" / "pawmmit.nsi.in").read_text()
    nsi.write_text(
        template
        .replace("@VERSION@", VERSION)
        .replace("@STAGE_DIR@", str(PREFIX))
        .replace("@SOURCE_ROOT@", str(SOURCE_ROOT))
        .replace("@OUT_FILE@", str(PACK_DIR / f"Pawmmit-win64-{VERSION}{BUILD_TAG}.exe"))
    )
    run(which("makensis"), nsi)
    print(f"pack: wrote {PACK_DIR / f'Pawmmit-win64-{VERSION}{BUILD_TAG}.exe'}")


if PLATFORM == "darwin":
    deploy_macos()
elif PLATFORM == "windows":
    deploy_windows()
# linux: nothing here - the CI wraps `meson install` output with appimagetool.

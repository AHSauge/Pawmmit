#!/bin/bash

cd "`dirname "$0"`"

# Variable that will hold the name of the clang-format command
FMT=""

FOLDERS=("./src" "./test" "./l10n")

# We specifically require clang-format v19. Some distros include the version
# number in the name, others don't. Prefer the specifically-named version.
for clangfmt in clang-format-19 clang-format
do
    if command -v "$clangfmt" &>/dev/null; then
        FMT="$clangfmt"
        break
    fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

# Check we have v19 of clang-format
VERSION=`$FMT --version | grep -Po 'version\s\K(\d+)'`
if [ "$VERSION" != "19" ]; then
	echo "Found clang-format v$VERSION, but v19 is required. Please install v19 of clang-format and try again."
	echo "On Debian-derived distributions, this can be done via: apt install clang-format-19"
	echo "Alternatively, install the pinned wheel: pip install clang-format==19.1.7"
	exit 1
fi

function format() {
    for f in $(find $@ \( -type d -path './test/dep/*' -prune \) -o \( -name '*.h' -or -name '*.m' -or -name '*.mm' -or -name '*.c' -or -name '*.cpp' \)); do
        echo "format ${f}";
        ${FMT} -i ${f};
    done

    echo "~~~ $@ Done ~~~";
}

for dir in ${FOLDERS[@]}; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory";
    else
        format ${dir};
    fi
done

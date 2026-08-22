#!/bin/bash

set -euo pipefail

if [[ $# -ne 1 ]]
then
	echo "Usage: ${0##*/} <debug|release>" >&2
	exit 2
fi

buildType="${1,,}"
case "$buildType" in
	debug | release) ;;
	*)
		echo "Invalid build type '$1'. Expected debug or release." >&2
		exit 2
		;;
esac

if [[ -z "${ANDROID_NDK:-}" ]]
then
	echo "ANDROID_NDK must point to an installed Android NDK." >&2
	exit 1
fi

toolchainFile="$ANDROID_NDK/build/cmake/android.toolchain.cmake"
if [[ ! -f "$toolchainFile" ]]
then
	echo "Android toolchain file not found: $toolchainFile" >&2
	exit 1
fi

scriptDirectory="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$scriptDirectory"

cmake --preset "android-arm64-$buildType"
cmake --build --preset "android-arm64-$buildType"

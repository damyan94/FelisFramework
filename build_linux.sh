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

scriptDirectory="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$scriptDirectory"

cmake --preset "linux-$buildType"
cmake --build --preset "linux-$buildType"

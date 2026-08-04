#!/usr/bin/env sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$script_dir/.."

usage() {
    echo "Usage: $0 --platform host|stm32f1|stm32f3|stm32f7 [--with-tests]" >&2
    exit 2
}

platform=""
with_tests=false

while [ "$#" -gt 0 ]; do
    case "$1" in
        --platform)
            [ "$#" -ge 2 ] || usage
            platform="$2"
            shift 2
            ;;
        --with-tests)
            with_tests=true
            shift
            ;;
        *) usage ;;
    esac
done

case "$platform" in
    host) set -- ;;
    stm32f1) set -- external/cmsis external/cmsis-device-f1 external/stm32f1_hal ;;
    stm32f3) set -- external/cmsis external/cmsis-device-f3 external/stm32f3_hal ;;
    stm32f7) set -- external/cmsis external/cmsis-device-f7 external/stm32f7_hal ;;
    *) usage ;;
esac

if [ "$with_tests" = true ]; then
    set -- "$@" external/googletest
fi

if [ "$#" -gt 0 ]; then
    git submodule update --init "$@"
fi

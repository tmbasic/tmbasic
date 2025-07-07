#!/bin/bash
# Required variables: TARGET_OS, ARCH, PREFIX
# Required when TARGET_OS=linux: LINUX_DISTRO, LINUX_TRIPLE
set -euxo pipefail

# Change to the repository root.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ..

# This script doesn't support Windows.
if [[ "$TARGET_OS" == "win" ]]; then
    echo "This script doesn't support Windows."
    exit 1
fi

# Avoid bash error about uninitialized variables when building for non-Linux.
if [[ "$TARGET_OS" != "linux" ]]; then
    LINUX_DISTRO=""
    LINUX_TRIPLE=""
fi

mkdir -p cmake
cd cmake

# If TARGET_OS is "linux" and LINUX_DISTRO is "ubuntu", then we need to use the "-dev" toolchain suffix.
TOOLCHAIN_SUFFIX=""
if [[ "$TARGET_OS" == "linux" && "$LINUX_DISTRO" == "ubuntu" ]]; then
    TOOLCHAIN_SUFFIX="-dev"
fi

TOOLCHAIN_FILE="../build/files/cmake-toolchain-${TARGET_OS}-${ARCH}${TOOLCHAIN_SUFFIX}.cmake"

# Log all variables
set +x
echo "TARGET_OS: $TARGET_OS"
echo "LINUX_DISTRO: $LINUX_DISTRO"
echo "LINUX_TRIPLE: $LINUX_TRIPLE"
echo "ARCH: $ARCH"
echo "PREFIX: $PREFIX"
echo "TOOLCHAIN_SUFFIX: $TOOLCHAIN_SUFFIX"
echo "TOOLCHAIN_FILE: $TOOLCHAIN_FILE"
echo "DISABLE_SANITIZERS: $DISABLE_SANITIZERS"
set -x

cmake \
    "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE" \
    -DCMAKE_VERBOSE_MAKEFILE=OFF \
    -DTARGET_OS=$TARGET_OS \
    -DLINUX_DISTRO=$LINUX_DISTRO \
    -DLINUX_TRIPLE=$LINUX_TRIPLE \
    -DARCH=$ARCH \
    "-DTARGET_PREFIX=$PREFIX" \
    -DDISABLE_SANITIZERS=$DISABLE_SANITIZERS \
    "$@" \
    ..

cmake --build .

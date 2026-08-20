#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ABI="arm64-v8a"
BUILD_TYPE="${BUILD_TYPE:-Release}"
NDK_VERSION="28.2.13676358"

if [[ -n "${ANDROID_NDK_HOME:-}" ]]; then
  NDK="$ANDROID_NDK_HOME"
elif [[ -n "${ANDROID_NDK_ROOT:-}" ]]; then
  NDK="$ANDROID_NDK_ROOT"
elif [[ -n "${ANDROID_HOME:-}" ]]; then
  NDK="$ANDROID_HOME/ndk/$NDK_VERSION"
elif [[ -n "${ANDROID_SDK_ROOT:-}" ]]; then
  NDK="$ANDROID_SDK_ROOT/ndk/$NDK_VERSION"
else
  echo "Android SDK/NDK not configured." >&2
  exit 1
fi

TOOLCHAIN="$NDK/build/cmake/android.toolchain.cmake"
if [[ ! -f "$TOOLCHAIN" ]]; then
  echo "NDK toolchain not found: $TOOLCHAIN" >&2
  exit 1
fi

BUILD_DIR="$ROOT/build/android-$ABI-$BUILD_TYPE"
DIST_DIR="$ROOT/dist/$ABI"
PACKAGE_DIR="$DIST_DIR/levi-freecam"

cmake -S "$ROOT" -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
  -DANDROID_ABI="$ABI" \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_STL=c++_shared \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

cmake --build "$BUILD_DIR" --target levi_freecam

rm -rf "$DIST_DIR"
mkdir -p "$PACKAGE_DIR"
cp "$ROOT/manifest.json" "$PACKAGE_DIR/manifest.json"
cp "$BUILD_DIR/out/$ABI/liblevi_freecam.so" "$PACKAGE_DIR/liblevi_freecam.so"

(
  cd "$PACKAGE_DIR"
  zip -qr "$DIST_DIR/levi-freecam.levipack" .
)

echo "Built: $DIST_DIR/levi-freecam.levipack"

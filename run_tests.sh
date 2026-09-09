#!/usr/bin/env bash
# Builds and runs the Atari Go test suite.
set -euo pipefail

cd "$(dirname "$0")"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:--std=c++17 -Wall -Wextra -Wpedantic -O2}
BUILD_DIR=build
TEST_BIN="$BUILD_DIR/game_test"

echo "Building tests with $CXX..."
mkdir -p "$BUILD_DIR"

# shellcheck disable=SC2086
$CXX $CXXFLAGS -Isrc \
    src/board.cpp src/game.cpp src/console_view.cpp src/gui_view.cpp src/match.cpp \
    test/game_test.cpp \
    -o "$TEST_BIN"

echo "Running tests..."
if "./$TEST_BIN"; then
    echo "All tests passed."
else
    status=$?
    echo "Tests failed." >&2
    exit "$status"
fi
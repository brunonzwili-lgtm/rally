#!/bin/bash
# Build SuperTuxKart for Web (Emscripten/WASM)
# Usage: ./build_web.sh [debug|release]

set -e

BUILD_TYPE="${1:-release}"
BUILD_TYPE_UPPER="$(echo "$BUILD_TYPE" | tr '[:lower:]' '[:upper:]')"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/cmake_build_web"

# Check for Emscripten
if [ -z "${EMSDK}" ] && [ -z "${EMSCRIPTEN}" ]; then
    if command -v emcc &> /dev/null; then
        EMCC_PATH=$(which emcc)
        export EMSCRIPTEN=$(dirname "${EMCC_PATH}")
        echo "Found emcc at: ${EMSCRIPTEN}"
    else
        echo "Error: Emscripten not found. Please install EMSDK and source it:"
        echo "  git clone https://github.com/emscripten-core/emsdk.git"
        echo "  cd emsdk && ./emsdk install latest && ./emsdk activate latest"
        echo "  source ./emsdk_env.sh"
        exit 1
    fi
fi

if [ -n "${EMSDK}" ]; then
    export EMSCRIPTEN="${EMSDK}/upstream/emscripten"
fi

echo "Using Emscripten at: ${EMSCRIPTEN}"
emcc --version

# Check for stk-assets
if [ ! -d "${SCRIPT_DIR}/../stk-assets" ]; then
    echo "Warning: stk-assets not found at ${SCRIPT_DIR}/../stk-assets"
    echo "You need to checkout stk-assets for the game to work:"
    echo "  svn co https://svn.code.sf.net/p/supertuxkart/code/stk-assets ../stk-assets"
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/Toolchain-emscripten.cmake" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE_UPPER}" \
    -DUSE_EMSCRIPTEN=ON \
    -DSERVER_ONLY=OFF \
    -DCHECK_ASSETS=OFF \
    -DBUILD_RECORDER=OFF \
    -DNO_SHADERC=ON \
    -DUSE_WIIUSE=OFF \
    -DUSE_GLES2=ON \
    -G Ninja

# Build
echo "Building..."
cmake --build . --config "${BUILD_TYPE_UPPER}" -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Package assets if stk-assets exists
if [ -d "${SCRIPT_DIR}/../stk-assets" ]; then
    echo ""
    echo "Packaging assets..."
    cd "${SCRIPT_DIR}/web"
    python3 pack_assets.py --assets-dir ../stk-assets --output-dir "${BUILD_DIR}/web" --preload
    cd "${BUILD_DIR}"
fi

echo ""
echo "Build complete!"
echo "Output files in: ${BUILD_DIR}/web/"
ls -la "${BUILD_DIR}/web/"

# Create a simple test HTML if it doesn't exist
if [ ! -f "${BUILD_DIR}/web/supertuxkart.html" ]; then
    echo "Note: Expected supertuxkart.html not found. Checking for other outputs..."
    find "${BUILD_DIR}" -name "*.html" -o -name "*.js" -o -name "*.wasm" | head -20
fi
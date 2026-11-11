#!/bin/bash
set -e

# build_cimgui.sh: Compiles cimgui.so including native AMD FSR 1.0 pipeline
# Usage:
#   ./scripts/build_cimgui.sh [x64|arm64]

TARGET_ARCH="${1:-x64}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
WORK_DIR="${ROOT_DIR}/.cimgui_build"

echo "=== Building cimgui.so with FSR 1.0 for architecture: ${TARGET_ARCH} ==="

if [ ! -d "${WORK_DIR}/CimguiSDL2Cross" ]; then
    echo "Cloning CimguiSDL2Cross (shallow)..."
    mkdir -p "${WORK_DIR}"
    git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/exelix11/CimguiSDL2Cross "${WORK_DIR}/CimguiSDL2Cross"
fi

cd "${WORK_DIR}/CimguiSDL2Cross"
if [ -f "apply_patches.sh" ]; then
    bash apply_patches.sh || true
fi

if [ "${TARGET_ARCH}" = "arm64" ] || [ "${TARGET_ARCH}" = "aarch64" ]; then
    CXX="aarch64-linux-gnu-g++"
    OUTPUT_DIR="${ROOT_DIR}/Client/Platform/Resources/linux-arm64/native"
    # Download SDL2 headers if not present for ARM64 cross compilation
    if [ ! -d "sdl2_headers" ]; then
        curl -sSL -o sdl2.tar.gz https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.1.tar.gz
        tar -xzf sdl2.tar.gz
        mv SDL-release-2.30.1/include sdl2_headers
        rm -rf sdl2.tar.gz SDL-release-2.30.1
    fi
    SDL_INC="-Isdl2_headers"
else
    CXX="g++"
    OUTPUT_DIR="${ROOT_DIR}/Client/Platform/Resources/linux-x64/native"
    SDL_INC="$(pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2")"
fi

mkdir -p "${OUTPUT_DIR}"
mkdir -p "build_${TARGET_ARCH}"
cd "build_${TARGET_ARCH}"

echo "Compiling cimgui.so using ${CXX}..."
${CXX} -shared -fPIC -O2 \
    -I../cimgui -I../cimgui/imgui -I../cimgui/imgui/backends \
    -I"${ROOT_DIR}/Client/Native/fsr" \
    ${SDL_INC} \
    -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 \
    -DIMGUI_IMPL_API='extern "C"' \
    ../cimgui/cimgui.cpp \
    ../cimgui/imgui/imgui.cpp \
    ../cimgui/imgui/imgui_draw.cpp \
    ../cimgui/imgui/imgui_demo.cpp \
    ../cimgui/imgui/imgui_widgets.cpp \
    ../cimgui/imgui/imgui_tables.cpp \
    ../cimgui/imgui/backends/imgui_impl_sdl2.cpp \
    ../cimgui/imgui/backends/imgui_impl_sdlrenderer2.cpp \
    "${ROOT_DIR}/Client/Native/fsr/fsr_pipeline.cpp" \
    -o cimgui.so

cp cimgui.so "${OUTPUT_DIR}/cimgui.so"
echo "=== cimgui.so successfully built and copied to: ${OUTPUT_DIR}/cimgui.so ==="

# Verify FSR symbols
nm -D cimgui.so | grep " fsr_" || true

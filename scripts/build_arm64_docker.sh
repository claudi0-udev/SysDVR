#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Building SysDVR-Client and cimgui.so (ARM64 with FSR) via Docker ==="

docker run --rm \
    -v "${ROOT_DIR}:/src" \
    -w /src \
    mcr.microsoft.com/dotnet/sdk:9.0 \
    bash -c '
set -e
echo "Installing ARM64 cross-compiler..."
apt-get update -qq && apt-get install -y -qq gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu curl git > /dev/null

echo "Downloading SDL2 headers..."
mkdir -p /tmp/sdl2
curl -sSL -o /tmp/sdl2.tar.gz https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.1.tar.gz
tar -xzf /tmp/sdl2.tar.gz -C /tmp/sdl2

echo "Fetching CimguiSDL2Cross..."
mkdir -p /tmp/cimgui
git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/exelix11/CimguiSDL2Cross /tmp/cimgui/CimguiSDL2Cross

cd /tmp/cimgui/CimguiSDL2Cross
if [ -f "apply_patches.sh" ]; then
    bash apply_patches.sh || true
fi

cp -r /tmp/sdl2/SDL-release-2.30.1/include/* cimgui/imgui/backends/

echo "Compiling cimgui.so for ARM64 with FSR..."
mkdir -p build_arm64 && cd build_arm64
aarch64-linux-gnu-g++ -shared -fPIC -O2 \
    -I../cimgui -I../cimgui/imgui -I../cimgui/imgui/backends \
    -I/src/Client/Native/fsr \
    -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 \
    -DIMGUI_IMPL_API="extern \"C\"" \
    ../cimgui/cimgui.cpp \
    ../cimgui/imgui/imgui.cpp \
    ../cimgui/imgui/imgui_draw.cpp \
    ../cimgui/imgui/imgui_demo.cpp \
    ../cimgui/imgui/imgui_widgets.cpp \
    ../cimgui/imgui/imgui_tables.cpp \
    ../cimgui/imgui/backends/imgui_impl_sdl2.cpp \
    ../cimgui/imgui/backends/imgui_impl_sdlrenderer2.cpp \
    /src/Client/Native/fsr/fsr_pipeline.cpp \
    -o cimgui.so

mkdir -p /src/Client/Platform/Resources/linux-arm64/native
cp cimgui.so /src/Client/Platform/Resources/linux-arm64/native/cimgui.so

echo "Verifying ARM64 cimgui.so symbols..."
aarch64-linux-gnu-nm -D cimgui.so | grep " fsr_" || true

echo "Publishing SysDVR-Client for linux-arm64..."
cd /src
dotnet publish Client/Client.csproj -c Release -r linux-arm64 -p:PublishAot=false --self-contained true -o /src/published-arm64
cp /src/Client/Platform/Resources/linux-arm64/native/cimgui.so /src/published-arm64/cimgui.so
echo "Build complete! Artifacts are in published-arm64/"
'

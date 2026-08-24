#!/bin/bash
# ==============================================================================
# Script de Instalación Automática de SysDVR para ROCKNIX (ARM64)
# ==============================================================================
# Repositorio: https://github.com/claudi0-udev/SysDVR
# ==============================================================================

set -e

PORTS_DIR="/storage/roms/ports"
SYSDVR_DIR="${PORTS_DIR}/.sysdvr_extracted"
SYSDVR_APPIMAGE="${SYSDVR_DIR}/SysDVR-Client-aarch64.AppImage"
SYSDVR_URL="https://github.com/claudi0-udev/SysDVR/releases/download/v6.0.0-arm64/SysDVR-Client-aarch64.AppImage"

echo "=================================================================="
echo "📦 Instalando SysDVR-Client (ARM64) en ROCKNIX..."
echo "=================================================================="

# Crear directorio
mkdir -p "${SYSDVR_DIR}"

# Descargar AppImage de SysDVR
curl -sSL -o "${SYSDVR_APPIMAGE}" "${SYSDVR_URL}"
chmod +x "${SYSDVR_APPIMAGE}"

# Extraer AppImage para evitar problemas de montaje FUSE
cd "${SYSDVR_DIR}"
rm -rf app
rm -rf squashfs-root
"${SYSDVR_APPIMAGE}" --appimage-extract > /dev/null 2>&1
mv squashfs-root app

# Crear script lanzador SysDVR.sh
cat > "${PORTS_DIR}/SysDVR.sh" << 'EOF'
#!/bin/bash
export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export SDL_VIDEODRIVER=wayland
export SDL_AUDIODRIVER=pulseaudio
export LD_PRELOAD=/usr/lib/libSDL2-2.0.so.0

cd /storage/roms/ports/.sysdvr_extracted/app
exec ./AppRun usb
EOF
chmod +x "${PORTS_DIR}/SysDVR.sh"

echo "=================================================================="
echo "✅ SysDVR instalado con éxito en /storage/roms/ports/SysDVR.sh"
echo "=================================================================="

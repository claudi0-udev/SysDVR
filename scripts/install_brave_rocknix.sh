#!/bin/bash
# ==============================================================================
# Script de Instalación Automática de Brave Browser para ROCKNIX (ARM64)
# ==============================================================================

set -e

PORTS_DIR="/storage/roms/ports"
CONFIG_DIR="/storage/.config"
BRAVE_DIR="${PORTS_DIR}/.brave"
BRAVE_URL="https://github.com/ivan-hc/Brave-appimage/releases/download/continuous-stable/Brave-Web-Browser-stable-1.93.138-aarch64.AppImage"

echo "=================================================================="
echo "🌐 Instalando Brave Web Browser (ARM64) en ROCKNIX..."
echo "=================================================================="

mkdir -p "${BRAVE_DIR}"
mkdir -p "${CONFIG_DIR}/brave"

cd "${BRAVE_DIR}"
rm -rf app
rm -f Brave-aarch64.AppImage

# Descargar AppImage
wget -c "${BRAVE_URL}" -O Brave-aarch64.AppImage
chmod +x Brave-aarch64.AppImage

# Extraer AppImage
mkdir -p app
./Brave-aarch64.AppImage --appimage-extract > /dev/null 2>&1
mv squashfs-root app/

# Crear script lanzador Brave.sh
cat > "${PORTS_DIR}/Brave.sh" << 'EOF'
#!/bin/bash
pkill -9 -f brave 2>/dev/null || true
rm -rf /storage/.config/brave/Singleton* 2>/dev/null

export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export DBUS_SESSION_BUS_ADDRESS=disabled:

cd /storage/roms/ports/.brave/app/squashfs-root
exec ./AppRun --no-sandbox \
  --user-data-dir=/storage/.config/brave \
  --enable-features=UseOzonePlatform \
  --ozone-platform=wayland \
  "$@"
EOF
chmod +x "${PORTS_DIR}/Brave.sh"

echo "=================================================================="
echo "✅ Brave Browser instalado con éxito en /storage/roms/ports/Brave.sh"
echo "=================================================================="

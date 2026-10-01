#!/bin/bash
# ==============================================================================
# Script de Actualización Automática de Brave Browser para ROCKNIX (ARM64)
# ==============================================================================
# Repositorio de lanzamientos: https://github.com/ivan-hc/Brave-appimage
# ==============================================================================

set -e

PORTS_DIR="/storage/roms/ports"
BRAVE_DIR="${PORTS_DIR}/.brave"
APP_DIR="${BRAVE_DIR}/app"

echo "=================================================================="
echo "🔄 Buscando actualizaciones de Brave Browser (ARM64)..."
echo "=================================================================="

# Obtener URL del último release aarch64 desde GitHub API
echo "🔍 Consultando la versión más reciente en GitHub..."
LATEST_URL=$(curl -sSL "https://api.github.com/repos/ivan-hc/Brave-appimage/releases/latest" | grep -i "browser_download_url" | grep -i "aarch64.AppImage" | head -n 1 | cut -d '"' -f 4)

if [ -z "$LATEST_URL" ]; then
    echo "❌ Error: No se pudo obtener la URL de descarga de la última versión."
    exit 1
fi

LATEST_FILENAME=$(basename "$LATEST_URL")
echo "📌 Última versión disponible: ${LATEST_FILENAME}"

mkdir -p "${BRAVE_DIR}"

# Comprobar si ya tenemos esta versión descargada
if [ -f "${BRAVE_DIR}/${LATEST_FILENAME}" ]; then
    echo "✅ Ya tienes instalada la versión más reciente de Brave Browser (${LATEST_FILENAME})."
    echo "No se requieren cambios."
    exit 0
fi

echo "📥 Descargando e instalando nueva versión..."

cd "${BRAVE_DIR}"

# Limpiar AppImages antiguas
rm -f Brave-*.AppImage Brave-aarch64.AppImage

# Descargar la nueva versión
wget -c "${LATEST_URL}" -O "${LATEST_FILENAME}"
chmod +x "${LATEST_FILENAME}"
ln -sf "${LATEST_FILENAME}" Brave-aarch64.AppImage

# Extraer para compatibilidad con ROCKNIX (evita FUSE)
echo "📦 Extrayendo archivos para ROCKNIX..."
rm -rf app
mkdir -p app
./Brave-aarch64.AppImage --appimage-extract > /dev/null 2>&1
mv squashfs-root app/

# Asegurar que el script lanzador Brave.sh esté actualizado y con permisos
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
echo "🎉 ¡Brave Browser se ha actualizado con éxito a la versión ${LATEST_FILENAME}!"
echo "=================================================================="

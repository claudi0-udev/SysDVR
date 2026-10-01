#!/bin/bash
pkill -9 -f brave 2>/dev/null || true
pkill -9 gptokeyb 2>/dev/null || true
rm -rf /storage/.config/brave/Singleton* 2>/dev/null

export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export DBUS_SESSION_BUS_ADDRESS=disabled:

# Iniciar gptokeyb mapeando el mando como raton y teclas
controlfolder="/storage/roms/ports/PortMaster"
if [ -f "$controlfolder/control.txt" ]; then
    source "$controlfolder/control.txt" 2>/dev/null || true
    source "$controlfolder/device_info.txt" 2>/dev/null || true
fi

# Lanzar el mapeador de mando en segundo plano
if [ -x "$controlfolder/gptokeyb" ]; then
    "$controlfolder/gptokeyb" -c "/storage/roms/ports/.brave/brave.gptk" &
    GPTOKEYB_PID=$!
elif [ -x "/usr/bin/gptokeyb" ]; then
    /usr/bin/gptokeyb -c "/storage/roms/ports/.brave/brave.gptk" &
    GPTOKEYB_PID=$!
fi

# Iniciar Brave Browser
cd /storage/roms/ports/.brave/app/squashfs-root
./AppRun --no-sandbox \
  --user-data-dir=/storage/.config/brave \
  --enable-features=UseOzonePlatform \
  --ozone-platform=wayland \
  "$@"

# Al salir de Brave, matar el proceso del mapeador del mando
if [ -n "$GPTOKEYB_PID" ]; then
    kill -9 "$GPTOKEYB_PID" 2>/dev/null || true
fi
pkill -9 gptokeyb 2>/dev/null || true

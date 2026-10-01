#!/bin/bash
pkill -9 -f brave 2>/dev/null || true
pkill -9 gptokeyb 2>/dev/null || true
pkill -9 gptokeyb2 2>/dev/null || true
rm -rf /storage/.config/brave/Singleton* 2>/dev/null

export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export DBUS_SESSION_BUS_ADDRESS=disabled:

controlfolder="/storage/roms/ports/PortMaster"

# Iniciar gptokeyb2 (que soporta mouse_wheel_up/down nativamente en los sticks analogicos)
if [ -x "$controlfolder/gptokeyb2" ] && [ -f "$controlfolder/libinterpose.aarch64.so" ]; then
    LD_LIBRARY_PATH="$controlfolder:$LD_LIBRARY_PATH" \
    LD_PRELOAD="$controlfolder/libinterpose.aarch64.so" \
    "$controlfolder/gptokeyb2" "AppRun" -c "/storage/roms/ports/.brave/brave.gptk" &
    GPTOKEYB_PID=$!
elif [ -x "$controlfolder/gptokeyb" ]; then
    "$controlfolder/gptokeyb" -c "/storage/roms/ports/.brave/brave.gptk" &
    GPTOKEYB_PID=$!
fi

# Iniciar Brave Browser
cd /storage/roms/ports/.brave/app/squashfs-root
./AppRun --no-sandbox \
  --user-data-dir=/storage/.config/brave \
  --enable-features=UseOzonePlatform \
  --ozone-platform=wayland \
  "$@"

# Matar el mapeador del mando al salir de Brave
if [ -n "$GPTOKEYB_PID" ]; then
    kill -9 "$GPTOKEYB_PID" 2>/dev/null || true
fi
pkill -9 gptokeyb2 2>/dev/null || true
pkill -9 gptokeyb 2>/dev/null || true

#!/bin/bash
export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export SDL_VIDEODRIVER=wayland
export SDL_AUDIODRIVER=pulseaudio

pkill -9 moonlight 2>/dev/null || true

clear
echo "=================================================================="
echo "          INICIANDO TRANSMISION MOONLIGHT                         "
echo "=================================================================="
echo ""
echo "Conectando al servidor (192.168.1.64)..."
echo "Para salir de la transmision presiona: SELECT + START"
echo ""

exec moonlight stream -config /storage/.config/moonlight/moonlight.conf -app "Desktop"

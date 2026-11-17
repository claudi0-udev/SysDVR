#!/bin/bash
export XDG_RUNTIME_DIR=/var/run/0-runtime-dir
export WAYLAND_DISPLAY=wayland-1
export SDL_VIDEODRIVER=wayland
export SDL_AUDIODRIVER=pulseaudio

CONF_FILE="/storage/.config/moonlight/moonlight.conf"
IP_FILE="/storage/.config/moonlight/server_ip.txt"

clear
echo "=================================================================="
echo "          INICIANDO TRANSMISION MOONLIGHT                         "
echo "=================================================================="

PC_IP=""
if [ -f "$IP_FILE" ]; then
    PC_IP=$(cat "$IP_FILE" 2>/dev/null)
fi

if [ -z "$PC_IP" ]; then
    echo "No has vinculado tu PC todavia."
    echo "Por favor ejecuta primero 'Moonlight (Vincular PC)' desde Ports."
    sleep 4
    exit 1
fi

echo "Conectando a tu PC ($PC_IP)..."
echo "Para salir de la transmision presiona: SELECT + START o Ctrl+Alt+Shift+Q"
echo ""

# Stream usando aceleracion de hardware rk (Rockchip RK3588) o sdl
moonlight stream "$PC_IP" -config "$CONF_FILE" -app "Desktop" 2>&1 || moonlight stream "$PC_IP" -app "Desktop"

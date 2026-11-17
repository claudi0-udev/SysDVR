#!/bin/bash
clear
echo "=================================================================="
echo "          VINCULAR MOONLIGHT CON TU PC - PAIRING                  "
echo "=================================================================="
echo ""

# Archivo de configuracion
CONF_FILE="/storage/.config/moonlight/moonlight.conf"
IP_FILE="/storage/.config/moonlight/server_ip.txt"

mkdir -p /storage/.config/moonlight

CURRENT_IP=""
if [ -f "$IP_FILE" ]; then
    CURRENT_IP=$(cat "$IP_FILE" 2>/dev/null)
fi

echo "Ingresa la direccion IP de tu PC (por ejemplo 192.168.1.7):"
if [ -n "$CURRENT_IP" ]; then
    echo "[IP guardada actualmente: $CURRENT_IP - Presiona Enter para usarla]"
fi

read -r INPUT_IP

if [ -n "$INPUT_IP" ]; then
    PC_IP="$INPUT_IP"
elif [ -n "$CURRENT_IP" ]; then
    PC_IP="$CURRENT_IP"
else
    echo "No se ingreso ninguna IP. Cancelando."
    sleep 3
    exit 1
fi

echo "$PC_IP" > "$IP_FILE"

# Actualizar IP en moonlight.conf
if [ -f "$CONF_FILE" ]; then
    sed -i '/^address =/d' "$CONF_FILE"
    echo "address = $PC_IP" >> "$CONF_FILE"
fi

echo ""
echo "Conectando con $PC_IP..."
echo "Aparecera un codigo PIN de 4 digitos a continuacion."
echo "Ve a la web de Sunshine en tu PC (https://localhost:47990) -> pestaña PIN e ingresalo."
echo ""

moonlight pair "$PC_IP"

echo ""
echo "Presiona cualquier tecla o espera 5 segundos para volver..."
read -t 5 -n 1 || sleep 5

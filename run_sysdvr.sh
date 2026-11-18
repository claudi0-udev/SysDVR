#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${DIR}/published-x64:${LD_LIBRARY_PATH}"
export LD_PRELOAD="/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0"
exec "${DIR}/published-x64/SysDVR-Client" usb --fsr "$@"

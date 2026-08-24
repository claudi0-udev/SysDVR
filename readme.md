# SysDVR (Linux ARM64 / aarch64 Fork)

[![ARM64 Release](https://img.shields.io/github/v/release/claudi0-udev/SysDVR?label=ARM64%20Release&color=blue)](https://github.com/claudi0-udev/SysDVR/releases/tag/v6.0.0-arm64)
[![ARM64 AppImage Download](https://img.shields.io/badge/Download-SysDVR--Client--aarch64.AppImage-success)](https://github.com/claudi0-udev/SysDVR/releases/download/v6.0.0-arm64/SysDVR-Client-aarch64.AppImage)
[![Discord](https://img.shields.io/discord/643436008452521984.svg?logo=discord&logoColor=white&label=Discord&color=7289DA)](https://discord.gg/rqU5Tf8)

> **This fork by [`claudi0-udev`](https://github.com/claudi0-udev/SysDVR) adds native Linux ARM64 (`aarch64`) support, automated GitHub Actions CI/CD workflows, and standalone AppImage releases for SysDVR-Client.**

Designed and optimized for Single Board Computers (SBCs) and Linux ARM64 hardware:
- **Orange Pi 5 / 5 Plus / 5B** (DietPi, Ubuntu ARM64, Armbian)
- **Raspberry Pi 4 / 5** (Raspberry Pi OS 64-bit, Ubuntu ARM64)
- **Rockchip, Pine64, Nvidia Jetson, and other ARM64 Linux systems**

<p align="center">
  <img src="https://raw.githubusercontent.com/exelix11/SysDVR/master/.github/images/Screenshot.jpg" width="50%">
</p>

---

## 🚀 Quick Start on Linux ARM64

### 1. Download the ARM64 AppImage
Download the latest [`SysDVR-Client-aarch64.AppImage`](https://github.com/claudi0-udev/SysDVR/releases/download/v6.0.0-arm64/SysDVR-Client-aarch64.AppImage) directly from the [v6.0.0-arm64 Release Page](https://github.com/claudi0-udev/SysDVR/releases/tag/v6.0.0-arm64).

### 2. Configure USB Permissions (Required for USB Mode)
Run this once on your ARM64 device to allow access to Nintendo Switch USB stream nodes:
```bash
cat << 'EOF' | sudo tee /etc/udev/rules.d/99-sysdvr.rules
SUBSYSTEM=="usb", ATTRS{idVendor}=="057e", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="18d1", ATTRS{idProduct}=="4ee0", MODE="0666"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
```

### 3. Launch SysDVR-Client
```bash
chmod +x SysDVR-Client-aarch64.AppImage

# USB Mode (Stream to Screen)
sudo ./SysDVR-Client-aarch64.AppImage usb

# Record Gameplay directly to MP4 Video File
sudo ./SysDVR-Client-aarch64.AppImage usb --file gameplay.mp4
```

---

## ✨ Key Features & ARM64 Enhancements

- **Self-Contained AppImage**: Pre-packaged with .NET 9 runtime, native ARM64 dynamic libraries (`cimgui.so`), SDL2, and OpenGL dependencies.
- **Native `cimgui.so` Bindings**: Recompiled with C-linkage export definitions (`extern "C"`) to ensure symbol compatibility on `aarch64-linux-gnu`.
- **Vector256 Audio Acceleration**: Uses SIMD acceleration for real-time 16-bit PCM 48kHz audio processing.
- **Desktop Launcher Friendly**: Double-clicking `SysDVR-Client-aarch64.AppImage` from your desktop or file manager automatically defaults to USB streaming mode without needing command line flags.
- **Automated GitHub Actions CI/CD**: Includes `.github/workflows/client-linux-arm64-appimage.yml` to automatically build ARM64 AppImages on repository updates.

---

## 📌 Original Project Features & Limitations

### Features
- Cross platform, can stream to Windows, Mac, Linux (x86_64 / ARM64) and Android.
- Stream via USB or Wifi.
- **Video quality is fixed to 720p @ 30fps with h264 compression, this is a hardware limit**.
- Audio quality is fixed to 16bit PCM @ 48kHz stereo. Not compressed.
- Very low latency with an optimal setup, most games are playable!

### Limitations
- **Only works on games that have video recording enabled** (aka you can long-press the capture button to save a video)
   - [There is a workaround to support most games](https://github.com/exelix11/dvr-patches/), hosted on a different repo.
- Only captures game output. System UI, home menu and homebrews running as applet won't be captured.
- **USB streaming is not available when docked**.
- Requires at least Nintendo Switch firmware 6.0.0.

---

## 📖 Usage & Documentation
For full instructions and setup guides, visit the [SysDVR Wiki](https://github.com/exelix11/SysDVR/wiki).

## 📄 Credits
- **Original SysDVR Author**: [exelix11](https://github.com/exelix11/SysDVR)
- **ARM64 Port & Packaging**: [claudi0-udev](https://github.com/claudi0-udev/SysDVR)
- Libnx team, `mtp-server-nx`, `RTSPSharp`, and the Nintendo Switch homebrew community.

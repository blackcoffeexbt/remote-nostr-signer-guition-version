# Nostr Remote Signer Device

A hardware Nostr remote signer implementing NIP-46 (Nostr Connect) protocol for secure event signing

## Overview

This is a hardware-based Nostr remote signer built for the Guition JC3248W535 3.5" ESP32-S3 development board with touch screen display. The device implements the NIP-46 (Nostr Connect) protocol to provide secure remote signing services for Nostr events while keeping private keys isolated on the hardware device.

## Features

- **NIP-46 Remote Signer**: Secure implementation of Nostr Connect protocol
- **Touch Screen Interface**: 320x480 color touchscreen for user interaction
- **Hardware Key Storage**: Private keys stored securely on device
- **Client Authorization**: Manual approval system for connecting applications
- **Event Signing Confirmation**: User approval required for each signing request
- **WiFi Connectivity**: Connect to Nostr relays via WiFi
- **PIN Protection**: Secure access to settings and configuration
- **Power Management**: Light sleep after a short period of inactivity and deep sleep mode after a longer period of inactivity
- **Real-time Status**: Connection status and signing request notifications

## Hardware Requirements

- Guition JC3248W535 3.5" ESP32-S3 development board with touch screen display (320x480)
- Power supply (USB or battery)

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- Guition JC3248W535 3.5" touch screen display (320x480)

### Building and Flashing

```bash
# Clone the repository
git clone <repository-url>
cd <repository-directory>

# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

### Documentation

```bash
# Generate documentation
pio run -t docs
```

### Initial Setup

1. Power on the device
2. Use the touch screen interface to navigate to settings
3. Configure WiFi connection to connect to your network
4. Generate or import a Nostr private key (nsec format)
5. Configure relay connection settings
6. The device will display a bunker URL that applications can use to connect
7. When applications request to connect, approve them via the touch interface
8. Authorize signing requests as they come in from connected applications

### Usage

**Connecting Applications:**
1. Applications use the bunker URL to initiate NIP-46 connections
2. Device prompts for user authorization of new connections
3. Approved applications are stored in authorized clients list

**Signing Events:**
1. Connected applications send signing requests via NIP-46
2. Device displays event details and prompts for user confirmation
3. User approves or rejects each signing request individually
4. Private key never leaves the device

## Architecture

The application uses a modular architecture with the following components:

- **App**: Central coordinator managing all modules
- **Display**: LVGL-based UI with LovyanGFX driver
- **WiFi**: Network connectivity management
- **RemoteSigner**: NIP-46 protocol implementation and event signing
- **Settings**: Persistent configuration storage
- **UI**: User interface screens and interactions

## Development

Built with:
- PlatformIO Arduino framework
- LVGL graphics library
- LovyanGFX display driver
- Custom Nostr protocol implementation (NIP-01, NIP-44, NIP-46)
- WebSocket communication for relay connections
- Secp256k1 cryptography for Schnorr signatures

### Code Quality and Analysis

#### Finding Unused Functions

To analyze the codebase for unused functions, use cppcheck:

```bash
# Install cppcheck (macOS with Homebrew)
brew install cppcheck

# Run analysis to find unused functions
cppcheck --enable=unusedFunction src/

# For more detailed analysis with all checks
cppcheck --enable=all --inconclusive src/
```

This will identify functions that are defined but never called, helping keep the codebase clean and reducing binary size.

## License

MIT License
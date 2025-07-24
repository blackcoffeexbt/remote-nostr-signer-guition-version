# Remote Nostr Signer Conversion Plan

## Project Overview

Convert the existing NWC POS device codebase into a NIP-46 compliant remote Nostr signer using the same hardware platform (ESP32 with WT32-SC01 touch screen) and support infrastructure.

## Hardware Analysis

### Current Hardware (NWC POS)
- **Platform**: ESP32 with WT32-SC01 320x480 touch screen
- **Display**: ST7796 controller with LovyanGFX driver
- **GUI**: LVGL framework
- **Touch**: Capacitive touch interface
- **Power**: Continuous operation, no sleep modes
- **Connectivity**: WiFi with AP mode capability

### Reference Hardware (hardware-nostr-connect-device)
- **Platform**: ESP32-S3 with physical buttons
- **Display**: TFT with basic graphics
- **Input**: 4 physical buttons (PIN_BUTTON_1-4)
- **Power**: Battery voltage monitoring

## Architecture Comparison

### Current NWC POS Architecture
```
App (Central Coordinator)
├── Display (LVGL + LovyanGFX)
├── UI (Touch screens)
├── WiFi (AP + Station modes)
├── NWC (Nostr Wallet Connect)
├── Settings (Persistent storage)
└── Main (Entry point)
```

### Target Remote Signer Architecture
```
App (Central Coordinator)
├── Display (LVGL + LovyanGFX)
├── UI (Touch screens - simplified)
├── WiFi (AP + Station modes)
├── RemoteSigner (NIP-46 protocol)
├── Settings (Persistent storage)
└── Main (Entry point)
```

## Conversion Plan

### Phase 1: Core Infrastructure Setup
1. **Remove POS Components**
   - Delete NWC module (`nwc.cpp/h`)
   - Remove payment-related UI screens
   - Remove keypad functionality
   - Clean up POS-specific settings

2. **Create RemoteSigner Module**
   - Port core NIP-46 logic from hardware-nostr-connect-device
   - Implement WebSocket communication
   - Add NIP-44 encryption support
   - Handle signing requests (sign_event, get_public_key, etc.)

3. **Update Configuration System**
   - Replace POS settings with signer settings
   - Add fields for: relay URL, private key (nsec)
   - Maintain AP mode setup for initial configuration

### Phase 2: UI Adaptation
1. **Remove POS UI Elements**
   - Remove keypad screens
   - Remove payment amount entry
   - Remove transaction history
   - Remove currency selection

2. **Create Signer UI Screens**
   - Welcome/status screen with signer info
   - Real-time event signing display
   - Configuration screen (via AP portal)
   - Connection status indicators
   - Battery/system status
   - Menu option to display pairing QR code

3. **Touch Event Mapping**
   - Map touch areas to replace physical buttons
   - Implement touch gestures for navigation
   - Add confirmation dialogs for signing requests

### Phase 3: Protocol Implementation
1. **NIP-46 Protocol Handler**
   - Implement all required methods:
     - `connect` - Client connection handling
     - `sign_event` - Event signing with user confirmation
     - `get_public_key` - Return user's public key
     - `ping` - Keep-alive responses
     - `nip04_encrypt/decrypt` - Legacy encryption
     - `nip44_encrypt/decrypt` - Modern encryption

2. **Security Features**
   - Client authorization management
   - Signing request confirmation UI
   - Secret key generation and storage
   - Session management

3. **WebSocket Communication**
   - Relay connection management
   - Message fragmentation handling
   - Auto-reconnection logic
   - Error handling and recovery

### Phase 4: Display and UX
1. **Real-time Event Display**
   - Show incoming signing requests
   - Display event details (kind, content preview)
   - Confirmation prompts with touch buttons
   - Signing status feedback

2. **System Monitoring**
   - Connection status indicators
   - Battery level display
   - Memory usage monitoring
   - Error state notifications

3. **Configuration Interface**
   - AP mode web portal for setup
   - QR code display for bunker URL using existing LVGL QR implementation
   - Network settings management
   - Key import/generation options
   - Touch menu for accessing pairing QR code anytime

## File Changes Required

### Files to Delete
- `src/nwc.cpp` - NWC protocol implementation
- `src/nwc.h` - NWC header file
- POS-specific UI screens in `src/ui.cpp`

### Files to Modify
- `src/main.cpp` - Update initialization sequence
- `src/app.cpp/h` - Replace NWC with RemoteSigner coordination
- `src/ui.cpp/h` - Replace POS UI with signer UI
- `src/settings.cpp/h` - Update configuration schema
- `src/config.h` - Update system constants

### Files to Create
- `src/remote_signer.cpp/h` - NIP-46 protocol implementation
- `src/crypto.cpp/h` - Cryptographic functions (NIP-44)
- `src/bunker.cpp/h` - Bunker URL and connection management

## Configuration Schema

### Current POS Settings
```json
{
  "device_name": "POS Device",
  "currency": "USD",
  "nwc_url": "...",
  "relay_url": "...",
  "wifi_ssid": "...",
  "wifi_password": "..."
}
```

### Target Signer Settings
```json
{
  "device_name": "Remote Signer",
  "relay_url": "wss://relay.example.com",
  "private_key": "nsec1...",
  "public_key": "npub1...",
  "authorized_clients": ["client1_pubkey", "client2_pubkey"],
  "wifi_ssid": "...",
  "wifi_password": "..."
}
```

## UI Flow Design

### 1. Initial Setup (AP Mode)
- Device creates WiFi hotspot
- User connects and opens web portal
- Configure relay URL and import/generate keys
- Save configuration and restart

### 2. Normal Operation
- Display welcome screen with signer status
- Show connection status to relay
- Touch menu to access pairing QR code
- Wait for signing requests
- Display real-time signing events

### 3. Signing Flow
- Receive signing request via WebSocket
- Parse and validate request
- Display event details on screen
- Show touch confirmation buttons (Accept/Reject)
- Sign event if accepted and send response
- Display confirmation message

## Technical Considerations

### 1. Memory Management
- Port memory allocation strategy from reference implementation
- Use PSRAM for large JSON documents
- Implement proper cleanup for WebSocket messages

### 2. Cryptography
- Port NIP-44 encryption implementation
- Maintain compatibility with existing Nostr libraries
- Ensure proper key derivation and storage

### 3. QR Code Display
- Use existing LVGL QR code implementation (lv_qrcode_create/lv_qrcode_update)
- Reuse Display::displayQRCode() function for bunker URL display
- Maintain consistent QR code styling with current UI

### 4. Power Management
- Remove all sleep functionality (deep sleep, light sleep)
- Maintain continuous WebSocket connection monitoring
- Keep device always active for instant signing response
- Battery monitoring for status display only

### 5. Network Reliability
- Implement WebSocket reconnection logic
- Handle network interruptions gracefully
- Support multiple relay connections

## Testing Strategy

### 1. Unit Testing
- Test NIP-46 message parsing
- Verify cryptographic functions
- Validate configuration management

### 2. Integration Testing
- Test with real Nostr clients
- Verify signing request flow
- Test AP mode configuration

### 3. Hardware Testing
- Touch interface responsiveness
- Display rendering performance
- Power consumption validation
- Network stability testing

## Migration Timeline

### Week 1: Infrastructure
- Remove POS components
- Set up basic RemoteSigner module
- Port core signing logic

### Week 2: Protocol Implementation
- Implement NIP-46 handlers
- Add WebSocket communication
- Test basic signing functionality

### Week 3: UI Development
- Create signer UI screens
- Implement touch event handling
- Add configuration interface

### Week 4: Testing & Polish
- Comprehensive testing
- Performance optimization
- Documentation updates

## Success Criteria

1. **Functional Requirements**
   - ✅ NIP-46 protocol compliance
   - ✅ Touch-based user interface
   - ✅ AP mode configuration
   - ✅ Real-time event signing display
   - ✅ Client authorization management

2. **Performance Requirements**
   - ✅ Sub-second signing response time
   - ✅ Stable WebSocket connections
   - ✅ Efficient memory usage
   - ✅ Continuous WebSocket monitoring

3. **User Experience**
   - ✅ Intuitive touch interface
   - ✅ Clear signing confirmations
   - ✅ Reliable network setup
   - ✅ Status visibility

## Risk Mitigation

### Technical Risks
- **Memory limitations**: Use streaming JSON parsing and careful memory management
- **Network stability**: Implement robust reconnection and error handling
- **Cryptographic complexity**: Port proven implementations from reference code

### Hardware Risks
- **Touch responsiveness**: Implement proper debouncing and gesture recognition
- **Display performance**: Optimize LVGL rendering and reduce unnecessary updates
- **Power consumption**: Optimize for continuous operation without sleep modes

### Integration Risks
- **Client compatibility**: Test with multiple Nostr clients and implementations
- **Protocol compliance**: Validate against NIP-46 specification thoroughly
- **Security concerns**: Implement proper key storage and client authorization

This plan provides a comprehensive roadmap for converting the NWC POS device into a remote Nostr signer while leveraging the existing hardware capabilities and maintaining the modular architecture.
#pragma once

#include <Arduino.h>
#include <lvgl.h>

namespace UI {
    // Screen state enumeration
    typedef enum {
        SCREEN_SIGNER_STATUS,
        SCREEN_SETTINGS,
        SCREEN_WIFI,
        SCREEN_WIFI_PASSWORD,
        SCREEN_SETTINGS_SUB,
        SCREEN_INFO,
        SCREEN_UPDATE_CHECK,
        SCREEN_UPDATE_CONFIRM,
        SCREEN_UPDATE_PROGRESS
    } screen_state_t;

    // Initialization and cleanup
    void init();
    void cleanup();

    String getReadableEventKind(String eventKind);
    
    // Screen creation functions
    void createSignerStatusScreen();
    void createSettingsScreen();
    void createWiFiScreen();
    void createWiFiPasswordScreen(const char* ssid);
    void createSettingsSubScreen();
    void createInfoScreen();
    void createUpdateCheckScreen();
    void createUpdateConfirmScreen();
    void createUpdateProgressScreen();
    
    // Screen management
    void loadScreen(screen_state_t screen);
    void cleanupGlobalPointers();
   
    // Message display
    void showMessage(String title, String message);
    
    // UI element accessors for other modules
    lv_obj_t* getWiFiList();
    lv_obj_t* getInvoiceLabel();
    lv_obj_t* getInvoiceSpinner();
    
    // UI element setters
    void setQRCanvas(lv_obj_t* canvas);
    
    // Event handlers for external use
    void navigationEventHandler(lv_event_t* e);
    void settingsSaveEventHandler(lv_event_t* e);
    void settingsBackEventHandler(lv_event_t* e);
    void apPasswordKBEventHandler(lv_event_t* e);
    void rebootDeviceEventHandler(lv_event_t* e);
    void checkForUpdatesEventHandler(lv_event_t* e);
    void confirmUpdateEventHandler(lv_event_t* e);
    void cancelUpdateEventHandler(lv_event_t* e);
    
    // Signed events management
    struct SignedEvent {
        String eventKind;
        String content;
        String timestamp;
    };
    
    void addSignedEvent(const String& eventKind, const String& content, const String& timestamp);
    
    // Utility functions
    void showPairingQRCode();
    void showEventSignedNotification(const String& eventKind, const String& content);
    
    // Signing modal functions
    void showSigningModal();
    void updateSigningModalText(const String& text);
    void hideSigningModal();
    void hideSigningModalDelayed(uint32_t delayMs);
    
    // Settings integration
    void updateShopNameDisplay();
    void updateCurrencyDisplay();

    // Firmware update progress functions
    void updateFirmwareProgress(int progress, size_t current, size_t total);
    void updateFirmwareStatus(const String& status);
    
    // Screen state queries
    screen_state_t getCurrentScreen();
    
    // Constants for UI styling
    namespace Colors {
        const uint32_t PRIMARY = 0xdf2ccf;
        const uint32_t SUCCESS = 0x4CAF50;
        const uint32_t WARNING = 0xFF9800;
        const uint32_t ERROR = 0xF44336;
        const uint32_t INFO = 0x607D8B;
        const uint32_t BACKGROUND = 0x000000;
        const uint32_t TEXT = 0xFFFFFF;
    }
    
    namespace Fonts {
        extern const lv_font_t* FONT_DEFAULT;
        extern const lv_font_t* FONT_LARGE;
        extern const lv_font_t* FONT_XLARGE;
        extern const lv_font_t* FONT_SMALL;
    }
}
#pragma once

/**
 * App.h - Main Application Coordinator
 * 
 * This header provides a unified interface to all application modules
 * and coordinates initialization, cleanup, and inter-module communication
 */

#include <Arduino.h>
#include "driver/gpio.h"
#include "settings.h"
#include "display.h"
#include "wifi_manager.h"
#include "ui.h"
#include "remote_signer.h"
#include "firmware_update.h"

namespace App {
    /**
     * Application State Management
     */
    typedef enum {
        APP_STATE_INITIALIZING,
        APP_STATE_READY,
        APP_STATE_ERROR,
        APP_STATE_UPDATING
    } app_state_t;
    
    /**
     * Core Application Functions
     */
    void init();                    // Initialize all modules
    void cleanup();                 // Cleanup all modules
    void run();                     // Main application loop
    
    /**
     * State Management
     */
    app_state_t getState();
    void setState(app_state_t state);
    String getStateString();
    
    /**
     * Error Handling
     */
    void handleError(const String& module, const String& error);
    void clearError();
    String getLastError();
    
    /**
     * Inter-Module Communication
     */
    void notifyWiFiStatusChanged(bool connected);
    void notifySignerStatusChanged(bool connected);
    void notifySigningRequest(const String& eventKind, const String& content);
    void notifySigningCompleted(bool success);
    void notifyFirmwareUpdateStatusChanged(FirmwareUpdate::update_status_t status, FirmwareUpdate::update_error_t error);
    
    /**
     * Configuration Management
     */
    bool loadConfiguration();
    bool saveConfiguration();
    void resetToDefaults();
    
    /**
     * System Information
     */
    String getVersion();
    String getBuildInfo();
    void printSystemInfo();
    
    /**
     * Power Management (Sleep modes removed for continuous monitoring)
     */
    void resetActivityTimer();
    void handleTouchWake();
    
    /**
     * Module Health Monitoring
     */
    bool checkModuleHealth();
    void reportModuleStatus();
    
    /**
     * Update Management
     */
    void checkForUpdates();
    bool isUpdateAvailable();
    void performUpdate();
    
    /**
     * Diagnostic Functions
     */
    void runDiagnostics();
    void generateStatusReport();
    
    /**
     * Event System
     */
    typedef void (*app_event_callback_t)(const String& event, const String& data);
    void setEventCallback(app_event_callback_t callback);
    void fireEvent(const String& event, const String& data);
    
    /**
     * Constants
     */
    namespace Config {
        const String VERSION = "v1.0.0";
        const String BUILD_DATE = __DATE__ " " __TIME__;
        const unsigned long HEALTH_CHECK_INTERVAL = 30000; // 30 seconds
        const unsigned long STATUS_REPORT_INTERVAL = 300000; // 5 minutes
        
        // Touch handling configuration
        const unsigned long TOUCH_DEBOUNCE_TIME = 50; // 50ms debounce
    }
}
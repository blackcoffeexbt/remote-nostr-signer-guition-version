#pragma once

#include <Arduino.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "config.h"

namespace FirmwareUpdate {
    
    // Update status enumeration
    typedef enum {
        UPDATE_IDLE,
        UPDATE_CHECKING,
        UPDATE_AVAILABLE,
        UPDATE_NO_UPDATE,
        UPDATE_DOWNLOADING,
        UPDATE_FLASHING,
        UPDATE_SUCCESS,
        UPDATE_ERROR
    } update_status_t;
    
    // Error codes
    typedef enum {
        ERROR_NONE = 0,
        ERROR_NETWORK,
        ERROR_API_PARSE,
        ERROR_NO_RELEASE,
        ERROR_DOWNLOAD_FAILED,
        ERROR_FLASH_FAILED,
        ERROR_INVALID_VERSION,
        ERROR_INSUFFICIENT_SPACE
    } update_error_t;
    
    // Release information structure
    struct ReleaseInfo {
        String version;
        String downloadUrl;
        String changelog;
        size_t fileSize;
        bool isValid;
    };
    
    // Progress callback function type
    typedef void (*ProgressCallback)(int progress, size_t current, size_t total);
    typedef void (*StatusCallback)(update_status_t status, update_error_t error);
    
    // Initialization and cleanup
    void init();
    void cleanup();
    
    // Update checking and management
    bool checkForUpdates();
    ReleaseInfo getLatestRelease();
    bool isUpdateAvailable();
    bool startUpdate();
    void cancelUpdate();
    
    // Version comparison
    bool isNewerVersion(const String& remoteVersion, const String& currentVersion);
    
    // Status and progress
    update_status_t getStatus();
    update_error_t getLastError();
    int getProgress();
    size_t getCurrentSize();
    size_t getTotalSize();
    String getStatusMessage();
    
    // Callbacks
    void setProgressCallback(ProgressCallback callback);
    void setStatusCallback(StatusCallback callback);
    
    // Utility functions
    String getCurrentVersion();
    bool hasEnoughSpace(size_t requiredSize);
    void logError(const String& message);
    
    // Internal functions (exposed for testing)
    String fetchLatestReleaseJson();
    String attemptHttpsRequest();
    String attemptDirectHttpsRequest();
    ReleaseInfo parseReleaseInfo(const String& jsonResponse);
    bool downloadFirmware(const String& url, size_t expectedSize);
    bool flashFirmware();
    void freeMemoryForUpdate();
    
}
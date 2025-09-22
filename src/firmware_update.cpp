#include "firmware_update.h"
#include "wifi_manager.h"
#include "esp_heap_caps.h"

namespace FirmwareUpdate {
    
    // Static variables
    static update_status_t current_status = UPDATE_IDLE;
    static update_error_t last_error = ERROR_NONE;
    static ReleaseInfo latest_release;
    static ProgressCallback progress_callback = nullptr;
    static StatusCallback status_callback = nullptr;
    static int current_progress = 0;
    static size_t current_size = 0;
    static size_t total_size = 0;
    static HTTPClient http_client;
    
    void init() {
        current_status = UPDATE_IDLE;
        last_error = ERROR_NONE;
        latest_release = {};
        current_progress = 0;
        current_size = 0;
        total_size = 0;
        
        // WiFi client management handled by WiFiManager
    }
    
    void freeMemoryForUpdate() {
        // Free up as much memory as possible before SSL operations
        Serial.println("Freeing memory for update...");
        Serial.println("Free heap before cleanup: " + String(ESP.getFreeHeap()));
        Serial.println("Free PSRAM before cleanup: " + String(ESP.getFreePsram()));
        
        // Force garbage collection multiple times
        for (int i = 0; i < 3; i++) {
            String dummy;
            dummy.reserve(1000);
            for (int j = 0; j < 100; j++) {
                dummy += "x";
            }
            dummy = "";
            delay(10);
        }
        
        // Try to trigger heap compaction
        heap_caps_check_integrity_all(true);
        
        Serial.println("Free heap after cleanup: " + String(ESP.getFreeHeap()));
        Serial.println("Free PSRAM after cleanup: " + String(ESP.getFreePsram()));
    }
    
    void cleanup() {
        if (http_client.connected()) {
            http_client.end();
        }
        progress_callback = nullptr;
        status_callback = nullptr;
    }
    
    void setStatus(update_status_t status, update_error_t error = ERROR_NONE) {
        current_status = status;
        if (error != ERROR_NONE) {
            last_error = error;
        }
        
        if (status_callback) {
            status_callback(status, error);
        }
    }
    
    void updateProgress(int progress, size_t current, size_t total) {
        current_progress = progress;
        current_size = current;
        total_size = total;
        
        if (progress_callback) {
            progress_callback(progress, current, total);
        }
    }
    
    bool checkForUpdates() {
        if (!WiFiManager::isConnected()) {
            setStatus(UPDATE_ERROR, ERROR_NETWORK);
            return false;
        }
        
        setStatus(UPDATE_CHECKING);
        
        Serial.println("Free heap before update check: " + String(ESP.getFreeHeap()));
        
        String jsonResponse = fetchLatestReleaseJson();
        if (jsonResponse.isEmpty()) {
            logError("Failed to fetch release info");
            setStatus(UPDATE_ERROR, ERROR_API_PARSE);
            return false;
        }
        
        latest_release = parseReleaseInfo(jsonResponse);
        if (!latest_release.isValid) {
            setStatus(UPDATE_ERROR, ERROR_NO_RELEASE);
            return false;
        }
        
        bool hasUpdate = isNewerVersion(latest_release.version, getCurrentVersion());
        setStatus(hasUpdate ? UPDATE_AVAILABLE : UPDATE_NO_UPDATE);
        
        return hasUpdate;
    }
    
    String fetchLatestReleaseJson() {
        if (!WiFiManager::isConnected()) {
            logError("WiFi not connected");
            return "";
        }
        
        Serial.println("Attempting GitHub API request...");
        Serial.println("Free heap: " + String(ESP.getFreeHeap()));
        
        // Try direct HTTPS first with advanced PSRAM allocation
        String response = attemptDirectHttpsRequest();
        
        // If direct HTTPS fails, fallback to HTTP proxy
        if (response.isEmpty()) {
            Serial.println("Direct HTTPS failed, trying HTTP proxy...");
            response = attemptHttpsRequest(); // This now uses HTTP proxy
        }
        
        if (response.isEmpty()) {
            logError("Failed to fetch GitHub release data via both methods");
        }
        
        return response;
    }
    
    String attemptHttpsRequest() {
        Serial.println("Attempting to get GitHub release info...");
        Serial.println("Free heap before request: " + String(ESP.getFreeHeap()));
        Serial.println("Free PSRAM: " + String(ESP.getFreePsram()));
        
        // Free up as much memory as possible before making request
        freeMemoryForUpdate();
        
        // Try HTTP proxy approach to avoid SSL memory issues entirely
        HTTPClient http;
        WiFiClient client;
        
        // Use a CORS proxy service to access GitHub API via HTTP
        String proxyUrl = "http://api.allorigins.win/get?url=";
        String githubUrl = "https://api.github.com/repos/" + 
                          String(GITHUB_REPO_OWNER) + "/" + 
                          String(GITHUB_REPO_NAME) + "/releases/latest";
        String fullUrl = proxyUrl + githubUrl;
        
        Serial.println("Using proxy URL: " + fullUrl);
        
        if (!http.begin(client, fullUrl)) {
            Serial.println("Failed to begin HTTP connection");
            return "";
        }
        
        http.addHeader("User-Agent", "ESP32/1.0");
        http.setTimeout(15000);
        
        int httpCode = http.GET();
        Serial.println("HTTP response code: " + String(httpCode));
        
        String response = "";
        if (httpCode == HTTP_CODE_OK) {
            String proxyResponse = http.getString();
            Serial.println("Proxy response length: " + String(proxyResponse.length()));
            
            // Parse the proxy response to extract the GitHub API data
            DynamicJsonDocument proxyDoc(16384);
            DeserializationError error = deserializeJson(proxyDoc, proxyResponse);
            
            if (error) {
                Serial.println("Failed to parse proxy response: " + String(error.c_str()));
                response = proxyResponse; // Fallback to raw response
            } else {
                // Extract the contents field which contains the actual GitHub API response
                if (proxyDoc.containsKey("contents")) {
                    response = proxyDoc["contents"].as<String>();
                    Serial.println("Extracted GitHub API response length: " + String(response.length()));
                } else {
                    Serial.println("No 'contents' field in proxy response");
                    response = proxyResponse; // Fallback
                }
            }
            
            if (response.length() > 100) {
                Serial.println("Response preview: " + response.substring(0, 100) + "...");
            }
        } else {
            Serial.println("HTTP request failed with code: " + String(httpCode));
        }
        
        http.end();
        
        Serial.println("Free heap after request: " + String(ESP.getFreeHeap()));
        return response;
    }
    
    String attemptDirectHttpsRequest() {
        Serial.println("Attempting direct HTTPS to GitHub API...");
        Serial.println("Free heap before request: " + String(ESP.getFreeHeap()));
        Serial.println("Free PSRAM: " + String(ESP.getFreePsram()));
        
        // Aggressive memory cleanup
        freeMemoryForUpdate();
        
        // Force SSL library to use PSRAM for allocations
        heap_caps_malloc_extmem_enable(4096); // Use external memory for allocations > 4KB
        
        WiFiClientSecure* client = nullptr;
        
        // Try to allocate everything in PSRAM
        client = (WiFiClientSecure*)heap_caps_malloc(sizeof(WiFiClientSecure), MALLOC_CAP_SPIRAM);
        if (!client) {
            Serial.println("Failed to allocate WiFiClientSecure in PSRAM");
            client = (WiFiClientSecure*)heap_caps_malloc(sizeof(WiFiClientSecure), MALLOC_CAP_8BIT);
            if (!client) {
                Serial.println("Failed to allocate WiFiClientSecure at all");
                return "";
            }
            Serial.println("Using regular heap for WiFiClientSecure");
        } else {
            Serial.println("Successfully allocated WiFiClientSecure in PSRAM");
        }
        
        // Use placement new to construct the object
        new(client) WiFiClientSecure();
        
        // Configure for minimal memory usage
        client->setInsecure(); // Skip certificate validation
        client->setTimeout(20000); // Shorter timeout
        
        HTTPClient http;
        String releaseUrl = "https://api.github.com/repos/" + 
                          String(GITHUB_REPO_OWNER) + "/" + 
                          String(GITHUB_REPO_NAME) + "/releases/latest";
        
        Serial.println("Using direct API URL: " + releaseUrl);
        
        if (!http.begin(*client, releaseUrl)) {
            Serial.println("Failed to begin HTTPS connection");
            client->~WiFiClientSecure();
            heap_caps_free(client);
            return "";
        }
        
        http.addHeader("User-Agent", "ESP32-Signer/1.0");
        http.setTimeout(20000);
        
        int httpCode = http.GET();
        Serial.println("HTTPS response code: " + String(httpCode));
        
        String response = "";
        if (httpCode == HTTP_CODE_OK) {
            response = http.getString();
            Serial.println("HTTPS response length: " + String(response.length()));
            if (response.length() > 100) {
                Serial.println("Response preview: " + response.substring(0, 100) + "...");
            }
        } else {
            Serial.println("HTTPS request failed with code: " + String(httpCode));
        }
        
        http.end();
        
        // Cleanup
        if (client) {
            client->~WiFiClientSecure();
            heap_caps_free(client);
        }
        
        Serial.println("Free heap after HTTPS request: " + String(ESP.getFreeHeap()));
        return response;
    }
    
    ReleaseInfo parseReleaseInfo(const String& jsonResponse) {
        ReleaseInfo release = {};
        
        if (jsonResponse.isEmpty()) {
            logError("Empty JSON response");
            return release;
        }
        
        Serial.println("Parsing JSON response...");
        Serial.println("Free heap before JSON parse: " + String(ESP.getFreeHeap()));
        
        // Use smaller buffer but should be sufficient for GitHub API response
        DynamicJsonDocument doc(12288); // 12KB should be enough
        DeserializationError error = deserializeJson(doc, jsonResponse);
        
        if (error) {
            logError("JSON parse error: " + String(error.c_str()));
            Serial.println("Failed JSON preview: " + jsonResponse.substring(0, 200) + "...");
            
            // Try with even smaller buffer for basic info
            DynamicJsonDocument smallDoc(4096);
            DeserializationError smallError = deserializeJson(smallDoc, jsonResponse);
            if (!smallError) {
                Serial.println("Trying with smaller buffer...");
                doc = smallDoc;
            } else {
                return release;
            }
        }
        
        Serial.println("JSON parsed successfully");
        Serial.println("Free heap after JSON parse: " + String(ESP.getFreeHeap()));
        
        // Extract version from tag_name (remove 'v' prefix if present)
        if (doc.containsKey("tag_name") && !doc["tag_name"].isNull()) {
            String tagName = doc["tag_name"].as<String>();
            release.version = tagName.startsWith("v") ? tagName.substring(1) : tagName;
            Serial.println("Found version: " + release.version);
        } else {
            logError("No tag_name found in release");
            return release;
        }
        
        // Extract changelog from body (optional)
        if (doc.containsKey("body") && !doc["body"].isNull()) {
            release.changelog = doc["body"].as<String>();
            // Limit changelog size to save memory
            if (release.changelog.length() > 500) {
                release.changelog = release.changelog.substring(0, 497) + "...";
            }
            Serial.println("Found changelog (length: " + String(release.changelog.length()) + ")");
        }
        
        // Find firmware binary in assets
        if (doc.containsKey("assets") && !doc["assets"].isNull()) {
            JsonArray assets = doc["assets"];
            Serial.println("Found " + String(assets.size()) + " assets");
            
            for (JsonObject asset : assets) {
                if (asset.containsKey("name") && asset.containsKey("browser_download_url")) {
                    String name = asset["name"].as<String>();
                    Serial.println("Asset: " + name);
                    
                    // Look for .bin files or files containing "firmware" or "esp32"
                    if (name.endsWith(".bin") || name.endsWith(".firmware") || 
                        name.indexOf("firmware") >= 0 || name.indexOf("esp32") >= 0 ||
                        name.indexOf("nostr") >= 0 || name.indexOf("signer") >= 0) {
                        release.downloadUrl = asset["browser_download_url"].as<String>();
                        if (asset.containsKey("size")) {
                            release.fileSize = asset["size"].as<size_t>();
                        }
                        Serial.println("Selected firmware: " + name + " (" + String(release.fileSize) + " bytes)");
                        Serial.println("Download URL: " + release.downloadUrl);
                        break;
                    }
                }
            }
            
            if (release.downloadUrl.isEmpty()) {
                logError("No firmware binary found in assets");
                Serial.println("Available assets:");
                for (JsonObject asset : assets) {
                    if (asset.containsKey("name")) {
                        Serial.println("  - " + asset["name"].as<String>());
                    }
                }
            }
        } else {
            logError("No assets found in release");
        }
        
        release.isValid = !release.version.isEmpty() && !release.downloadUrl.isEmpty();
        
        Serial.println("Release info parsed - Valid: " + String(release.isValid));
        Serial.println("Free heap after parse complete: " + String(ESP.getFreeHeap()));
        
        return release;
    }
    
    bool isNewerVersion(const String& remoteVersion, const String& currentVersion) {
        // Simple semantic version comparison (major.minor.patch)
        // For more complex versioning, consider using a proper semver library
        
        int remoteMajor, remoteMinor, remotePatch;
        int currentMajor, currentMinor, currentPatch;
        
        // Parse remote version
        sscanf(remoteVersion.c_str(), "%d.%d.%d", &remoteMajor, &remoteMinor, &remotePatch);
        
        // Parse current version
        sscanf(currentVersion.c_str(), "%d.%d.%d", &currentMajor, &currentMinor, &currentPatch);
        
        // Compare versions
        if (remoteMajor > currentMajor) return true;
        if (remoteMajor < currentMajor) return false;
        
        if (remoteMinor > currentMinor) return true;
        if (remoteMinor < currentMinor) return false;
        
        return remotePatch > currentPatch;
    }
    
    bool startUpdate() {
        if (current_status != UPDATE_AVAILABLE) {
            return false;
        }
        
        if (!hasEnoughSpace(latest_release.fileSize)) {
            setStatus(UPDATE_ERROR, ERROR_INSUFFICIENT_SPACE);
            return false;
        }
        
        bool downloadSuccess = downloadFirmware(latest_release.downloadUrl, latest_release.fileSize);
        if (!downloadSuccess) {
            setStatus(UPDATE_ERROR, ERROR_DOWNLOAD_FAILED);
            return false;
        }
        
        bool flashSuccess = flashFirmware();
        if (!flashSuccess) {
            setStatus(UPDATE_ERROR, ERROR_FLASH_FAILED);
            return false;
        }
        
        setStatus(UPDATE_SUCCESS);
        return true;
    }
    
    bool downloadFirmware(const String& url, size_t expectedSize) {
        setStatus(UPDATE_DOWNLOADING);
        updateProgress(0, 0, expectedSize);
        
        Serial.println("Free heap before download: " + String(ESP.getFreeHeap()));
        Serial.println("Free PSRAM before download: " + String(ESP.getFreePsram()));
        
        // Allocate WiFiClientSecure in PSRAM to avoid SSL memory issues
        WiFiClientSecure* client = (WiFiClientSecure*)ps_malloc(sizeof(WiFiClientSecure));
        if (!client) {
            Serial.println("Failed to allocate WiFiClientSecure in PSRAM for download");
            // Fallback to regular malloc
            client = new WiFiClientSecure();
            if (!client) {
                logError("Failed to allocate secure client for download");
                return false;
            }
            Serial.println("Using regular heap for WiFiClientSecure download");
        } else {
            // Use placement new to construct the object in PSRAM
            new(client) WiFiClientSecure();
            Serial.println("Successfully allocated WiFiClientSecure in PSRAM for download");
        }
        
        client->setTimeout(30000); // 30 seconds for firmware download
        client->setInsecure(); // Skip certificate validation for GitHub downloads
        
        if (!http_client.begin(*client, url)) {
            logError("Failed to begin download connection");
            // Properly cleanup PSRAM-allocated client
            client->~WiFiClientSecure();
            free(client);
            return false;
        }
        
        int httpCode = http_client.GET();
        
        if (httpCode != HTTP_CODE_OK) {
            logError("Download failed: HTTP " + String(httpCode));
            http_client.end();
            // Properly cleanup PSRAM-allocated client
            client->~WiFiClientSecure();
            free(client);
            return false;
        }
        
        int contentLength = http_client.getSize();
        if (contentLength <= 0) {
            logError("Invalid content length: " + String(contentLength));
            http_client.end();
            // Properly cleanup PSRAM-allocated client
            client->~WiFiClientSecure();
            free(client);
            return false;
        }
        
        if (contentLength != expectedSize) {
            logError("Size mismatch: expected " + String(expectedSize) + ", got " + String(contentLength));
        }
        
        if (!Update.begin(contentLength)) {
            logError("Update.begin failed: " + String(Update.errorString()));
            http_client.end();
            // Properly cleanup PSRAM-allocated client
            client->~WiFiClientSecure();
            free(client);
            return false;
        }
        
        WiFiClient* stream = http_client.getStreamPtr();
        size_t written = 0;
        
        while (http_client.connected() && written < contentLength) {
            size_t available = stream->available();
            if (available) {
                uint8_t buffer[1024];
                size_t readBytes = stream->readBytes(buffer, min(available, sizeof(buffer)));
                
                size_t writtenBytes = Update.write(buffer, readBytes);
                if (writtenBytes != readBytes) {
                    logError("Write error: " + String(Update.errorString()));
                    http_client.end();
                    // Properly cleanup PSRAM-allocated client
                    client->~WiFiClientSecure();
                    free(client);
                    return false;
                }
                
                written += writtenBytes;
                int progress = (written * 100) / contentLength;
                updateProgress(progress, written, contentLength);
            }
            delay(1);
        }
        
        http_client.end();
        
        // Properly cleanup PSRAM-allocated client
        if (client) {
            client->~WiFiClientSecure();
            free(client);
        }
        
        if (written != contentLength) {
            logError("Download incomplete: " + String(written) + "/" + String(contentLength));
            return false;
        }
        
        Serial.println("Free heap after download: " + String(ESP.getFreeHeap()));
        return true;
    }
    
    bool flashFirmware() {
        setStatus(UPDATE_FLASHING);
        updateProgress(0, 0, 100);
        
        if (!Update.end(true)) {
            logError("Flash failed: " + String(Update.errorString()));
            return false;
        }
        
        updateProgress(100, 100, 100);
        return true;
    }
    
    // Getter functions
    ReleaseInfo getLatestRelease() { return latest_release; }
    bool isUpdateAvailable() { return current_status == UPDATE_AVAILABLE; }
    update_status_t getStatus() { return current_status; }
    update_error_t getLastError() { return last_error; }
    int getProgress() { return current_progress; }
    size_t getCurrentSize() { return current_size; }
    size_t getTotalSize() { return total_size; }
    String getCurrentVersion() { return String(FIRMWARE_VERSION); }
    
    String getStatusMessage() {
        switch (current_status) {
            case UPDATE_IDLE: return "Ready";
            case UPDATE_CHECKING: return "Checking for updates...";
            case UPDATE_AVAILABLE: return "Update available: " + latest_release.version;
            case UPDATE_NO_UPDATE: return "No updates available";
            case UPDATE_DOWNLOADING: return "Downloading firmware...";
            case UPDATE_FLASHING: return "Installing update...";
            case UPDATE_SUCCESS: return "Update successful! Restart required.";
            case UPDATE_ERROR:
                switch (last_error) {
                    case ERROR_NETWORK: return "Network error";
                    case ERROR_API_PARSE: return "Failed to parse release info";
                    case ERROR_NO_RELEASE: return "No firmware found in release";
                    case ERROR_DOWNLOAD_FAILED: return "Download failed";
                    case ERROR_FLASH_FAILED: return "Installation failed";
                    case ERROR_INVALID_VERSION: return "Invalid version format";
                    case ERROR_INSUFFICIENT_SPACE: return "Insufficient storage space";
                    default: return "Unknown error";
                }
            default: return "Unknown status";
        }
    }
    
    bool hasEnoughSpace(size_t requiredSize) {
        size_t freeSpace = ESP.getFreeSketchSpace();
        return freeSpace >= requiredSize;
    }
    
    void setProgressCallback(ProgressCallback callback) {
        progress_callback = callback;
    }
    
    void setStatusCallback(StatusCallback callback) {
        status_callback = callback;
    }
    
    void cancelUpdate() {
        if (current_status == UPDATE_DOWNLOADING || current_status == UPDATE_CHECKING) {
            http_client.end();
            setStatus(UPDATE_IDLE);
        }
    }
    
    void logError(const String& message) {
        Serial.println("FirmwareUpdate Error: " + message);
    }
    
}
/**
 * @file main.cpp
 * @brief ESP32-S3 AI-Based Safety Detection and Image Capture System
 *
 * This system uses SSCMA AI for real-time object detection to identify
 * safety compliance (PPE detection). It captures and stores images on
 * LittleFS with FIFO management, provides a web interface for viewing
 * captures, and controls LED indicators and hooter based on detection state.
 *
 * Detection States:
 *   - State 0 (BLUE):  No scene / AI timeout
 *   - State 1 (RED):   Risk detected (illegal objects)
 *   - State 2 (GREEN): Safe scene (legal objects only)
 */

// =============================================================================
// INCLUDES
// =============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Ticker.h>
#include <LittleFS.h>

#include <Seeed_Arduino_SSCMA.h>
#include "mbedtls/base64.h"
#include "time.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// =============================================================================
// USER CONFIGURATION
// =============================================================================

// Wi-Fi Credentials
static const char* WIFI_SSID     = "Demo";
static const char* WIFI_PASSWORD = "TTPL@dmin1208";

// Hardware Connection Mode
#define SSCMA_CONNECT_TO_XIAO_S3    1

// Timing Configuration
#define HOOTER_ON_PERIOD_SEC        60      // Duration hooter stays ON (seconds)
#define NO_SCENE_HOLD_TIME_MS       6000    // Debounce hold time for state transitions (ms)
#define DETECTION_INTERVAL_MS       500     // AI detection polling interval (ms)

// Storage Configuration
#define DECODED_IMAGE_MAX_SIZE      16384   // Maximum decoded JPEG size (bytes)
#define MAX_SAVED_IMAGES            80      // Maximum images allowed in LittleFS

// System Maintenance
#define MAX_CYCLES_BEFORE_REBOOT    100     // Reboot threshold to clear heap fragmentation

// NTP Configuration
static const char* NTP_SERVER           = "pool.ntp.org";
static const long  GMT_OFFSET_SEC       = 19800;    // IST: UTC+5:30
static const int   DAYLIGHT_OFFSET_SEC  = 0;

// =============================================================================
// DETECTION LABEL DEFINITIONS
// =============================================================================

/**
 * Label categories for AI detection
 * - Illegal: FACE, FAKE_TURBAN, HALF (triggers risk state)
 * - Legal:   FULL, TURBAN (safe state)
 */
#define LABEL_FACE          0   // Illegal
#define LABEL_FAKE_TURBAN   1   // Illegal
#define LABEL_FULL          2   // Legal
#define LABEL_HALF          3   // Illegal
#define LABEL_TURBAN        4   // Legal

static const char* LABEL_NAMES[] = {
    "FACE",
    "FAKE_TURBAN",
    "FULL",
    "HALF",
    "TURBAN"
};

static const size_t LABEL_COUNT = sizeof(LABEL_NAMES) / sizeof(LABEL_NAMES[0]);

// =============================================================================
// HARDWARE PIN DEFINITIONS
// =============================================================================

#define PIN_LED_ORANGE  D2      // Orange LED (unused in current logic)
#define PIN_LED_GREEN   D1      // Green LED  - State 2: Safe
#define PIN_LED_RED     D0      // Red LED    - State 1: Risk
#define PIN_LED_BLUE    D7      // Blue LED   - State 0: No Scene / AI Timeout
#define PIN_HOOTER      D6      // Buzzer / Hooter output

// =============================================================================
// GLOBAL OBJECTS AND VARIABLES
// =============================================================================

// Core Objects
static SSCMA           AI;
static Ticker          hooterOffTimer;
static WebServer       server(80);
static SemaphoreHandle_t littleFSMutex;

// State Variables
static volatile uint16_t imageCount            = 0;
static volatile int      uploadCycleCounter    = 0;
static bool              webServerInitialized  = false;
static int               lastDetectionState    = -1;
static unsigned long     lastPositiveDetectionTime = 0;

// Static buffer for JPEG decoding (avoids heap fragmentation)
static uint8_t g_jpegDecodeBuffer[DECODED_IMAGE_MAX_SIZE] __attribute__((aligned(32)));

// =============================================================================
// DATA STRUCTURES
// =============================================================================

/**
 * @brief Structure to pass capture data to the async saving task
 */
typedef struct {
    String base64Image;
    String labelsString;
    int    detectionState;
} CaptureData_t;

// =============================================================================
// FUNCTION PROTOTYPES
// =============================================================================

// File System Functions
static String getOldestFileName(void);
static void   manageFifoStorage(void);
static size_t decodeBase64Image(const char* pData, uint8_t* decodedStr, size_t maxOut);
static bool   writeImageToLittleFS(const uint8_t* data, size_t len, String* pathOut);
static void   writeLabelFileToLittleFS(const String& jpgPath, const String& labelsString);

// Hardware Control Functions
static void disableHooter(void);
static void enableHooter(void);
static void updateLEDs(int state);

// AI Detection Functions
static int  checkScene(void);
static void saveImageTask(void* pvParameters);

// Web Server Handlers
static bool handleFileRead(String path);
static void handleRoot(void);
static void handleNotFound(void);

// =============================================================================
// FILE SYSTEM FUNCTIONS
// =============================================================================

/**
 * @brief Scans LittleFS to find the oldest file and updates imageCount
 * @return Full path of the oldest JPEG file (guaranteed to start with '/')
 */
static String getOldestFileName(void) {
    String   oldestFilename = "";
    time_t   oldestTime     = 0xFFFFFFFF;
    uint16_t count          = 0;

    File root = LittleFS.open("/", "r");
    if (root) {
        File file = root.openNextFile();
        while (file) {
            String filename = file.name();

            if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) {
                count++;
                time_t writeTime = file.getLastWrite();
                if (writeTime < oldestTime) {
                    oldestTime     = writeTime;
                    oldestFilename = filename;
                }
            }
            file.close();
            file = root.openNextFile();
        }
        root.close();
    }

    // Ensure path starts with '/'
    if (!oldestFilename.isEmpty() && oldestFilename[0] != '/') {
        oldestFilename = "/" + oldestFilename;
    }

    imageCount = count;
    return oldestFilename;
}

/**
 * @brief Implements FIFO storage policy - deletes oldest file when limit reached
 */
static void manageFifoStorage(void) {
    if (imageCount < MAX_SAVED_IMAGES) {
        return;
    }

    String oldestPath = getOldestFileName();

    if (!oldestPath.isEmpty()) {
        Serial.printf("FIFO: Attempting to delete oldest file: %s (Current count: %d)\n",
                      oldestPath.c_str(), imageCount);

        // Generate associated text file path
        String oldestTxtPath = oldestPath;
        oldestTxtPath.replace(".jpg", ".txt");
        oldestTxtPath.replace(".jpeg", ".txt");

        // Delete associated TXT log file
        if (LittleFS.exists(oldestTxtPath)) {
            if (LittleFS.remove(oldestTxtPath)) {
                Serial.printf("FIFO: Deleted associated log file: %s\n", oldestTxtPath.c_str());
            } else {
                Serial.printf("FIFO: FAILED to delete log file: %s\n", oldestTxtPath.c_str());
            }
        }

        // Delete JPG image file
        if (LittleFS.exists(oldestPath)) {
            if (LittleFS.remove(oldestPath)) {
                Serial.printf("FIFO: Deleted oldest image file: %s\n", oldestPath.c_str());
                imageCount--;
                Serial.printf("FIFO: Image count reduced to %d. Ready for save.\n", imageCount);
            } else {
                Serial.printf("FIFO: CRITICAL FAILURE: Failed to delete %s\n", oldestPath.c_str());
            }
        }
    } else {
        Serial.println("FIFO ERROR: Could not find a file to delete despite count at limit.");
    }
}

/**
 * @brief Decodes Base64 encoded image data
 * @param pData      Input Base64 string
 * @param decodedStr Output buffer for decoded data
 * @param maxOut     Maximum output buffer size
 * @return Number of decoded bytes, or 0 on failure
 */
static size_t decodeBase64Image(const char* pData, uint8_t* decodedStr, size_t maxOut) {
    if (!pData || !decodedStr) {
        return 0;
    }

    size_t strLen    = strlen(pData);
    size_t outputLen = 0;

    // First call to determine required output size
    int ret = mbedtls_base64_decode(NULL, 0, &outputLen,
                                    (const unsigned char*)pData, strLen);
    if (ret != 0 && ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        return 0;
    }

    if (outputLen == 0 || outputLen > maxOut) {
        return 0;
    }

    // Actual decode
    ret = mbedtls_base64_decode(decodedStr, maxOut, &outputLen,
                                (const unsigned char*)pData, strLen);
    return (ret == 0) ? outputLen : 0;
}

/**
 * @brief Writes decoded JPEG image data to LittleFS with chunked I/O
 * @param data    Pointer to JPEG data
 * @param len     Length of JPEG data
 * @param pathOut Output parameter for saved file path
 * @return true on success, false on failure
 */
static bool writeImageToLittleFS(const uint8_t* data, size_t len, String* pathOut) {
    manageFifoStorage();

    if (imageCount >= MAX_SAVED_IMAGES) {
        Serial.printf("WARNING: Image limit (%d) still reached after FIFO. Skipping save.\n",
                      MAX_SAVED_IMAGES);
        return false;
    }

    // Generate filename with timestamp
    char      pathChar[35];
    struct tm timeinfo;
    bool      timeSynced = getLocalTime(&timeinfo);
    uint16_t  ms         = millis() % 1000;

    if (timeSynced) {
        snprintf(pathChar, sizeof(pathChar), "/iimg_%02d:%02d:%02d_%03dms.jpg",
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, ms);
    } else {
        snprintf(pathChar, sizeof(pathChar), "/iimg_%d_%03dms.jpg", imageCount, ms);
    }

    // Open file for writing
    File file = LittleFS.open(pathChar, FILE_WRITE);
    if (!file) {
        Serial.printf("ERROR: Failed to open image file %s for writing.\n", pathChar);
        return false;
    }

    // Write data in chunks to prevent watchdog timeout
    const size_t CHUNK_SIZE    = 512;
    size_t       bytesWritten  = 0;

    while (bytesWritten < len) {
        size_t writeSize = min(CHUNK_SIZE, len - bytesWritten);

        if (file.write(data + bytesWritten, writeSize) != writeSize) {
            Serial.printf("ERROR: Failed to write all data to file %s.\n", pathChar);
            file.close();
            return false;
        }
        bytesWritten += writeSize;
        vTaskDelay(1);  // Yield CPU during long I/O
    }

    file.close();
    imageCount++;
    *pathOut = String(pathChar);

    Serial.printf("SUCCESS: Image saved to %s (Count: %d)\n", pathChar, imageCount);
    return true;
}

/**
 * @brief Writes detection label log file associated with an image
 * @param jpgPath      Path to the associated JPEG file
 * @param labelsString String containing detected labels
 */
static void writeLabelFileToLittleFS(const String& jpgPath, const String& labelsString) {
    if (jpgPath.isEmpty()) {
        return;
    }

    // Generate text file path from JPEG path
    String txtPath = jpgPath;
    txtPath.replace(".jpg", ".txt");
    txtPath.replace(".jpeg", ".txt");

    File file = LittleFS.open(txtPath, FILE_WRITE);
    if (!file) {
        Serial.printf("WARNING: Failed to open label file %s for writing.\n", txtPath.c_str());
        return;
    }

    file.println("--- AI Detection Log ---");
    file.println("File Reference: " + jpgPath.substring(1));
    file.println("Objects Detected:");
    file.println(labelsString);

    vTaskDelay(1);  // Yield CPU
    file.close();

    Serial.printf("SUCCESS: Label log saved to %s\n", txtPath.c_str());
}

// =============================================================================
// HARDWARE CONTROL FUNCTIONS
// =============================================================================

/**
 * @brief Callback to disable the hooter (called by Ticker)
 */
static void disableHooter(void) {
    digitalWrite(PIN_HOOTER, LOW);
    hooterOffTimer.detach();
}

/**
 * @brief Enables the hooter with automatic timeout
 */
static void enableHooter(void) {
    if (digitalRead(PIN_HOOTER) == LOW) {
        digitalWrite(PIN_HOOTER, HIGH);
        hooterOffTimer.once(HOOTER_ON_PERIOD_SEC, disableHooter);
    }
}

/**
 * @brief Updates LED indicators based on detection state
 * @param state Detection state (0=Blue, 1=Red, 2=Green)
 */
static void updateLEDs(int state) {
    // Turn off all state LEDs first
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);

    // Activate appropriate LED for current state
    switch (state) {
        case 0:
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
        case 1:
            digitalWrite(PIN_LED_RED, HIGH);
            break;
        case 2:
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
        default:
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
    }
}

// =============================================================================
// AI DETECTION FUNCTIONS
// =============================================================================

/**
 * @brief Invokes AI inference with retry logic
 * @return CMD_OK on success, error code on failure
 */
static int checkScene(void) {
    int retval = -1;
    int retry  = 2;

    do {
        retval = AI.invoke();
        if (retval != CMD_OK) {
            delay(10);
        }
    } while (retval != CMD_OK && --retry > 0);

    return retval;
}

/**
 * @brief FreeRTOS task for asynchronous image saving (runs on Core 1)
 * @param pvParameters Pointer to CaptureData_t structure
 */
static void saveImageTask(void* pvParameters) {
    Serial.println("[TASK] Core 1: Image Save Task started.");

    CaptureData_t* data = (CaptureData_t*)pvParameters;
    size_t         jpegLen   = 0;
    String         imagePath = "";

    // Decode Base64 image to static buffer
    jpegLen = decodeBase64Image(data->base64Image.c_str(),
                                g_jpegDecodeBuffer,
                                DECODED_IMAGE_MAX_SIZE);

    if (jpegLen > 0) {
        // Acquire mutex for file system access
        if (xSemaphoreTake(littleFSMutex, portMAX_DELAY) == pdTRUE) {
            bool written = writeImageToLittleFS(g_jpegDecodeBuffer, jpegLen, &imagePath);

            if (written) {
                // Generate log content based on detection state
                String logContent;
                if (data->detectionState == 1) {
                    logContent = "RISK DETECTED: " + data->labelsString;
                } else if (data->detectionState == 2) {
                    logContent = "SAFE SCENE CAPTURED: " + data->labelsString;
                } else {
                    logContent = "STATE CHANGE TO NO SCENE: Previous was " +
                                 (data->labelsString.isEmpty() ? "TIMEOUT" : data->labelsString);
                }

                writeLabelFileToLittleFS(imagePath, logContent);

                // Check reboot threshold for heap fragmentation management
                uploadCycleCounter++;
                if (uploadCycleCounter >= MAX_CYCLES_BEFORE_REBOOT) {
                    Serial.println("[SYSTEM] --- REBOOTING to clear heap fragmentation ---");
                    vTaskDelay(pdMS_TO_TICKS(500));
                    ESP.restart();
                }
            }

            xSemaphoreGive(littleFSMutex);
        } else {
            Serial.println("[TASK] ERROR: Failed to acquire LittleFS mutex.");
        }
    } else {
        Serial.println("[TASK] WARNING: Base64 decode failed. Skipping save.");
    }

    // Clean up and self-terminate
    delete data;
    Serial.println("[TASK] Core 1: Image Save Task finished.");
    vTaskDelete(NULL);
}

// =============================================================================
// WEB SERVER HANDLERS
// =============================================================================

/**
 * @brief Handles file read requests with mutex protection
 * @param path Requested file path
 * @return true if file was served, false if not found
 */
static bool handleFileRead(String path) {
    if (path.endsWith("/")) {
        path += "index.html";
    }

    if (LittleFS.exists(path)) {
        // Try to acquire mutex without blocking
        if (xSemaphoreTake(littleFSMutex, 0) == pdTRUE) {
            File file = LittleFS.open(path, "r");

            // Determine content type
            String contentType = "text/plain";
            if (path.endsWith(".html")) {
                contentType = "text/html";
            } else if (path.endsWith(".css")) {
                contentType = "text/css";
            } else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) {
                contentType = "image/jpeg";
            } else if (path.endsWith(".txt")) {
                contentType = "text/plain";
            }

            server.streamFile(file, contentType);
            file.close();
            xSemaphoreGive(littleFSMutex);
            return true;
        } else {
            server.send(503, "text/plain", "File System Busy. Try again in a moment.");
            return true;
        }
    }
    return false;
}

/**
 * @brief Handles root page request - displays file listing
 */
static void handleRoot(void) {
    if (xSemaphoreTake(littleFSMutex, 0) != pdTRUE) {
        server.send(503, "text/plain", "File System Busy. Try again in a moment.");
        return;
    }

    getOldestFileName();  // Update image count

    // Build HTML response
    String html = "<html><head><title>ESP32-S3 AI Image System</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;background:#f0f0f0;padding:20px;}";
    html += "a{text-decoration:none;display:block;padding:8px;margin:4px 0;";
    html += "background:#fff;border-radius:4px;border:1px solid #ccc;";
    html += "white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}";
    html += "</style>";
    html += "</head><body>";
    html += "<h2>LittleFS Capture Files:</h2>";
    html += "<p>Total space: " + String(LittleFS.totalBytes() / 1024) + " KB</p>";
    html += "<p>Currently Saved Images: " + String(imageCount) + " / " +
            String(MAX_SAVED_IMAGES) + "</p>";
    html += "<p>Click JPG for image, click TXT for label log.</p><hr>";

    File root      = LittleFS.open("/", "r");
    int  fileCount = 0;

    if (root) {
        File file = root.openNextFile();
        while (file) {
            String filename = file.name();

            if (filename.endsWith(".jpg") || filename.endsWith(".jpeg") ||
                filename.endsWith(".txt")) {

                String displayInfo  = "";
                int    firstColon   = filename.indexOf(':');
                int    underscore   = filename.indexOf('_', firstColon);

                // Parse NTP-synced filename format
                if (firstColon > 0 && underscore > 0) {
                    String hmsStr  = filename.substring(5, underscore);
                    int    msIndex = filename.indexOf('m', underscore);
                    String msStr   = filename.substring(underscore + 1, msIndex);

                    if (hmsStr.length() == 8) {
                        displayInfo = "NTP Time: **" + hmsStr + "." + msStr + "**";
                    }
                }
                // Parse offline filename format
                else if (filename.indexOf("/iimg_") == 0 && filename.lastIndexOf('_') > 0) {
                    int firstUnderscore = filename.indexOf('_');
                    int lastUnderscore  = filename.lastIndexOf('_');
                    int msIndex         = filename.indexOf('m', lastUnderscore);

                    if (firstUnderscore > 0 && lastUnderscore > firstUnderscore &&
                        msIndex > lastUnderscore) {
                        String indexStr = filename.substring(firstUnderscore + 1, lastUnderscore);
                        String msStr    = filename.substring(lastUnderscore + 1, msIndex);
                        displayInfo = "Offline Index: **" + indexStr + "** at **" + msStr + "ms**";
                    }
                }

                // Build file entry link
                html += "<a href='" + filename + "'>";

                String fileType = (filename.endsWith(".jpg") || filename.endsWith(".jpeg"))
                                  ? " (JPG)" : " (TXT)";

                // Format file modification time
                struct tm* tm;
                time_t     t = file.getLastWrite();
                tm = localtime(&t);
                char timeBuf[30];
                strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", tm);

                if (!displayInfo.isEmpty()) {
                    html += "Capture" + fileType + " (" + displayInfo +
                            ") (FS Time: " + String(timeBuf) + ")";
                } else {
                    html += filename.substring(1) + fileType +
                            " (FS Time: " + String(timeBuf) + ") (" +
                            String(file.size()) + " bytes)";
                }
                html += "</a>";

                fileCount++;
            }
            file.close();
            file = root.openNextFile();
        }
        root.close();
    } else {
        html += "<p>Error reading directory.</p>";
    }

    html += "<hr><p>Total Files Found on Disk: " + String(fileCount) + "</p>";
    html += "</body></html>";

    server.send(200, "text/html", html);
    xSemaphoreGive(littleFSMutex);
}

/**
 * @brief Handles 404 Not Found responses
 */
static void handleNotFound(void) {
    if (!handleFileRead(server.uri())) {
        server.send(404, "text/plain", "File Not Found");
    }
}

// =============================================================================
// SETUP
// =============================================================================

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    // Uncomment below when connecting to SenseCraft
    // while (!Serial) delay(100);
    delay(100);

    // Configure logging levels
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("sscma", ESP_LOG_INFO);

    Serial.println("\n=== ESP32-S3 AI Capture System Initializing ===");

    // -------------------------------------------------------------------------
    // Initialize FreeRTOS Mutex
    // -------------------------------------------------------------------------
    littleFSMutex = xSemaphoreCreateMutex();
    if (littleFSMutex == NULL) {
        Serial.println("CRITICAL: Failed to create LittleFS mutex.");
        while (true) {
            delay(1000);
        }
    }

    // -------------------------------------------------------------------------
    // Initialize Hardware Pins
    // -------------------------------------------------------------------------
    pinMode(PIN_LED_ORANGE, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    pinMode(PIN_HOOTER, OUTPUT);

    // -------------------------------------------------------------------------
    // Initialize LittleFS
    // -------------------------------------------------------------------------
    if (!LittleFS.begin(true, "/spiffs")) {
        Serial.println("CRITICAL: Failed to mount/format LittleFS.");
        while (true) {
            delay(1000);
        }
    }

    // Perform initial file count
    if (xSemaphoreTake(littleFSMutex, portMAX_DELAY) == pdTRUE) {
        getOldestFileName();
        xSemaphoreGive(littleFSMutex);
    }

    // -------------------------------------------------------------------------
    // Initialize SSCMA AI
    // -------------------------------------------------------------------------
#if SSCMA_CONNECT_TO_XIAO_S3
    Wire.begin(SDA, SCL, 1000000);
    AI.begin(&Wire);
#else
    Wire.begin();
    AI.begin(&Wire, D3);
#endif

    // -------------------------------------------------------------------------
    // Initialize Wi-Fi and Web Server
    // -------------------------------------------------------------------------
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    const unsigned long WIFI_TIMEOUT_MS = 20000;
    unsigned long       startTime       = millis();

    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < WIFI_TIMEOUT_MS)) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
        Serial.println("\nConnected! NTP time sync initiated.");

        IPAddress ip = WiFi.localIP();
        server.on("/", handleRoot);
        server.onNotFound(handleNotFound);
        server.begin();
        webServerInitialized = true;

        Serial.printf("Web Server started at: http://%s\n", ip.toString().c_str());
    } else {
        Serial.println("\nWARNING: Wi-Fi connection failed. Running in OFFLINE mode.");
        webServerInitialized = false;
    }

    Serial.println("Setup complete. Starting AI detection loop...\n");
}

// =============================================================================
// MAIN LOOP (Core 0)
// =============================================================================

void loop() {
    static unsigned long lastDetectTime = 0;

    // Handle web server requests
    if (webServerInitialized) {
        server.handleClient();
    }

    // -------------------------------------------------------------------------
    // AI Detection Logic (runs at configured interval)
    // -------------------------------------------------------------------------
    if (millis() - lastDetectTime < DETECTION_INTERVAL_MS) {
        return;
    }
    lastDetectTime = millis();

    // Run AI inference
    int sceneRet = checkScene();

    // Detection state variables
    bool   hasRisk              = false;
    bool   allSafe              = true;
    int    objectCount          = 0;
    int    currentDetectionState = 0;
    String detectedLabelsLog    = "";
    String serialLogDetails     = "";
    String base64Image          = "";

    // -------------------------------------------------------------------------
    // Process AI Results
    // -------------------------------------------------------------------------
    if (sceneRet == CMD_OK) {
        objectCount = AI.boxes().size();

        if (objectCount > 0) {
            lastPositiveDetectionTime = millis();
        }

        // Analyze each detected object
        for (int i = 0; i < objectCount; i++) {
            auto    box   = AI.boxes()[i];
            uint8_t label = box.target;

            if (label < LABEL_COUNT) {
                const char* labelName = LABEL_NAMES[label];

                // Build labels string for logging
                if (detectedLabelsLog.length() > 0) {
                    detectedLabelsLog += ", ";
                }
                detectedLabelsLog += labelName;

                // Build detailed serial log
                if (serialLogDetails.length() > 0) {
                    serialLogDetails += " | ";
                }
                serialLogDetails += String(labelName) + " (" + String(box.score, 2) + ")";
            }

            // Classify object as risk or safe
            switch (label) {
                case LABEL_FACE:
                case LABEL_FAKE_TURBAN:
                case LABEL_HALF:
                    hasRisk = true;
                    allSafe = false;
                    break;
                case LABEL_FULL:
                case LABEL_TURBAN:
                    // Legal objects - no action needed
                    break;
                default:
                    allSafe = false;
                    break;
            }
        }

        // Rule: 3+ objects forces RISK state
        if (objectCount >= 3) {
            hasRisk = true;
            Serial.println("[RULE] Detected 3+ objects. Forcing RISK state.");
        }

        // Determine detection state
        if (hasRisk) {
            currentDetectionState = 1;  // RED: Risk
        } else if (allSafe && objectCount > 0) {
            currentDetectionState = 2;  // GREEN: Safe
        } else {
            currentDetectionState = 0;  // BLUE: No relevant objects
        }
    } else {
        currentDetectionState = 0;
        detectedLabelsLog     = "AI TIMEOUT/INVOKE FAILED";
        serialLogDetails      = "AI TIMEOUT/INVOKE FAILED";
    }

    // -------------------------------------------------------------------------
    // Serial Monitor Logging
    // -------------------------------------------------------------------------
    if (sceneRet == CMD_OK && !serialLogDetails.isEmpty()) {
        Serial.printf("[Detect] State %d: %s (Count: %d)\n",
                      currentDetectionState, serialLogDetails.c_str(), objectCount);
    } else if (sceneRet == CMD_OK && objectCount == 0) {
        Serial.printf("[Detect] State %d: No Objects Detected\n", currentDetectionState);
    } else if (sceneRet != CMD_OK) {
        Serial.printf("[Detect] State %d: AI Model Invoke Failed.\n", currentDetectionState);
    }

    // -------------------------------------------------------------------------
    // Debounce Logic - Hold previous positive state during brief gaps
    // -------------------------------------------------------------------------
    if (currentDetectionState == 0) {
        if (millis() - lastPositiveDetectionTime < NO_SCENE_HOLD_TIME_MS) {
            if (lastDetectionState > 0) {
                currentDetectionState = lastDetectionState;
                Serial.printf("[HOLD] Debounce active. Holding previous state %d.\n",
                              currentDetectionState);
            }
        }
    }

    // -------------------------------------------------------------------------
    // State Change Detection and Image Capture
    // -------------------------------------------------------------------------
    if (currentDetectionState != lastDetectionState) {
        // Update outputs immediately on state change
        if (currentDetectionState == 1) {
            enableHooter();
        } else {
            disableHooter();
        }
        updateLEDs(currentDetectionState);

        // Capture image on meaningful state transitions
        bool shouldCapture = (currentDetectionState != 0) ||
                             (lastDetectionState == 1 || lastDetectionState == 2);

        if (shouldCapture) {
            // Request image from AI (blocking call)
            int ret = AI.invoke(1, false, true);

            if (ret == CMD_OK) {
                base64Image = AI.last_image();

                if (base64Image.length() > 0) {
                    // Create capture data for async task
                    CaptureData_t* data = new CaptureData_t{
                        .base64Image    = base64Image,
                        .labelsString   = detectedLabelsLog,
                        .detectionState = currentDetectionState
                    };

                    // Spawn async save task on Core 1
                    xTaskCreatePinnedToCore(
                        saveImageTask,      // Task function
                        "SaveImageTask",    // Task name
                        8192,               // Stack size
                        (void*)data,        // Parameters
                        1,                  // Priority
                        NULL,               // Task handle
                        1                   // Pin to Core 1
                    );

                    Serial.println("[ASYNC] Save task created on Core 1.");
                }
            }
        }

        lastDetectionState = currentDetectionState;
    }

    // -------------------------------------------------------------------------
    // Continuous Output Update (maintains state during non-transitions)
    // -------------------------------------------------------------------------
    if (currentDetectionState == 1) {
        enableHooter();
    } else {
        disableHooter();
    }
    updateLEDs(currentDetectionState);
}

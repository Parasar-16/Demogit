#include <Arduino.h>
#include <Wire.h>
#include <Seeed_Arduino_SSCMA.h>
#include <Ticker.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
#define LED_RED     D0
#define LED_GREEN   D1
#define LED_BLUE    D7
#define LED_ORANGE  D2
#define HOOTER      D6

// ============================================================================
// HARDWARE INSTANCES
// ============================================================================
SSCMA AI;
Ticker hooter_off;

#define SSCMA_CONNECT_TO_XIAO_S3 1

// ============================================================================
// DETECTION LABELS
// ============================================================================
#define FACE        0   // No helmet - RISK
#define FAKE_TURBAN 1   // Fake covering - RISK
#define FULL        2   // Full helmet - SAFE
#define HALF        3   // Half helmet/partial - RISK
#define TURBAN      4   // Turban (acceptable) - SAFE

// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================
#define HOOTER_ON_PERIOD_SEC    60

// ============================================================================
// DAMPING CONFIGURATION
// ============================================================================
const int BUFFER_SIZE = 10;
const int NOSCENE_THRESHOLD = 5;

// Multipliers: Negative = risk, Positive = safe
// Index: FACE=0, FAKE_TURBAN=1, FULL=2, HALF=3, TURBAN=4
static int BufferDirection[5] = { -1, -1, 2, -1, 2 };

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================
void ProcessLEDs(uint8_t state);
void ProcessHooter(uint8_t state);
void UpdateLEDs(uint8_t state);
void UpdateHooter(uint8_t state);
void EnableHooter(void);
void DisableHooter(void);
int CheckScene(void);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_ORANGE, OUTPUT);
    pinMode(HOOTER, OUTPUT);

    Serial.begin(115200);
    delay(2000);

#if SSCMA_CONNECT_TO_XIAO_S3
    Wire.begin(SDA, SCL, 1000000);
    AI.begin(&Wire);
#endif

    Serial.println("--- System Initialized ---");
    Serial.println("Multi-Detection: 2+ detections = RISK");
}

// ============================================================================
// MAIN LOOP
// Detection logic with damping:
// - All detections are summed using BufferDirection multipliers
// - FACE, FAKE_TURBAN, HALF = negative score (risk)
// - FULL, TURBAN = positive score (safe, 2x weight)
// - buffer_sum > 0 = GREEN, buffer_sum < 0 = RED, buffer_sum == 0 = ORANGE
//
// Examples:
// 3 detections (triple riding) -> ALWAYS RED
// - [FULL, FULL, FULL] -> RED (triple riding)
// - [FACE, FULL, TURBAN] -> RED (triple riding)
//
// 2 detections:
// - [FULL, FULL] -> ALL safe = GREEN
// - [FULL, TURBAN] -> ALL safe = GREEN
// - [FACE, FULL] -> has risk (FACE) = RED
// - [FACE, FACE] -> has risk = RED
//
// 1 detection: uses damping buffer with weighted scores
// ============================================================================
void loop() {
    static uint8_t LEDState = 0;
    static uint8_t HooterState = 0;

    // Damping variables
    static int buffer[BUFFER_SIZE] = {0};
    static int buffer_index = 0;
    static int buffer_sum = 0;
    static int noSceneCount = 0;
    static unsigned int showNoScene = 1;

    if (CheckScene() == CMD_OK) {
        int detectionCount = AI.boxes().size();

        if (detectionCount > 0) {
            showNoScene = 1;
            noSceneCount = 0;

            // Print all detections
            Serial.print("Detected: ");
            for (int i = 0; i < detectionCount; i++) {
                uint8_t label = AI.boxes()[i].target;
                switch(label) {
                    case FACE:        Serial.print("[FACE] "); break;
                    case FAKE_TURBAN: Serial.print("[FAKE_TURBAN] "); break;
                    case HALF:        Serial.print("[HALF_COVER] "); break;
                    case FULL:        Serial.print("[FULL_COVER] "); break;
                    case TURBAN:      Serial.print("[TURBAN] "); break;
                    default:          Serial.print("[UNKNOWN] "); break;
                }
            }
            Serial.println();

            // ================================================================
            // DETECTION LOGIC
            // For 2+ detections: If ANY is risky = RED, ALL safe = GREEN
            // For 1 detection: Use damping buffer with weighted score
            // ================================================================
            bool hasRisk = false;
            int newScore = 0;

            // Check all detections for any risky label
            for (int i = 0; i < detectionCount; i++) {
                uint8_t label = AI.boxes()[i].target;
                int score = AI.boxes()[i].score;

                // Check if this is a risky detection (FACE, FAKE_TURBAN, HALF)
                if (label == FACE || label == FAKE_TURBAN || label == HALF) {
                    hasRisk = true;
                }

                // Calculate weighted score for damping (single detection only)
                if (detectionCount == 1 && label < 5) {
                    newScore = score * BufferDirection[label];
                }
            }

            // ================================================================
            // TRIPLE DETECTION (3 persons) - Always RED (triple riding)
            // ================================================================
            if (detectionCount >= 3) {
                Serial.printf(">>> TRIPLE RIDING (%d persons) = RED\n", detectionCount);
                LEDState = (1 << 0);  // RED
                HooterState = 1;

                // Push negative score to buffer for consistency
                newScore = -100 * detectionCount;

                // Update buffer
                buffer_sum -= buffer[buffer_index];
                buffer[buffer_index] = newScore;
                buffer_sum += newScore;
                buffer_index = (buffer_index + 1) % BUFFER_SIZE;
            }
            // ================================================================
            // DOUBLE DETECTION (2 persons)
            // If ANY is risky = RED, ALL safe = GREEN
            // ================================================================
            else if (detectionCount == 2) {
                if (hasRisk) {
                    // Any risky label = RED
                    Serial.printf(">>> DOUBLE (%d): HAS RISK = RED\n", detectionCount);
                    LEDState = (1 << 0);  // RED
                    HooterState = 1;

                    // Push negative score to buffer for consistency
                    newScore = -100 * detectionCount;
                } else {
                    // All safe (FULL/TURBAN only) = GREEN
                    Serial.printf(">>> DOUBLE (%d): ALL SAFE = GREEN\n", detectionCount);
                    LEDState = (1 << 1);  // GREEN
                    HooterState = 0;

                    // Push positive score to buffer for consistency
                    newScore = 100 * detectionCount;
                }

                // Update buffer
                buffer_sum -= buffer[buffer_index];
                buffer[buffer_index] = newScore;
                buffer_sum += newScore;
                buffer_index = (buffer_index + 1) % BUFFER_SIZE;
            }
            // ================================================================
            // SINGLE DETECTION - Use damping buffer
            // ================================================================
            else {
                // Update damping buffer
                buffer_sum -= buffer[buffer_index];
                buffer[buffer_index] = newScore;
                buffer_sum += newScore;
                buffer_index = (buffer_index + 1) % BUFFER_SIZE;

                // Determine state based on buffer_sum
                if (buffer_sum > 0) {
                    Serial.printf(">>> SINGLE: SAFE (GREEN) - score: %d, sum: %d\n",
                                  newScore, buffer_sum);
                    LEDState = (1 << 1);  // GREEN
                    HooterState = 0;
                }
                else if (buffer_sum < 0) {
                    Serial.printf(">>> SINGLE: RISK (RED) - score: %d, sum: %d\n",
                                  newScore, buffer_sum);
                    LEDState = (1 << 0);  // RED
                    HooterState = 1;
                }
                else {
                    Serial.printf(">>> SINGLE: NEUTRAL (ORANGE) - score: %d, sum: %d\n",
                                  newScore, buffer_sum);
                    LEDState = (1 << 3);  // ORANGE
                    HooterState = 0;
                }
            }
        }
        else {
            // No detections - empty frame
            noSceneCount++;

            if (noSceneCount >= NOSCENE_THRESHOLD) {
                // Clear the buffer when bike leaves frame
                buffer_sum = 0;
                buffer_index = 0;
                memset(buffer, 0, sizeof(buffer));
                noSceneCount = 0;
                Serial.println("  [Buffer cleared]");
            }

            if (showNoScene) {
                Serial.printf("Status: EMPTY (%d/%d to clear)\n", noSceneCount, NOSCENE_THRESHOLD);
                LEDState = (1 << 3);  // ORANGE
                HooterState = 0;
                showNoScene = 0;
            }
        }
    }
    else {
        // Camera communication error
        noSceneCount++;

        if (noSceneCount >= NOSCENE_THRESHOLD) {
            buffer_sum = 0;
            buffer_index = 0;
            memset(buffer, 0, sizeof(buffer));
            noSceneCount = 0;
        }

        if (showNoScene) {
            Serial.println("!! CAMERA ERROR (BLUE LED)");
            LEDState = (1 << 2);  // BLUE
            HooterState = 0;
            showNoScene = 0;
        }
    }

    ProcessLEDs(LEDState);
    ProcessHooter(HooterState);
    delay(500);
}

// ============================================================================
// HARDWARE CONTROL - LED
// ============================================================================
void ProcessLEDs(uint8_t state) {
    static uint8_t prev = 0;
    if (prev != state) {
        UpdateLEDs(state);
        prev = state;
    }
}

void UpdateLEDs(uint8_t state) {
    digitalWrite(LED_RED,    (state & (1 << 0)) ? HIGH : LOW);
    digitalWrite(LED_GREEN,  (state & (1 << 1)) ? HIGH : LOW);
    digitalWrite(LED_BLUE,   (state & (1 << 2)) ? HIGH : LOW);
    digitalWrite(LED_ORANGE, (state & (1 << 3)) ? HIGH : LOW);
}

// ============================================================================
// HARDWARE CONTROL - HOOTER
// ============================================================================
void ProcessHooter(uint8_t state) {
    static uint8_t prev = 0;
    if (prev != state) {
        UpdateHooter(state);
        prev = state;
    }
}

void UpdateHooter(uint8_t state) {
    if (state) EnableHooter();
    else DisableHooter();
}

void EnableHooter(void) {
    digitalWrite(HOOTER, HIGH);
    hooter_off.attach(HOOTER_ON_PERIOD_SEC, DisableHooter);
}

void DisableHooter(void) {
    digitalWrite(HOOTER, LOW);
    hooter_off.detach();
}

// ============================================================================
// AI CAMERA INTERFACE
// ============================================================================
int CheckScene(void) {
    int retval = CMD_ETIMEDOUT;
    int retry = 3;
    do {
        retval = AI.invoke();
    } while (retval != CMD_OK && --retry > 0);
    return retval;
}

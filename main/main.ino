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
// Labels returned by the AI model for helmet detection
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
// The damping system uses a circular buffer to smooth detection results,
// preventing rapid state changes from momentary misdetections.
// ============================================================================

// Buffer size determines response inertia
// Larger = slower state transitions, more stable
// Smaller = faster response, more sensitive to noise
const int BUFFER_SIZE = 10;

// Number of consecutive empty frames before clearing the buffer
// This prevents the system from staying in alert when the bike leaves
const int NOSCENE_THRESHOLD = 5;

// ============================================================================
// SCORING MULTIPLIERS
// These multipliers determine how each detection type affects the buffer sum.
//
// MULTI-DETECTION SCORING LOGIC:
// - Each frame can contain multiple detections (e.g., 3 people on a bike)
// - ALL detections in a frame are summed together (additive scoring)
// - Negative multipliers indicate risk (helmet violation)
// - Positive multipliers indicate compliance (proper helmet)
// - The 2x positive multiplier helps safe detections overcome risk faster
//
// Example: Frame with [FULL, FULL, FACE] at scores 80, 75, 90:
//   frameTotalScore = (80 * 2) + (75 * 2) + (90 * -1) = 160 + 150 - 90 = 220
//   Result: Positive score, trends toward SAFE
//
// Example: Frame with [FACE, FACE, HALF] at scores 85, 80, 70:
//   frameTotalScore = (85 * -1) + (80 * -1) + (70 * -1) = -235
//   Result: Strong negative, trends toward RISK
// ============================================================================
// Index:              FACE  FAKE_TURBAN  FULL  HALF  TURBAN
static int BufferDirection[5] = { -1,     -1,      2,   -1,    2 };

// ============================================================================
// LED STATE CONSTANTS
// ============================================================================
#define LED_STATE_OFF     0
#define LED_STATE_RED     (1 << 0)
#define LED_STATE_GREEN   (1 << 1)
#define LED_STATE_BLUE    (1 << 2)
#define LED_STATE_ORANGE  (1 << 3)

// ============================================================================
// GLOBAL STATE VARIABLES (for damping buffer)
// ============================================================================
static int buffer[BUFFER_SIZE] = {0};  // Circular buffer for frame scores
static int buffer_index = 0;           // Current position in circular buffer
static int buffer_sum = 0;             // Running sum of all buffer values
static int noSceneCount = 0;           // Consecutive empty frame counter
static bool showNoScene = true;        // Suppress repeated "no scene" messages

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

// Core detection processing
int  CheckScene(void);
int  CalculateFrameScore(void);
void UpdateDampingBuffer(int frameTotalScore);
void DetermineSystemState(int frameTotalScore, uint8_t &ledState, uint8_t &hooterState);
void HandleNoDetection(uint8_t &ledState, uint8_t &hooterState);
void HandleCameraError(uint8_t &ledState, uint8_t &hooterState);
void ClearDampingBuffer(void);

// Hardware control
void ProcessLEDs(uint8_t state);
void ProcessHooter(uint8_t state);
void UpdateLEDs(uint8_t state);
void UpdateHooter(uint8_t state);
void EnableHooter(void);
void DisableHooter(void);

// Debug output
void PrintDetections(void);
const char* GetLabelName(uint8_t label);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    // Initialize GPIO pins
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_ORANGE, OUTPUT);
    pinMode(HOOTER, OUTPUT);

    // Initialize serial communication
    Serial.begin(115200);
    delay(2000);

    // Initialize AI camera connection
#if SSCMA_CONNECT_TO_XIAO_S3
    Wire.begin(SDA, SCL, 1000000);
    AI.begin(&Wire);
#endif

    Serial.println("===========================================");
    Serial.println("  Safety Helmet Detection System v2.0");
    Serial.println("  Multi-Detection Additive Scoring");
    Serial.println("===========================================");
    Serial.printf("  Buffer Size: %d frames\n", BUFFER_SIZE);
    Serial.printf("  No-Scene Threshold: %d frames\n", NOSCENE_THRESHOLD);
    Serial.println("===========================================");
}

// ============================================================================
// MAIN LOOP
// Handles AI detection, damping buffer updates, and hardware control.
// ============================================================================
void loop() {
    uint8_t ledState = LED_STATE_OFF;
    uint8_t hooterState = 0;

    // Attempt to get detection data from AI camera
    if (CheckScene() == CMD_OK) {

        // Check if any objects were detected in this frame
        if (AI.boxes().size() > 0) {
            // Reset no-scene tracking since we have valid detections
            showNoScene = true;
            noSceneCount = 0;

            // Print all detections for debugging
            PrintDetections();

            // Calculate combined score from ALL detections in this frame
            int frameTotalScore = CalculateFrameScore();

            // Update the damping buffer with this frame's combined score
            UpdateDampingBuffer(frameTotalScore);

            // Determine LED and hooter state based on buffer_sum
            DetermineSystemState(frameTotalScore, ledState, hooterState);
        }
        else {
            // AI working but no objects detected (empty frame)
            HandleNoDetection(ledState, hooterState);
        }
    }
    else {
        // Camera communication error
        HandleCameraError(ledState, hooterState);
    }

    // Update hardware outputs (only changes when state differs)
    ProcessLEDs(ledState);
    ProcessHooter(hooterState);

    delay(500);
}

// ============================================================================
// CORE DETECTION PROCESSING
// ============================================================================

/**
 * CalculateFrameScore - Additive Multi-Detection Scoring
 *
 * This function processes ALL detections in the current frame and combines
 * their weighted scores into a single value. This handles scenarios like:
 * - Single rider with/without helmet
 * - Double riding (2 people)
 * - Triple riding (3 people)
 *
 * Each detection's score is multiplied by its BufferDirection value:
 * - Risk detections (FACE, FAKE_TURBAN, HALF) contribute negative scores
 * - Safe detections (FULL, TURBAN) contribute positive scores (2x weight)
 *
 * The combined score for the frame is then stored in the damping buffer.
 *
 * @return Combined weighted score for all detections in current frame
 */
int CalculateFrameScore(void) {
    int frameTotalScore = 0;
    int detectionCount = AI.boxes().size();

    Serial.printf("  Processing %d detection(s):\n", detectionCount);

    // Iterate through ALL detections and sum their weighted scores
    for (int i = 0; i < detectionCount; i++) {
        uint8_t label = AI.boxes()[i].target;
        int score = AI.boxes()[i].score;
        int weightedScore = 0;

        // Apply direction multiplier based on detection type
        if (label < 5) {
            weightedScore = score * BufferDirection[label];
        }
        // Unknown labels contribute 0 to avoid undefined behavior

        frameTotalScore += weightedScore;

        // Debug output for each detection
        Serial.printf("    [%d] %s: raw=%d, weight=%d, weighted=%d\n",
                      i, GetLabelName(label), score,
                      (label < 5) ? BufferDirection[label] : 0,
                      weightedScore);
    }

    Serial.printf("  Frame Total Score: %d\n", frameTotalScore);
    return frameTotalScore;
}

/**
 * UpdateDampingBuffer - Circular Buffer Management
 *
 * Maintains a circular buffer of frame scores to smooth out detection noise.
 * Uses a running sum for O(1) buffer_sum calculation.
 *
 * @param frameTotalScore Combined score from current frame's detections
 */
void UpdateDampingBuffer(int frameTotalScore) {
    // Remove the old value at current index from running sum
    buffer_sum -= buffer[buffer_index];

    // Store new frame score and add to running sum
    buffer[buffer_index] = frameTotalScore;
    buffer_sum += frameTotalScore;

    // Advance circular buffer index
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}

/**
 * DetermineSystemState - State Decision Based on Buffer Sum
 *
 * Uses the accumulated buffer_sum to determine system state:
 * - Positive sum = More safe detections overall = GREEN (safe)
 * - Negative sum = More risk detections overall = RED (alert)
 * - Zero sum = Neutral/transitioning = ORANGE (caution)
 *
 * @param frameTotalScore Current frame's score (for logging)
 * @param ledState Output: LED state to set
 * @param hooterState Output: Hooter state to set
 */
void DetermineSystemState(int frameTotalScore, uint8_t &ledState, uint8_t &hooterState) {
    if (buffer_sum > 0) {
        // Positive buffer sum: majority safe detections
        Serial.printf(">>> SAFE (GREEN) | frame=%d, buffer_sum=%d\n",
                      frameTotalScore, buffer_sum);
        ledState = LED_STATE_GREEN;
        hooterState = 0;
    }
    else if (buffer_sum < 0) {
        // Negative buffer sum: majority risk detections
        Serial.printf(">>> RISK (RED) | frame=%d, buffer_sum=%d\n",
                      frameTotalScore, buffer_sum);
        ledState = LED_STATE_RED;
        hooterState = 1;
    }
    else {
        // Zero buffer sum: transitional/neutral state
        Serial.printf(">>> NEUTRAL (ORANGE) | frame=%d, buffer_sum=%d\n",
                      frameTotalScore, buffer_sum);
        ledState = LED_STATE_ORANGE;
        hooterState = 0;
    }
}

/**
 * HandleNoDetection - Empty Frame Processing
 *
 * Called when AI camera returns valid data but no objects detected.
 * Uses noSceneCount to implement hysteresis - only clears buffer after
 * NOSCENE_THRESHOLD consecutive empty frames (bike has left the frame).
 */
void HandleNoDetection(uint8_t &ledState, uint8_t &hooterState) {
    noSceneCount++;

    // Check if we've hit the threshold to clear the buffer
    if (noSceneCount >= NOSCENE_THRESHOLD) {
        ClearDampingBuffer();
        noSceneCount = 0;  // Reset counter after clearing
    }

    // Only print "no scene" message once to avoid log spam
    if (showNoScene) {
        Serial.printf("Status: NO DETECTION (%d/%d to clear)\n",
                      noSceneCount, NOSCENE_THRESHOLD);
        showNoScene = false;
    }

    ledState = LED_STATE_ORANGE;
    hooterState = 0;
}

/**
 * HandleCameraError - Communication Failure Processing
 *
 * Called when AI.invoke() fails or times out.
 * Similar to HandleNoDetection but sets BLUE LED to indicate error.
 */
void HandleCameraError(uint8_t &ledState, uint8_t &hooterState) {
    noSceneCount++;

    if (noSceneCount >= NOSCENE_THRESHOLD) {
        ClearDampingBuffer();
        noSceneCount = 0;
    }

    if (showNoScene) {
        Serial.println("!! CAMERA ERROR (BLUE LED)");
        showNoScene = false;
    }

    ledState = LED_STATE_BLUE;
    hooterState = 0;
}

/**
 * ClearDampingBuffer - Reset Buffer State
 *
 * Clears the damping buffer when bike leaves frame or on error recovery.
 * This ensures the system doesn't carry stale state to the next detection.
 */
void ClearDampingBuffer(void) {
    memset(buffer, 0, sizeof(buffer));
    buffer_index = 0;
    buffer_sum = 0;
    Serial.println("  [Buffer cleared]");
}

// ============================================================================
// DEBUG OUTPUT HELPERS
// ============================================================================

/**
 * PrintDetections - Log All Detections in Current Frame
 */
void PrintDetections(void) {
    Serial.print("Detected: ");
    for (int i = 0; i < AI.boxes().size(); i++) {
        Serial.printf("[%s] ", GetLabelName(AI.boxes()[i].target));
    }
    Serial.println();
}

/**
 * GetLabelName - Convert Label ID to Human-Readable String
 */
const char* GetLabelName(uint8_t label) {
    switch (label) {
        case FACE:        return "FACE";
        case FAKE_TURBAN: return "FAKE_TURBAN";
        case FULL:        return "FULL_COVER";
        case HALF:        return "HALF_COVER";
        case TURBAN:      return "TURBAN";
        default:          return "UNKNOWN";
    }
}

// ============================================================================
// HARDWARE CONTROL - LED
// ============================================================================

/**
 * ProcessLEDs - State-Change Detection for LEDs
 * Only updates hardware when state actually changes.
 */
void ProcessLEDs(uint8_t state) {
    static uint8_t prevState = 0xFF;  // Initialize to invalid state
    if (prevState != state) {
        UpdateLEDs(state);
        prevState = state;
    }
}

/**
 * UpdateLEDs - Direct LED Hardware Control
 */
void UpdateLEDs(uint8_t state) {
    digitalWrite(LED_RED,    (state & LED_STATE_RED)    ? HIGH : LOW);
    digitalWrite(LED_GREEN,  (state & LED_STATE_GREEN)  ? HIGH : LOW);
    digitalWrite(LED_BLUE,   (state & LED_STATE_BLUE)   ? HIGH : LOW);
    digitalWrite(LED_ORANGE, (state & LED_STATE_ORANGE) ? HIGH : LOW);
}

// ============================================================================
// HARDWARE CONTROL - HOOTER
// ============================================================================

/**
 * ProcessHooter - State-Change Detection for Hooter
 * Only updates hardware when state actually changes.
 */
void ProcessHooter(uint8_t state) {
    static uint8_t prevState = 0xFF;
    if (prevState != state) {
        UpdateHooter(state);
        prevState = state;
    }
}

/**
 * UpdateHooter - Hooter State Control
 */
void UpdateHooter(uint8_t state) {
    if (state) {
        EnableHooter();
    } else {
        DisableHooter();
    }
}

/**
 * EnableHooter - Turn On Hooter with Auto-Off Timer
 */
void EnableHooter(void) {
    digitalWrite(HOOTER, HIGH);
    hooter_off.attach(HOOTER_ON_PERIOD_SEC, DisableHooter);
}

/**
 * DisableHooter - Turn Off Hooter and Cancel Timer
 */
void DisableHooter(void) {
    digitalWrite(HOOTER, LOW);
    hooter_off.detach();
}

// ============================================================================
// AI CAMERA INTERFACE
// ============================================================================

/**
 * CheckScene - Query AI Camera for Detections
 * Implements retry logic for robustness.
 *
 * @return CMD_OK on success, error code on failure
 */
int CheckScene(void) {
    int retval = CMD_ETIMEDOUT;
    int retry = 3;

    do {
        retval = AI.invoke();
    } while (retval != CMD_OK && --retry > 0);

    return retval;
}

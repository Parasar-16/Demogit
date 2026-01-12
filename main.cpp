#include <Arduino.h>
#include <Wire.h>
#include <Seeed_Arduino_SSCMA.h>
#include <Ticker.h>

// 1. Pin definitions
#define LED_RED     D0
#define LED_GREEN   D1
#define LED_BLUE    D7
#define LED_ORANGE  D2
#define HOOTER      D6

SSCMA AI;
Ticker hooter_off;

#define SSCMA_CONNECT_TO_XIAO_S3 1

// 2. Labels mapping
#define FACE        0
#define FAKE_TURBAN 1
#define FULL        2
#define HALF        3
#define TURBAN      4

#define HOOTER_ON_PERIOD_SEC    60

// 3. Damping configuration
// The number of score values to hold for dampening the reporting
// The larger the buffer, the longer it takes to switch between negative and positive readings
const int BUFFER_SIZE = 10;

// How many no_scenes before clearing the alert
// The smaller the number, the more sensitive the system is to changing state when a noScene is detected
// The value is approximately the number of seconds it takes to return to a noScene state
const int NOSCENE_THRESHOLD = 5;

// Multipliers for the buffer sum value for the labels returned by the AI
// Negative values indicate risk (FACE=0, FAKE_TURBAN=1, HALF=3)
// Positive values indicate safe (FULL=2, TURBAN=4)
// Index: FACE=0, FAKE_TURBAN=1, FULL=2, HALF=3, TURBAN=4
static int BufferDirection[5] = { -1, -1, 2, -1, 2 };

// 4. Function Prototypes
void ProcessLEDs(uint8_t state);
void ProcessHooter(uint8_t state);
void UpdateLEDs(uint8_t state);
void UpdateHooter(uint8_t state);
void EnableHooter();
void DisableHooter();
int CheckScene();

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
}

void loop() {
  static uint8_t LEDState = 0;
  static uint8_t HooterState = 0;

  // Damping variables
  static int buffer[BUFFER_SIZE] = {0};  // Initialize buffer with zeros
  static int buffer_index = 0;           // Next buffer entry to be used
  static int buffer_sum = 0;             // Sum of buffer indicating +ve or -ve status
  static int noSceneCount = 0;           // Number of consecutive no scenes
  static unsigned int showNoScene = 1;   // Control whether no scene is printed

  if (CheckScene() == CMD_OK) {
    if (AI.boxes().size() > 0) {
      int score = 0;
      int newScore = 0;
      uint8_t label = 0;

      showNoScene = 1;
      noSceneCount = 0;

      Serial.print("Detected: ");

      // Find the detection with highest score
      for (int i = 0; i < AI.boxes().size(); i++) {
        uint8_t currentLabel = AI.boxes()[i].target;
        int currentScore = AI.boxes()[i].score;

        // Print all detections
        switch(currentLabel) {
          case FACE:        Serial.print("[FACE] "); break;
          case FAKE_TURBAN: Serial.print("[FAKE_TURBAN] "); break;
          case HALF:        Serial.print("[HALF_COVER] "); break;
          case FULL:        Serial.print("[FULL_COVER] "); break;
          case TURBAN:      Serial.print("[TURBAN] "); break;
          default:          Serial.print("[UNKNOWN] "); break;
        }

        // Track highest scoring detection
        if (i == 0) {
          label = currentLabel;
          score = currentScore;
        } else if (currentScore > score) {
          label = currentLabel;
          score = currentScore;
        }
      }
      Serial.println();

      /* BUFFERING / DAMPING LOGIC */

      // First remove value at the current buffer index
      buffer_sum -= buffer[buffer_index];

      // Update the score based on positive or negative result from the AI
      // Ensure label is within bounds
      if (label < 5) {
        newScore = score * BufferDirection[label];
      } else {
        newScore = 0;
      }

      // Add the new weighted score to the sum
      buffer_sum += newScore;

      // Overwrite the new value in the buffer
      buffer[buffer_index] = newScore;

      // Increment the index and roll the buffer at BUFFER_SIZE
      buffer_index = (buffer_index + 1) % BUFFER_SIZE;

      /* STATUS REPORTING based on buffer_sum */

      // If the buffer_sum is positive, we have more positive (safe) results
      if (buffer_sum > 0) {
        Serial.printf(">>> Status: SAFE (GREEN LED) - index: %02d - score: %03d - sum: %d\n",
                      buffer_index, newScore, buffer_sum);
        LEDState = (1 << 1); // GREEN
        HooterState = 0;
      }
      // If the sum is negative, we have a risk alert
      else if (buffer_sum < 0) {
        Serial.printf(">>> Status: RISK (RED LED) - index: %02d - score: %03d - sum: %d\n",
                      buffer_index, newScore, buffer_sum);
        LEDState = (1 << 0); // RED
        HooterState = 1;
      }
      // If the sum is zero, neutral state (orange)
      else {
        Serial.printf(">>> Status: NEUTRAL (ORANGE LED) - index: %02d - score: %03d - sum: %d\n",
                      buffer_index, newScore, buffer_sum);
        LEDState = (1 << 3); // ORANGE
        HooterState = 0;
      }

    } else {
      // AI is working, but sees NO objects (Empty Frame)
      noSceneCount = (noSceneCount + 1) % NOSCENE_THRESHOLD;

      // Ensure a buffer of NOSCENE_THRESHOLD number of noScenes before declaring No Scene
      if (noSceneCount == 0) {
        // Clear the buffer
        buffer_sum = 0;
        buffer_index = 0;
        memset(buffer, 0, sizeof(buffer));
      }

      if (showNoScene) {
        Serial.println("Status: EMPTY - No Scene (ORANGE LED)");
        LEDState = (1 << 3); // ORANGE
        HooterState = 0;
        showNoScene = 0; // Don't display 'No scene' again until state changes
      }
    }
  }
  else {
    // Communication failure / Camera Error
    noSceneCount = (noSceneCount + 1) % NOSCENE_THRESHOLD;

    if (noSceneCount == 0) {
      // Clear the buffer after threshold
      buffer_sum = 0;
      buffer_index = 0;
      memset(buffer, 0, sizeof(buffer));
    }

    if (showNoScene) {
      Serial.println("!! NO SCENE DETECTED (BLUE LED ON)");
      LEDState = (1 << 2);   // BLUE (Bit 2)
      HooterState = 0;
      showNoScene = 0;
    }
  }

  ProcessLEDs(LEDState);
  ProcessHooter(HooterState);
  delay(500);
}

// --- Supporting Logic ---

void ProcessLEDs(uint8_t state) {
  static uint8_t prev = 0;
  if (prev != state) {
    UpdateLEDs(state);
    prev = state;
  }
}

void UpdateLEDs(uint8_t state) {
  digitalWrite(LED_RED,   (state & (1 << 0)) ? HIGH : LOW);
  digitalWrite(LED_GREEN, (state & (1 << 1)) ? HIGH : LOW);
  digitalWrite(LED_BLUE,  (state & (1 << 2)) ? HIGH : LOW);
  digitalWrite(LED_ORANGE, (state & (1 << 3)) ? HIGH : LOW);
}

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

void EnableHooter() {
  digitalWrite(HOOTER, HIGH);
  hooter_off.attach(HOOTER_ON_PERIOD_SEC, DisableHooter);
}

void DisableHooter() {
  digitalWrite(HOOTER, LOW);
  hooter_off.detach();
}

int CheckScene() {
  int retval = CMD_ETIMEDOUT;
  int retry = 3;
  do {
    retval = AI.invoke();
  } while (retval != CMD_OK && --retry > 0);
  return retval;
}

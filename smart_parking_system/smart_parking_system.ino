/*
  Smart Parking Lot Monitoring System
  ACE6263 - Embedded IoT Systems & Applications

  Sensors (3 types):
    - HC-SR04 Ultrasonic x2  -> Slot A & B occupancy
    - LDR Module             -> Ambient light detection
    - Push Button            -> Entry trigger

  Actuators (3):
    - Servo Motor            -> Barrier gate
    - LEDs (Red/Green x2)    -> Slot status indicators
    - Buzzer + LED Strip     -> Gate feedback beep + auto lighting
*/

#include <ESP32Servo.h>

// ---------------- Pin Definitions ----------------
// Slot A - Ultrasonic + LEDs
#define TRIG_A    5
#define ECHO_A    18
#define LED_RED_A 26
#define LED_GREEN_A 32

// Slot B - Ultrasonic + LEDs
#define TRIG_B    19
#define ECHO_B    21
#define LED_RED_B 33
#define LED_GREEN_B 4

// LDR + Lighting
#define LDR_PIN   34
#define LIGHT_LED 23

// Entry Gate
#define BUTTON_PIN 13
#define SERVO_PIN  25
#define BUZZER_PIN 15

// ---------------- Constants ----------------
const float OCCUPIED_DISTANCE = 10.0;   // cm, below this = car present
const int   DARK_THRESHOLD    = 1500;   // below this = dark
const unsigned long GATE_DURATION = 5000; // gate open time (ms)

// ---------------- Globals ----------------
Servo gateServo;
long duration;
float distanceA, distanceB;
int ldrValue;

bool gateOpen = false;
unsigned long gateOpenTime = 0;

void setup() {
  Serial.begin(115200);

  // Slot A
  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(LED_RED_A, OUTPUT);
  pinMode(LED_GREEN_A, OUTPUT);

  // Slot B
  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);
  pinMode(LED_RED_B, OUTPUT);
  pinMode(LED_GREEN_B, OUTPUT);

  // LDR + Lighting
  pinMode(LIGHT_LED, OUTPUT);

  // Gate
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // start closed

  Serial.println("Smart Parking Lot System Initialized");
}

void loop() {
  checkSlotA();
  delay(50);   // avoid ultrasonic cross-interference
  checkSlotB();
  checkLight();
  checkGate();

  delay(300);
}

// ---------------- Slot A ----------------
void checkSlotA() {
  distanceA = readDistance(TRIG_A, ECHO_A);

  if (distanceA > 0 && distanceA < OCCUPIED_DISTANCE) {
    digitalWrite(LED_RED_A, HIGH);
    digitalWrite(LED_GREEN_A, LOW);
  } else {
    digitalWrite(LED_RED_A, LOW);
    digitalWrite(LED_GREEN_A, HIGH);
  }

  Serial.print("Slot A: ");
  Serial.print(distanceA);
  Serial.println(" cm");
}

// ---------------- Slot B ----------------
void checkSlotB() {
  distanceB = readDistance(TRIG_B, ECHO_B);

  if (distanceB > 0 && distanceB < OCCUPIED_DISTANCE) {
    digitalWrite(LED_RED_B, HIGH);
    digitalWrite(LED_GREEN_B, LOW);
  } else {
    digitalWrite(LED_RED_B, LOW);
    digitalWrite(LED_GREEN_B, HIGH);
  }

  Serial.print("Slot B: ");
  Serial.print(distanceB);
  Serial.println(" cm");
}

// ---------------- Ultrasonic Helper ----------------
float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long d = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  return d * 0.0343 / 2;
}

// ---------------- Ambient Lighting ----------------
void checkLight() {
  ldrValue = analogRead(LDR_PIN);

  if (ldrValue > DARK_THRESHOLD) {  // reversed: now > means dark
    digitalWrite(LIGHT_LED, HIGH);
    Serial.println("LDR: DARK -> Light ON");
  } else {
    digitalWrite(LIGHT_LED, LOW);
    Serial.println("LDR: BRIGHT -> Light OFF");
  }
}

// ---------------- Entry Gate ----------------
void checkGate() {
  if (digitalRead(BUTTON_PIN) == LOW && !gateOpen) {
    openGate();
  }

  if (gateOpen && millis() - gateOpenTime >= GATE_DURATION) {
    closeGate();
  }
}

void openGate() {
  gateServo.write(90);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  gateOpen = true;
  gateOpenTime = millis();
  Serial.println("Gate opened");
}

void closeGate() {
  gateServo.write(0);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  gateOpen = false;
  Serial.println("Gate closed");
}

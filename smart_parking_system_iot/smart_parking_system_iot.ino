// --- BLYNK CREDENTIALS ---
#define BLYNK_TEMPLATE_ID "TMPL6bWl61B5e"
#define BLYNK_TEMPLATE_NAME "Smart Parking Monitor"
#define BLYNK_AUTH_TOKEN "ijeK5feFVbB-W-9uNJE5-gsYcUGD0s19"

// ---------------- Libraries ----------------
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// ---------------- WiFi Credentials ----------------
char ssid[] = "eBfi@MMU";
char pass[] = "ebfi@mmu";

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
const float OCCUPIED_DISTANCE = 10.0;     // cm, below this = car present
const int   DARK_THRESHOLD    = 900;     // below this = dark
const unsigned long GATE_DURATION = 5000; // gate open time (ms)

// ---------------- Globals ----------------
Servo gateServo;
BlynkTimer timer; // Timer to replace delays

long duration;
float distanceA, distanceB;
int ldrValue;

bool gateOpen = false;
unsigned long gateOpenTime = 0;

// --- MANUAL FUNCTION PROTOTYPES ---
// These tell the compiler the functions exist further down to fix the scope error
void runSensors();
void checkSlotA();
void checkSlotB();
float readDistance(int trigPin, int echoPin);
void checkLight();
void checkGate();
void openGate();
void closeGate();

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

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup the timer to run sensor checks every 1000ms (1 second)
  timer.setInterval(1000L, runSensors);

  Serial.println("Smart Parking Lot System Initialized");
}

void loop() {
  Blynk.run(); // Keeps the WiFi and cloud connection alive
  timer.run(); // Checks if it is time to run the sensor routine
  checkGate(); // Runs constantly to ensure physical button is responsive
}

// ---------------- Sensor Routine ----------------
// Called every 1 second by BlynkTimer
void runSensors() {
  checkSlotA();
  delay(50);   // Short delay is okay to avoid ultrasonic ping collisions
  checkSlotB();
  checkLight();
}

// ---------------- Slot A ----------------
void checkSlotA() {
  distanceA = readDistance(TRIG_A, ECHO_A);
  int statusA = 0; 

  if (distanceA > 0 && distanceA < OCCUPIED_DISTANCE) {
    digitalWrite(LED_RED_A, HIGH);
    digitalWrite(LED_GREEN_A, LOW);
    statusA = 1; // 1 = Occupied
  } else {
    digitalWrite(LED_RED_A, LOW);
    digitalWrite(LED_GREEN_A, HIGH);
    statusA = 0; // 0 = Free
  }

  Blynk.virtualWrite(V1, statusA); // Send to Dashboard V1
  
  Serial.print("Slot A: ");
  Serial.print(distanceA);
  Serial.println(" cm");
}

// ---------------- Slot B ----------------
void checkSlotB() {
  distanceB = readDistance(TRIG_B, ECHO_B);
  int statusB = 0;

  if (distanceB > 0 && distanceB < OCCUPIED_DISTANCE) {
    digitalWrite(LED_RED_B, HIGH);
    digitalWrite(LED_GREEN_B, LOW);
    statusB = 1; 
  } else {
    digitalWrite(LED_RED_B, LOW);
    digitalWrite(LED_GREEN_B, HIGH);
    statusB = 0; 
  }

  Blynk.virtualWrite(V2, statusB); // Send to Dashboard V2

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
  int lightStatus = 0;

  if (ldrValue > DARK_THRESHOLD) {  
    digitalWrite(LIGHT_LED, HIGH);
    lightStatus = 1; // Lights are ON
    Serial.println("LDR: DARK -> Light ON");
  } else {
    digitalWrite(LIGHT_LED, LOW);
    lightStatus = 0; // Lights are OFF
    Serial.println("LDR: BRIGHT -> Light OFF");
  }

  Blynk.virtualWrite(V4, lightStatus); // Send to Dashboard V4
}

// ---------------- Entry Gate ----------------
void checkGate() {
  // Physical Button Check
  if (digitalRead(BUTTON_PIN) == LOW && !gateOpen) {
    openGate();
  }

  // Auto Close Timer
  if (gateOpen && millis() - gateOpenTime >= GATE_DURATION) {
    closeGate();
  }
}

// ---------------- Blynk Virtual Button (V3) ----------------
// This function is triggered whenever the V3 button is pressed in the app
BLYNK_WRITE(V3) {
  int buttonState = param.asInt(); // Get value from Blynk app

  if (buttonState == 1 && !gateOpen) {
    openGate();
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
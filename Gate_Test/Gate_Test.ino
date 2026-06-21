#include <ESP32Servo.h>

#define BUTTON_PIN 13
#define SERVO_PIN  25
#define BUZZER_PIN 15

Servo gateServo;

bool gateOpen = false;
unsigned long gateOpenTime = 0;
const unsigned long GATE_DURATION = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Start closed
}

void loop() {
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
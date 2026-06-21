// Test both Slot A and Slot B simultaneously
#define TRIG_A 5
#define ECHO_A 18
#define LED_RED_A   26
#define LED_GREEN_A 32

#define TRIG_B 19
#define ECHO_B 21
#define LED_RED_B   33
#define LED_GREEN_B 4

long duration;
float distanceA, distanceB;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(LED_RED_A, OUTPUT);
  pinMode(LED_GREEN_A, OUTPUT);

  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);
  pinMode(LED_RED_B, OUTPUT);
  pinMode(LED_GREEN_B, OUTPUT);
}

void loop() {
  // ---- Slot A ----
  digitalWrite(TRIG_A, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_A, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_A, LOW);
  duration = pulseIn(ECHO_A, HIGH, 30000);
  distanceA = duration * 0.0343 / 2;

  if (distanceA > 0 && distanceA < 10) {
    digitalWrite(LED_RED_A, HIGH);
    digitalWrite(LED_GREEN_A, LOW);
  } else {
    digitalWrite(LED_RED_A, LOW);
    digitalWrite(LED_GREEN_A, HIGH);
  }

  delay(50); // small gap to avoid ultrasonic cross-interference

  // ---- Slot B ----
  digitalWrite(TRIG_B, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_B, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_B, LOW);
  duration = pulseIn(ECHO_B, HIGH, 30000);
  distanceB = duration * 0.0343 / 2;

  if (distanceB > 0 && distanceB < 10) {
    digitalWrite(LED_RED_B, HIGH);
    digitalWrite(LED_GREEN_B, LOW);
  } else {
    digitalWrite(LED_RED_B, LOW);
    digitalWrite(LED_GREEN_B, HIGH);
  }

  Serial.print("Slot A: "); Serial.print(distanceA); Serial.print(" cm | ");
  Serial.print("Slot B: "); Serial.print(distanceB); Serial.println(" cm");

  delay(500);
}
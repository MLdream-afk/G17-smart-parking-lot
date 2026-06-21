// LDR + LED Test Code (auto light when dark)
#define LDR_PIN 34
#define LED_PIN 23

int threshold = 1500; // below 1500 = dark

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR Value: ");
  Serial.print(ldrValue);

  if (ldrValue < threshold) {  // dark
    digitalWrite(LED_PIN, HIGH);
    Serial.println(" -> DARK, LED ON");
  } else {  // bright
    digitalWrite(LED_PIN, LOW);
    Serial.println(" -> BRIGHT, LED OFF");
  }

  delay(500);
}
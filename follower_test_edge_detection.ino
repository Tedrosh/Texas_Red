#define IN_pin 2          // input from leader
#define LED_pin LED_BUILTIN

volatile unsigned long ticker = 0;  // must be volatile for ISR

void setup() {
  Serial.begin(115200);
  while(!Serial);

  pinMode(IN_pin, INPUT);
  pinMode(LED_pin, OUTPUT);

  // attach interrupt on rising edge
  attachInterrupt(digitalPinToInterrupt(IN_pin), handlePulse, RISING);
}

void loop() {
  // main loop can do other things
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) { // print every 200 ms
    lastPrint = millis();
    Serial.print("Pulse count: ");
    Serial.println(ticker);
  }
}

// ISR: increments ticker and toggles LED
void handlePulse() {
  ticker++;                     // increment pulse count
  digitalWrite(LED_pin, !digitalRead(LED_pin)); // toggle LED
}
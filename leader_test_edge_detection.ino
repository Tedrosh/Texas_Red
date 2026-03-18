// #define OUT_pin 3   // output to follower

#define ENABLE_OUT_TO_DRUM_FOLLOWER_pin 4
#define UPDATE_OUT_TO_DRUM_FOLLOWER_pin 2

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  pinMode(ENABLE_OUT_TO_DRUM_FOLLOWER_pin, OUTPUT);
  pinMode(UPDATE_OUT_TO_DRUM_FOLLOWER_pin, OUTPUT);
  digitalWrite(ENABLE_OUT_TO_DRUM_FOLLOWER_pin, HIGH); 
  digitalWrite(UPDATE_OUT_TO_DRUM_FOLLOWER_pin, LOW);  // ensure low at start
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();      // read the typed character
    if (c != '\n') {
          // send a short pulse to the follower
      digitalWrite(UPDATE_OUT_TO_DRUM_FOLLOWER_pin, HIGH);
      delay(1);                   // 50 ms pulse
      digitalWrite(UPDATE_OUT_TO_DRUM_FOLLOWER_pin, LOW);
      
      Serial.print("Sent pulse for character: ");
      Serial.println(c);
    }

  }
}
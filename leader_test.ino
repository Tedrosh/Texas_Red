#define IN_pin 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);

  pinMode(IN_pin, INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(IN_pin));
}

#define OUT_pin 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);

  pinMode(OUT_pin, OUTPUT);
}

int i = 0;
bool out = 0;

void loop() {
  // put your main code here, to run repeatedly:
  i=10000;
  while(i) i--;
  out = !out;
  digitalWrite(OUT_pin, out);
}

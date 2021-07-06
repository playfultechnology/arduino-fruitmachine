
const byte pinCountOutput = 8; 

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(pinCountOutput, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(pinCountOutput));
  delay(20);
}

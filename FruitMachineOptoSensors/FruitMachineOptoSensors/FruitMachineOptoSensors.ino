// CONSTANTS
const byte sensorPin = 2;
const byte ledPin = LED_BUILTIN;

bool previousReading = false;
int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int isThereSomethingInTheWay = digitalRead(sensorPin);
  
  // put your main code here, to run repeatedly:
  //Serial.println(isThereSomethingInTheWay);
  digitalWrite(ledPin, isThereSomethingInTheWay);

  if(!previousReading && isThereSomethingInTheWay) {
    count++;
    Serial.print(F("Count: "));
    Serial.println(count);
  }

  previousReading = isThereSomethingInTheWay;
  
}

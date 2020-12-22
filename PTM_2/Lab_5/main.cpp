int licznik = 0, licznikDlaUART = 0;
String odebraneDane = "";

void dioda1() {
  if (digitalRead(8) == LOW) {
    digitalWrite(8, HIGH);
    Serial.println("Dioda 1 włączona");
  } else {
    digitalWrite(8, LOW);
    Serial.println("Dioda 1 wyłączona");
  }
}

void dioda2() {
  if (digitalRead(9) == LOW) {
    digitalWrite(9, HIGH);
    Serial.println("Dioda 2 włączona");
  } else {
    digitalWrite(9, LOW);
    Serial.println("Dioda 2 wyłączona");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), dioda1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), dioda2, RISING);

  Serial.println("Hello world - 248926");
}

void loop() {
  if (Serial.available() > 0) {
    odebraneDane = Serial.readStringUntil('\n');
    Serial.println("Odebralem: " + String(odebraneDane[0]) + "-" + String((int) odebraneDane[0]));
    licznikDlaUART = 0;
  } else
    licznikDlaUART++;
    
  if (licznikDlaUART > 9) {
    Serial.println("Czekam na znak, pospiesz sie…");
    licznikDlaUART = 0;
  }

  licznik++;
  Serial.println("Dziala: " + String(licznik));
  delay(1000);
}

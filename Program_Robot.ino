#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>

SoftwareSerial Bluetooth(3, 2); // RX, TX 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // 16-kanálové PWM-Servo Shield

#define SERVOMIN  150 // Minimální hodnota PWM pro servo
#define SERVOMAX  600 // Maximální hodnota PWM pro servo

// Definice kanálů pro serva
int servo1 = 6;  // zápěstí →
int servo2 = 5;  // zápěstí ↑
int servo3 = 10; // rameno
int servo4 = 4;  // základna
int servo5 = 7;  // loket
int servo7 = 8;  // CHYTAČ

// Nastavení rychlostí serv
int s1Vel = 25; // zápěstí →
int s2Vel = 25; // zápěstí ↑
int s3Vel = 25; // rameno
int s4Vel = 25; // základna
int s5Vel = 25; // loket
int s7Vel = 25; // CHYTAČ

// Aktuální a předchozí pozice serv
int s1Act, s2Act, s3Act, s4Act, s5Act, s7Act;
int s1Ant, s2Ant, s3Ant, s4Ant, s5Ant, s7Ant;
int s1[50], s2[50], s3[50], s4[50], s5[50], s7[50]; // Pole pro ukládání pozic

// Proměnné pro nahrávání a přehrávání
bool jeNahravani = false;
int ulozenyIndex = 0; // Pro sledování počtu uložených kroků

String bt, btS;

// Deklarace funkcí před použitím
void nastavUhelServa(int servoNum, int uhel);
void pohybujServemPostupne(int servoNum, int start, int konec, int casovaSmycka);
void ulozenipozice();

void setup() {
  Serial.begin(115200);
  Bluetooth.begin(9600);
  Bluetooth.setTimeout(10);

  pwm.begin();
  pwm.setPWMFreq(60); // Nastavení frekvence PWM na 60 Hz

  delay(10);

  // Inicializace výchozích pozic serv
  s1Ant = 90;  // zápěstí →
  nastavUhelServa(servo1, s1Ant);

  s2Ant = 90; // zápěstí ↑
  nastavUhelServa(servo2, s2Ant);

  s3Ant = 90;  // rameno
  nastavUhelServa(servo3, s3Ant);
  s4Ant = 90;  // základna
  nastavUhelServa(servo4, s4Ant);

  s5Ant = 90;  // loket
  nastavUhelServa(servo5, s5Ant);

  s7Ant = 110;  // CHYTAČ
  nastavUhelServa(servo7, s7Ant);

  delay(50);
}

void loop() {
  if (Bluetooth.available() > 0) {
    bt = Bluetooth.readString();

    //////////////////////////// Nahrávání a přehrávání ////////////////////////////
    if (bt.startsWith("rec")) {
      jeNahravani = true;
      ulozenyIndex = 0;
      Serial.println("Zahájeno nahrávání pohybů");
    }

    if (bt.startsWith("stop")) {
      jeNahravani = false;
      Serial.println("Ukončeno nahrávání pohybů");
    }

    if (bt.startsWith("play")) {
      Serial.println("Přehrávání uložené sekvence");
      for (int i = 0; i < ulozenyIndex; i++) {
        pohybujServemPostupne(servo1, s1Ant, s1[i], s1Vel);
        pohybujServemPostupne(servo2, s2Ant, s2[i], s2Vel);
        pohybujServemPostupne(servo3, s3Ant, s3[i], s3Vel);
        pohybujServemPostupne(servo4, s4Ant, s4[i], s4Vel);
        pohybujServemPostupne(servo5, s5Ant, s5[i], s5Vel);
        pohybujServemPostupne(servo7, s7Ant, s7[i], s7Vel);

        // Aktualizace aktuálních pozic pro další krok
        s1Ant = s1[i];
        s2Ant = s2[i];
        s3Ant = s3[i];
        s4Ant = s4[i];
        s5Ant = s5[i];
        s7Ant = s7[i];
      }
    }

    //////////////////////////// Ovládání jednotlivých serv /////////////////////////
    if (bt.startsWith("s4")) { //zakladna
      btS = bt.substring(2);
      s4Act = btS.toInt();
      pohybujServemPostupne(servo4, s4Ant, s4Act, s4Vel);
      s4Ant = s4Act;
      ulozenipozice();
    }

    if (bt.startsWith("s3")) { //rameno
      btS = bt.substring(2);
      s3Act = btS.toInt();
      pohybujServemPostupne(servo3, s3Ant, s3Act, s3Vel);
      s3Ant = s3Act;
      ulozenipozice();
    }

    if (bt.startsWith("s5")) { // loket
      btS = bt.substring(2);
      s5Act = btS.toInt();
      pohybujServemPostupne(servo5, s5Ant, s5Act, s5Vel);
      s5Ant = s5Act;
      ulozenipozice();
    }

    if (bt.startsWith("s1")) { // zápěstí →
      btS = bt.substring(2);
      s1Act = btS.toInt();
      pohybujServemPostupne(servo1, s1Ant, s1Act, s1Vel);
      s1Ant = s1Act;
      ulozenipozice();
    }

    if (bt.startsWith("s2")) { // zápěstí ↑
      btS = bt.substring(2);
      s2Act = btS.toInt();
      pohybujServemPostupne(servo2, s2Ant, s2Act, s2Vel);
      s2Ant = s2Act;
      ulozenipozice();
    }

    if (bt.startsWith("s7")) { // CHYTAČ
      btS = bt.substring(2);
      s7Act = btS.toInt();
      pohybujServemPostupne(servo7, s7Ant, s7Act, s7Vel);
      s7Ant = s7Act;
      ulozenipozice();
    }
  }
}

// Funkce pro nastavení úhlu serva
void nastavUhelServa(int servoNum, int uhel) {
  // Při použití rozsahu 0-180 stupňů pro všechny serva
  int pulse = map(uhel, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servoNum, 0, pulse);
}

// Funkce pro postupný pohyb serva
void pohybujServemPostupne(int servoNum, int start, int konec, int casovaSmycka) {
  if (start > konec) {
    for (int i = start; i >= konec; i--) {
      nastavUhelServa(servoNum, i);
      delay(casovaSmycka);
    }
  } else {
    for (int i = start; i <= konec; i++) {
      nastavUhelServa(servoNum, i);
      delay(casovaSmycka);
    }
  }
}

// Funkce pro uložení pozice
void ulozenipozice() {
  if (jeNahravani && ulozenyIndex < 50) {
    s1[ulozenyIndex] = s1Ant;
    s2[ulozenyIndex] = s2Ant;
    s3[ulozenyIndex] = s3Ant;
    s4[ulozenyIndex] = s4Ant;
    s5[ulozenyIndex] = s5Ant;
    s7[ulozenyIndex] = s7Ant;
    ulozenyIndex++;
  }
}

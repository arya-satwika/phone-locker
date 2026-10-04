#include "HX711.h"
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

HX711 scale;

Servo lockServo;

// pins
const int scaleData = 7;
const int scaleClock = 6;

const int sonicTrig = 2;
const int sonicEcho = 3;

const int buttonPin= 8;

const int pwmServo = 4;

const int buzzerPin = 5;

bool initialized;

int oldButtonVal= LOW;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup reached");
  // put your setup code here, to run once:
  pinMode(sonicEcho, INPUT);
  pinMode(sonicTrig, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  scale.begin(scaleData, scaleClock);

  scale.set_scale(420.0);

  initialized = true;
  Serial.println("HX711 initialized and zeroed.");

  lockServo.attach(pwmServo);
  lockServo.write(0);
  Serial.println("LmaoXD");
}

float getDistance(){
    digitalWrite(sonicTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(sonicTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(sonicTrig, LOW);

    // 2. Membaca Durasi Pantulan (µs)
    long durasi = pulseIn(sonicEcho, HIGH);

    // 3. Menghitung Jarak (cm)
    float jarak = (durasi * 0.0343) / 2;
    return jarak; 
}

float getWeight(){
  float weight;
  if (scale.wait_ready_timeout(1000)) {
    weight = scale.get_units(5);
    Serial.println(weight, 2);
  } else {
    Serial.println("HX711 not ready. Check connections.");
  }
  return weight;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!initialized) {
    delay(1000);
    return;
  }

  int newButtonVal = digitalRead(buttonPin);

  if (newButtonVal != oldButtonVal)
  {
    // run when button pressed
    scale.tare(); 
  }
  

  
  // tare phone weight
  // cek person using getDistance -- if distance < 30cm servo unlocks -> starts reading weight
  float distance = getDistance();
  float weight = getWeight();
  if (distance < 30){ // person detected
    lockServo.write(90);
    if (weight < 0){ // phone is not picked up
      // if weight is negative for more than 120s activate buzzer
      long startTime = millis();
      if (startTime > 120000) { tone(buzzerPin, 1500);}
    }
  } else if (distance > 30){ // no person detected
    lockServo.write(0);
    // if weight is negative and distance > 30cm activate buzzer
    if (weight < 0){ // phone is picked up
      tone(buzzerPin, 1500);
    } else return;
  }
  
  // delay(100);
}

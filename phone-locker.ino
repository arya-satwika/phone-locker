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

float WEIGHT, DISTANCE;

long startLiftTime = 0;
bool startedLifting = false;


void setup() {
  Serial.begin(115200);
  Serial.println("Setup reached");
  // put your setup code here, to run once:
  pinMode(sonicEcho, INPUT);
  pinMode(sonicTrig, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  scale.begin(scaleData, scaleClock);
  scale.set_scale(420.0);

  lockServo.attach(pwmServo);
  lockServo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Arya Satwika");
  lcd.setCursor(0, 1);
  lcd.print("24051204069");
  delay(1000);
  lcd.clear();
}

void getDistance(){
    digitalWrite(sonicTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(sonicTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(sonicTrig, LOW);

    // 3. Menghitung Jarak (cm)
    DISTANCE = (pulseIn(sonicEcho, HIGH) * 0.0343) / 2;
}

void getWeight(){
    if (scale.is_ready()){
      WEIGHT = scale.get_units(5);
      // Serial.println(WEIGHT, 2);
    } 
}

void loop() {

  int buttonState = digitalRead(buttonPin);

  // tare phone weight
  if (buttonState == HIGH){
    // run when button pressed
    Serial.println("Button Clicked");
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Zero-ing scale");
    scale.tare(); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Zero-ed scale");
    Serial.println("Weight Tared");
    lcd.clear();
    return;
  }

  // cek person using getDistance -- if distance < 30cm servo unlocks -> starts reading weight
  getDistance();
  // lcd.setCursor(0,0);
  // lcd.print(DISTANCE);
  
  getWeight();
  // lcd.setCursor(0,1);
  // lcd.print(WEIGHT,2);

  // bool phoneDetected = fabsf(WEIGHT) <= weightTolerance;

  float weightTolerance = 0.3; // weight can fluctuate depending on placement
  bool phoneDetected =  fabsf(WEIGHT) <= weightTolerance; //absolute weight less than tolerance
  bool tooHeavy = WEIGHT > weightTolerance;
  bool phoneLifted = WEIGHT < -weightTolerance;
  bool personDetected = DISTANCE <= 30;
  if (phoneDetected && !tooHeavy) startedLifting = false;  //phone put down
  bool liftTimeout = startedLifting && millis() - startLiftTime >= 120000;


  bool soundAlarm = 
        tooHeavy ||
        (phoneLifted && !personDetected) ||
        liftTimeout;

  Serial.println(millis());
  Serial.print("start lift time: ");
  Serial.println(startLiftTime);
  lcd.setCursor(0,1);
  lcd.print(phoneDetected ? "Phone Detected  " : (tooHeavy ? "Too Heavy       " : "Phone Lifted    "));

  lcd.setCursor(0,0);
  lcd.print(personDetected ? "Unlocked" : "Locked  ");


  if(soundAlarm){
    tone(buzzerPin,1500);

  } else{ 
    noTone(buzzerPin); 
  }
  
  if (personDetected){ 
    lockServo.write(90); 
    if (phoneLifted){
      if (!startedLifting) { startLiftTime = millis(); startedLifting = true; }
    }
    
  } 
  else { 
    lockServo.write(0);
  }
}

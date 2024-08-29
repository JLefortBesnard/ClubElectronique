
// include the Servo library
#include <Servo.h>
#include "Ultrasonic.h"
Ultrasonic ultrasonic(7, 8); // Trig et Echo
Servo bras_servo;

#define speakerPin 4
#define redPin 2
#define greenPin 3

int dist = 20;

void setup() {
  bras_servo.attach(5);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(redPin, HIGH);  
  digitalWrite(greenPin, LOW);
  ultrasonic.read(CM);
}

void loop() {
  dist = ultrasonic.read(CM);
  if (dist < 20) {
    digitalWrite(redPin, HIGH);  
    digitalWrite(greenPin, LOW);
    tone(speakerPin, 330); 
    delay(1000);
    noTone(speakerPin);
    digitalWrite(redPin, LOW);
    delay(150);
    bras_servo.write(160);
    delay(300);
    bras_servo.write(90);
    delay(1000);
    bras_servo.write(0);
  } else {
    noTone(speakerPin);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    }; 
  delay(1000);    // needs time for capteur ultrason    
}

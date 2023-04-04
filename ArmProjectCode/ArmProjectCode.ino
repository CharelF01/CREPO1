/*
Feil Charel
BTS-IOT1
CREPO - Robot arm
*/
#include <Servo.h>

//defines all the pins for the stepper motor driver
const int dirPin = 2;
const int stepPin = 3;
const int MS1 = 4;
const int MS2 = 5;
const int MS3 = 6;

//defines the directions
const bool dirLeft = HIGH;
const bool dirRight = LOW;

//saves the total steps it can move from one side to the other
int stepsLeftRight = 0;
//saves the position of the stepper
int stepperPos;

bool pushed = LOW;  //to indicate what state the button is when pushed
const int LEFT = 12;  //left button pin
const int RIGHT = 11; //right button pin

bool zeroed = false; //stores if the arm has been zeroed

//defines the maximum angles the hand can move
int handMin = 43;
int handMax = 160;

//initialize the servo motors
Servo baseServo;  //up is ++ down is --
Servo interServo;  //up is -- down is ++
Servo handServo; //up is ++ down is --
Servo grabServo;  //open is ++ close is --

//set the servo motors pins
int baseServoPin = 7;
int interServoPin = 8;
int handServoPin = 9;
int grabServoPin = 10;

//save the position of every servo
int base = 0;
int inter = 0;
int hand = 0;
int grab = 0;

/*void grabCube() {

  for (int i = hand; i >= handMin; i--) {
    handServo.write(i);
    delay(speed);
  }
  hand = handMin;
  for (int i = inter; i <= 30; i++) {
    interServo.write(i);
    delay(speed);
  }
  inter = 30;
  for (int i = base; i >= 35; i--) {
    baseServo.write(i);
    delay(speed);
  }
  base = 35;
  for (int i = grab; i >= 50; i--) {
    grabServo.write(i);
    delay(speed);
  }
  grab = 50;

  for (int i = base; i <= 90; i++) {
    baseServo.write(i);
    delay(speed);
  }
  base = 90;

  for (int i = inter; i >= 0; i--) {
    interServo.write(i);
    delay(speed);
  }
  inter = 50;

  for (int i = hand; i <= 130; i++) {
    handServo.write(i);
    delay(speed);
  }
  hand = 130;
  }*/

//grabs the cube from the marked position
void grabCube2(int speed) {

  for (int i = base; i <= 120; i++) {
    baseServo.write(i);
    delay(speed);
  }
  base = 120;
  for (int i = hand; i >= handMin; i--) {
    handServo.write(i);
    delay(speed);
  }
  hand = handMin;
  for (int i = inter; i <= 130; i++) {
    interServo.write(i);
    delay(speed);
  }
  inter = 130;

  for (int i = grab; i >= 50; i--) {
    grabServo.write(i);
    delay(speed);
  }
  grab = 50;

  servoZeroSpeed(speed);
}

//puts the cube down safely
void letGo(int speed) {
  for (int i = hand; i >= handMin; i--) {
    handServo.write(i);
    delay(speed);
  }
  hand = handMin;
  for (int i = inter; i <= 30; i++) {
    interServo.write(i);
    delay(speed);
  }
  inter = 30;
  for (int i = base; i >= 35; i--) {
    baseServo.write(i);
    delay(speed);
  }
  base = 35;
  for (int i = grab; i <= 150; i++) {
    grabServo.write(i);
    delay(speed);
  }
  grab = 150;

  servoZeroSpeed(speed);
}


void setup()
{
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  //declare buttons as internal pullup
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);

  //attach the servos to their according pins
  baseServo.attach(baseServoPin);
  grabServo.attach(grabServoPin);
  handServo.attach(handServoPin);
  interServo.attach(interServoPin);

  //go to the servos zero position without knowing their previous position
  servoZero(true);

  //set the accuracy to maximum: 1/16 steps
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);

  //begin serial monitor for testing
  Serial.begin(9600);
}

/*goes to the zero position of every servo with or without the grabber 
and not knowing their previous position*/
void servoZero(bool grabOpen) {
  baseServo.write(90);
  base = 90;
  delay(100);
  handServo.write(125);
  hand = 125;
  delay(100);
  interServo.write(0);
  inter = 0;
  delay(100);
  if (grabOpen) {
    grabServo.write(150);
    grab = 150;
  }
}

/*goes to the zero position of every servo except the grabber 
and knowing their previous position*/
void servoZeroSpeed(int speed) {
  for (int i = base; i <= 90; i++) {
    baseServo.write(i);
    delay(speed);
  }
  base = 90;

  for (int i = inter; i >= 0; i--) {
    interServo.write(i);
    delay(speed);
  }
  inter = 0;

  for (int i = hand; i <= 130; i++) {
    handServo.write(i);
    delay(speed);
  }
  hand = 130;
}

//finds the zero position of the stepper motor
void findZero() {
  digitalWrite(dirPin, dirLeft);
  //turns left until the button is pressed
  while (digitalRead(LEFT) != pushed) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
  }
  delay(100);
  //turns right until the other button is pressed and counts the steps 
  digitalWrite(dirPin, dirRight);
  while (digitalRead(RIGHT) != pushed) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
    stepsLeftRight += 1;
  }
  delay(100);
  //turns to the middle
  digitalWrite(dirPin, dirLeft);
  for (int i = 0; i <= stepsLeftRight / 2; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
  }
  stepperPos = 0;
  zeroed = true;
}


void loop()
{
  //check if it's in the 0 position, if not, go there
  if (!zeroed) {
    goToZero();
    delay(500);
  }
  else {
    //check if both buttons are pushed, then execute the grabbing code
    if ((digitalRead(LEFT) == pushed) && (digitalRead(RIGHT) == pushed)) {
      digitalWrite(dirPin, dirLeft);
      for (int i = 0; i <= stepsLeftRight / 4; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(400);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(400);
        stepperPos--;
      }

      grabCube2(15);
      
      digitalWrite(dirPin, dirRight);
      for (int i = 0; i <= stepsLeftRight / 2; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(400);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(400);
        stepperPos++;
      }
      
      letGo(15);

      //for later iterations if the arm is turned otherwise, it will always go to 0
      if (stepperPos < 0) {
        digitalWrite(dirPin, dirRight);
        while (stepperPos != 0) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(400);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(400);
          stepperPos--;
        }
      }
      else if (stepperPos < 0) {
        digitalWrite(dirPin, dirLeft);
        while (stepperPos != 0) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(400);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(400);
          stepperPos++;
        }
      }
    }
  }
}

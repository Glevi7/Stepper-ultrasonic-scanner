#include <Stepper.h>
#include <IRremote.h>

//stepper
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 2, 4, 3, 5);


//ultrasonic 
const int trigPin = 9;
const int echoPin = 10;

//start button
const int buttonPin = 8;
bool lastButtonState = HIGH;

//ir sensor
#define IR_PIN 11

// radar states
enum RadarState {
  RADAR_OFF,
  RADAR_CALIBRATION,
  RADAR_AUTO,
  RADAR_MANUAL
};

RadarState radarState = RADAR_OFF;

// angle control
int angle = 90;        
int targetAngle = 90;

int calAngle = 0;      

int stepAngle = 2;
bool clockwise = true;
bool calibrated = false;

long duration;

//setup
void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  myStepper.setSpeed(10);
  IrReceiver.begin(IR_PIN);

  Serial.println("Radar Ready");
}

//ultrasonic
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

//calibration
void moveRelative(int deltaDeg) {
  long steps = (long)deltaDeg * stepsPerRevolution / 360;
  myStepper.step(steps);
  calAngle += deltaDeg;
}

//auto
void moveToTarget() {
  targetAngle = constrain(targetAngle, 0, 180);

  int delta = targetAngle - angle;
  if (delta == 0) return;

  long steps = (long)delta * stepsPerRevolution / 360;
  myStepper.step(steps);
  angle = targetAngle;
}

//start
void checkButton() {
  bool current = digitalRead(buttonPin);

  if (lastButtonState == HIGH && current == LOW) {

    if (radarState == RADAR_OFF) {
      
      radarState = RADAR_CALIBRATION;
      calibrated = false;
      calAngle = 0;
      Serial.println("CALIBRATION MODE");
    }
    else {
      
      radarState = RADAR_OFF;
      calibrated = false;
      Serial.println("RADAR OFF");
    }

    delay(200);
  }

  lastButtonState = current;
}

//IR
void checkIR() {
  if (!IrReceiver.decode()) return;
  auto &d = IrReceiver.decodedIRData;

  //calibration
  if (radarState == RADAR_CALIBRATION) {

    if (d.command == 0x09) moveRelative(+5);  // UP
    if (d.command == 0x07) moveRelative(-5);  // DOWN

    
    if (d.command == 0x43) { // >>|
      angle = 90;
      targetAngle = 90;
      clockwise = true;
      calibrated = true;
      radarState = RADAR_AUTO;
      Serial.println("AUTO MODE (CALIBRATED)");
    }
  }

  //toggle manual
  else if (calibrated && (radarState == RADAR_AUTO || radarState == RADAR_MANUAL)) {

    if (d.command == 0xD) { // FUNC/STOP
      radarState = (radarState == RADAR_AUTO) ? RADAR_MANUAL : RADAR_AUTO;
      Serial.println(radarState == RADAR_MANUAL ? "MANUAL MODE" : "AUTO MODE");
    }
  }

  //manual control
  if (radarState == RADAR_MANUAL) {

    if (d.command == 0x09) targetAngle += 5;
    if (d.command == 0x07) targetAngle -= 5;

    switch (d.command) {
      case 0x16: targetAngle = 0;   break;
      case 0x0C: targetAngle = 20;  break;
      case 0x18: targetAngle = 40;  break;
      case 0x5E: targetAngle = 60;  break;
      case 0x08: targetAngle = 80;  break;
      case 0x1C: targetAngle = 100; break;
      case 0x5A: targetAngle = 120; break;
      case 0x42: targetAngle = 140; break;
      case 0x52: targetAngle = 160; break;
      case 0x4A: targetAngle = 180; break;
    }

    moveToTarget();
  }

  IrReceiver.resume();
}

//main
void loop() {
  checkButton();
  checkIR();

  if (radarState == RADAR_OFF) return;

  int distance = getDistance();

  Serial.print(angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.println(radarState == RADAR_MANUAL ? 1 : 0);

  // sweep
  if (radarState == RADAR_AUTO && calibrated) {
    int delta = clockwise ? stepAngle : -stepAngle;

    targetAngle = angle + delta;
    moveToTarget();

    if (angle >= 180) clockwise = false;
    if (angle <= 0)   clockwise = true;
  }
}
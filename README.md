# Stepper ultrasonic scanner
Arduino "radar" system that uses a stepper motor, ultrasonic distance sensor, and an IR remote to sweep an 180° field of view and visualize detected objects in real time using a Processing GUI. This system features a manual calibration workflow, hard angle limits, and multiple operating modes.

## instructions
see included video 

## Hardware required
Arduino Uno R3
28BYJ-48 stepper motor with driver
HC-SRO4 Ultrasonic Sensor 
IR reciever module
IR remote
Momentary push button 

##pin assignment
-Stepper motor uses 2,3,4,5
-Ultrasonic sensor uses trig 9, echo 10
-IR reciever uses 11
-button uses 8

all modules are powered from 5v and GND

## extra
I've included the ultrasonic mount that I modified from the following model on GRABCAD : https://grabcad.com/library/hc-sr04-holder-2
I've also included the model I created to hold the stepper motor 


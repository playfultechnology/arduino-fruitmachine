// ProportionalControl.pde
// -*- mode: C++ -*-
//
// Make a single stepper follow the analog value read from a pot or whatever
// The stepper will move at a constant speed to each newly set posiiton, 
// depending on the value of the pot.
//
// Copyright (C) 2012 Mike McCauley
// $Id: ProportionalControl.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

#define STEPS_PER_VALUE 4

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, 11, 10, 9, 8); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

// This defines the analog input pin for reading the control voltage
// Tested with a 10k linear pot between 5v and GND
#define ANALOG_IN A5

int last_analogRead;
int location;

void setup()
{  
  stepper.setMaxSpeed(100);
  Serial.begin(9600);
}

void loop()
{

  // Read new position
  int analog_in = analogRead(ANALOG_IN);
    Serial.println(analog_in);
  if(abs(analog_in - last_analogRead) > 10) {
    last_analogRead = analog_in;
    location = map(analog_in, 0, 1023, 0, 12) * STEPS_PER_VALUE;
    stepper.moveTo(location);
  }  
  stepper.setSpeed(40);
  stepper.runSpeedToPosition();
}

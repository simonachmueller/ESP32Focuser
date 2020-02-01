/*
StepperControl_a4988.h - - Driver for a Stepper Motor with controler a4988 - Version 1.0

History:
Version 1.0 - Author Jean-Philippe Bonnet
   First release

This librarie allow the control of a stepper motor controled with a A4988 Stepper motor controler. This libraie allow absolut positioning and can be used for horizontal movement.

If the brake mode is enabled the the motor will be premenantly powered. This increase the power consomption but the motor will be fix. If the brake mode is disabled, the motor is able to run freely if it not in move. If the motor turn the counter which indicate the steps will be wrong.

This file is part of the StepperControl_A4988 library.

StepperControl_A4988 library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

StepperControl_4988 library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with StepperControl library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef stepperControl_A4988_h
#define stepperControl_A4988_h

#if ARDUINO < 100
#include <Wprogram.h>
#else
#include <Arduino.h>
#endif

#define SC_CLOCKWISE 0
#define SC_COUNTER_CLOCKWISE 1

// TMC2209 constants:
#define SC_8TH_STEP 0
#define SC_16TH_STEP 2
#define SC_32TH_STEP 1
#define SC_64TH_STEP 3

#define SC_MAX_SPEED 1000000
#define SC_MAX_SPEED_8TH_STEP 3000
#define SC_MAX_SPEED_16TH_STEP 5000
#define SC_MAX_SPEED_32TH_STEP 10000
#define SC_MAX_SPEED_64TH_STEP 20000

#define SC_MOVEMODE_PER_STEP 0
#define SC_MOVEMODE_SMOOTH 1

#define SC_DEFAULT_ACCEL 100

#define SC_DEFAULT_SPEED 1000

class StepperControl
{
 public:
  //DEBUG
  long dbg_correction;
  // Constructors:
  StepperControl(int stepPin,
			int directionPin,
			int stepModePin1,
			int stepModePin2,
			int stepModePin3,
			int enablePin,
			int sleepPin,
			int resetPin);

  // Setters
  void setTargetPosition(long position);
  void setCurrentPosition(long position);
  void setDirection(int direction);
  void setStepMode(int stepMode);
  void setMoveMode(int moveMode);
  void setSpeed(unsigned int speed);
  void setTemperatureCompensationCoefficient(int coef);
  void setCurrentTemperature(float temperature);

  // Getters
  long getCurrentPosition();
  long getTargetPosition();
  int getDirection();
  int getStepMode();
  int getMoveMode();
  unsigned int getSpeed();
  int getTemperatureCompensationCoefficient();

  // Other public members
  void Manage();
  void goToTargetPosition();
  void stopMovement();
  int isInMove();
  void compensateTemperature();  
  bool isTemperatureCompensationEnabled();
  void enableTemperatureCompensation();
  void disableTemperatureCompensation();

 private:
  int direction;
  int stepMode;
  int moveMode;
  int inMove;
  int brakeMode;
  unsigned int acceleration;
  long startPosition; 
  long currentPosition;
  long targetPosition;
  unsigned int speed;  // Speed in ticks per seconds
  bool targetSpeedReached;
  unsigned int targetSpeed;
  long positionTargetSpeedReached;
  bool temperatureCompensationIsEnabled;
  int temperatureCompensationCoefficient;
  float lastCompensatedTemperature;
  bool temperatureCompensationIsInit;
  float currentTemperature;

  unsigned long lastMovementTimestamp;
  unsigned long accelTimestamp;

  int stepPin;
  int directionPin;
  int stepModePin1;
  int stepModePin2;
  int stepModePin3;
  int enablePin;
  int sleepPin;
  int resetPin;

  void moveMotor();
  void calculateSpeed();
};

#endif //stepperControl_A4988_h

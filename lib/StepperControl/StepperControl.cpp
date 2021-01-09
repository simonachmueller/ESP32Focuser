/*
StepperControl_A4988.h - - Driver for a Stepper Motor with controler A4988- Version 1.0

History:
Version 1.0 - Author Jean-Philippe Bonnet
   First release

This file is part of the StepperControl_A4988 library.

StepperControl_A4988 library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

StepperControl_A4988 library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with StepperControl library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "StepperControl.h"

//------------------------------------------------------------------------------------
// Constructors:
StepperControl::StepperControl(int stepPin,
                                           int directionPin,
                                           int stepModePin1,
                                           int stepModePin2,
                                           int stepModePin3,
                                           int enablePin,
                                           int sleepPin,
                                           int resetPin)
{
  this->stepPin = stepPin;
  this->directionPin = directionPin;
  this->stepModePin1 = stepModePin1;
  this->stepModePin2 = stepModePin2;
  this->stepModePin3 = stepModePin3;
  this->enablePin = enablePin;
  this->sleepPin = sleepPin;
  this->resetPin = resetPin;

  pinMode(stepPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepModePin1, OUTPUT);
  pinMode(stepModePin2, OUTPUT);
  pinMode(stepModePin3, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  digitalWrite(directionPin, LOW);
  digitalWrite(stepPin, LOW);

  digitalWrite(sleepPin, HIGH);
  digitalWrite(resetPin, HIGH);

  digitalWrite(enablePin, HIGH);

  this->direction = SC_CLOCKWISE;
  this->inMove = false;
  this->startPosition = 0;
  this->currentPosition = 0;
  this->targetPosition = 0;
  this->moveMode = SC_MOVEMODE_PER_STEP;
  this->acceleration = SC_DEFAULT_ACCEL;
  this->speed = 0;
  this->lastMovementTimestamp = 0;
  this->accelTimestamp = 0;
  this->targetSpeedReached = false;
  this->positionTargetSpeedReached = 0;
  this->lastCompensatedTemperature = 0;
  this->temperatureCompensationIsInit = false;
  this->temperatureCompensationIsEnabled = false;
  this->temperatureCompensationCoefficient = 0;
  this->currentTemperature = 0;
  this->setStepMode(SC_8TH_STEP);
}

//------------------------------------------------------------------------------------
// Setters
void StepperControl::setTargetPosition(long position)
{
  this->targetPosition = position;
}

void StepperControl::setCurrentPosition(long position)
{
  this->currentPosition = position;
}

void StepperControl::setDirection(int direction)
{
  this->direction = direction;
}

void StepperControl::setStepMode(int stepMode)
{
  this->stepMode = stepMode;
  digitalWrite(stepModePin1, this->stepMode & 0x04);
  digitalWrite(stepModePin2, this->stepMode & 0x02);
  digitalWrite(stepModePin3, this->stepMode & 0x01);
}

void StepperControl::setMoveMode(int moveMode)
{
  this->moveMode = moveMode;
}

void StepperControl::setSpeed(unsigned int speed)
{
  if (this->stepMode == SC_8TH_STEP && speed >= SC_MAX_SPEED_8TH_STEP)
  {
    this->targetSpeed = SC_MAX_SPEED_8TH_STEP;
  }
  else if (this->stepMode == SC_16TH_STEP && speed >= SC_MAX_SPEED_16TH_STEP)
  {
    this->targetSpeed = SC_MAX_SPEED_16TH_STEP;
  }
  else if (this->stepMode == SC_32TH_STEP && speed >= SC_MAX_SPEED_32TH_STEP)
  {
    this->targetSpeed = SC_MAX_SPEED_32TH_STEP;
  }
  else if (this->stepMode == SC_64TH_STEP && speed >= SC_MAX_SPEED_64TH_STEP)
  {
    this->targetSpeed = SC_MAX_SPEED_64TH_STEP;
  }
  else
  {
    this->targetSpeed = speed;
  }
}

void StepperControl::setTemperatureCompensationCoefficient(int coef)
{
  this->temperatureCompensationCoefficient = coef;
}

void StepperControl::setCurrentTemperature(float curTemp)
{
  this->currentTemperature = curTemp;
  if (!this->temperatureCompensationIsInit)
  {
    this->lastCompensatedTemperature = this->currentTemperature;
    this->temperatureCompensationIsInit = true;
  }
}

//------------------------------------------------------------------------------------
// Getters
long StepperControl::getCurrentPosition()
{
  return this->currentPosition;
}

long StepperControl::getTargetPosition()
{
  return this->targetPosition;
}

int StepperControl::getDirection()
{
  return this->direction;
}

int StepperControl::getStepMode()
{
  return this->stepMode;
}

int StepperControl::getMoveMode()
{
  return this->moveMode;
}

unsigned int StepperControl::getSpeed()
{
  return this->speed;
}

int StepperControl::getTemperatureCompensationCoefficient()
{
  return this->temperatureCompensationCoefficient;
}

//------------------------------------------------------------------------------------
// Other public members
void StepperControl::Manage()
{
  if (this->inMove)
  {
    this->moveMotor();
  }
  else if (this->temperatureCompensationIsEnabled)
  {
  }
  else
  {
    // disable motor by timeout
    if((micros() - this->lastMovementTimestamp) >= 1000000)
    {
      digitalWrite(this->enablePin, HIGH);
    }
  }
}

void StepperControl::goToTargetPosition()
{
  if (this->currentPosition != this->targetPosition)
  {
    if (this->moveMode == SC_MOVEMODE_SMOOTH)
    {
      this->speed = 0;
      this->targetSpeedReached = false;
      this->positionTargetSpeedReached = 0;
    }
    else
    {
      this->speed = this->targetSpeed;
    }
    this->startPosition = this->currentPosition;
    digitalWrite(this->enablePin, LOW);
    this->inMove = true;
  }
}

void StepperControl::stopMovement()
{
  this->inMove = false;
  this->speed = 0;
  this->positionTargetSpeedReached = 0;
}

int StepperControl::isInMove()
{
  return this->inMove;
}

void StepperControl::compensateTemperature()
{
  long correction = 0;

  if (this->temperatureCompensationIsInit && !this->inMove)
  {
    correction = (long)(1.0 * (this->lastCompensatedTemperature - this->currentTemperature) * (float)this->temperatureCompensationCoefficient);

    if (correction)
    {
      this->lastCompensatedTemperature = this->currentTemperature;
      this->dbg_correction = this->getCurrentPosition() + (long)correction;
      this->setTargetPosition(this->getCurrentPosition() + (long)correction);
      this->goToTargetPosition();
    }
  }
}

//------------------------------------------------------------------------------------
// Privates
void StepperControl::moveMotor()
{
  if (this->moveMode == SC_MOVEMODE_SMOOTH)
  {
    this->calculateSpeed();
  }

  if ((this->targetPosition != this->currentPosition))
  {
    if ((this->speed != 0) && (micros() - this->lastMovementTimestamp) >= ((unsigned long)((1 / ((float)this->speed + 1)) * 1000000)))
    {
      if ((this->targetPosition - this->currentPosition) > 0)
      {
        if (this->direction == SC_CLOCKWISE)
        {
          digitalWrite(this->directionPin, LOW);
        }
        else
        {
          digitalWrite(this->directionPin, HIGH);
        }
        this->currentPosition++;
      }
      else
      {
        if (this->direction == SC_CLOCKWISE)
        {
          digitalWrite(this->directionPin, HIGH);
        }
        else
        {
          digitalWrite(this->directionPin, LOW);
        }
        this->currentPosition--;
      }
      digitalWrite(this->enablePin, LOW);
      delayMicroseconds(5);
      digitalWrite(this->stepPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(this->stepPin, LOW);

      if (this->speed >= this->targetSpeed)
      {
        if (!this->targetSpeedReached)
        {
          this->positionTargetSpeedReached = this->startPosition - this->currentPosition;
          if (this->positionTargetSpeedReached < 0)
          {
            this->positionTargetSpeedReached *= -1;
          }
        }
        this->speed = this->targetSpeed;
        this->targetSpeedReached = true;
      }
      this->lastMovementTimestamp = micros();
    }
  }
  else
  {
    this->stopMovement();
  }
}

void StepperControl::calculateSpeed()
{
  if ((millis() - this->accelTimestamp) >= 50)
  {
    long midway = (this->targetPosition - this->startPosition);
    // avoid miday == 0 in case of movement of only one step
    if (abs(midway) == 1)
    {
      midway *= 2;
    }

    midway /= 2;

    if (midway > 0)
    {
      midway += this->startPosition;
      if (!this->targetSpeedReached && (this->currentPosition < midway))
      {
        this->speed += this->acceleration;
      }
      else
      {
        if ((!this->targetSpeedReached && (this->currentPosition > midway)) || (this->currentPosition >= (this->targetPosition - this->positionTargetSpeedReached)))
        {

          if ((this->targetPosition != this->currentPosition) && (this->speed > this->acceleration))
          {
            this->speed -= this->acceleration;
          }
          else
          {
            this->speed = this->acceleration;
          }
        }
      }
    }
    else
    {
      midway = this->startPosition - -1 * midway;
      if (!this->targetSpeedReached && (this->currentPosition > midway))
      {
        this->speed += this->acceleration;
      }
      else
      {
        if ((!this->targetSpeedReached && (this->currentPosition < midway)) || (this->currentPosition <= (this->positionTargetSpeedReached + this->targetPosition)))
        {

          if ((this->targetPosition != this->currentPosition) && (this->speed > this->acceleration))
          {
            this->speed -= this->acceleration;
          }
          else
          {
            this->speed = this->acceleration;
          }
        }
      }
    }
    this->accelTimestamp = millis();
  }
}

bool StepperControl::isTemperatureCompensationEnabled()
{
  return this->temperatureCompensationIsEnabled;
}

void StepperControl::enableTemperatureCompensation()
{
  this->temperatureCompensationIsInit = false;
  this->temperatureCompensationIsEnabled = true;
}

void StepperControl::disableTemperatureCompensation()
{
  this->temperatureCompensationIsEnabled = false;
}

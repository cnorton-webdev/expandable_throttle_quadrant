
// Expandable Flightsim Throttle Quadrant Firmware
// Copyright 2020 Christopher Norton

// begin configuration section ----------------------------------------------------------------------------
#define SPOILERS true
#define ENGINES 2
#define FLAPS true
// end configuration section ----------------------------------------------------------------------------

// DO NOT MODIFY ANYTHING BEYOND THIS POINT UNLESS YOU KNOW WHAT YOU ARE DOING

#include <Joystick.h> // Joystick Library by Matthew Heironimus: https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <EEPROM.h>   // Save min/max to EEPROM
#define JUMPER 3
#define JUMPERTEST 8



Joystick_ Joystick(0x03, // 0 buttons, 0 hat switches, X (Spoilers), Y (Engine 1), Z (Engine 2), Rx (Engine 3), Ry (Engine 4), Rz (Flaps)
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, true, true, true, true, true,
                   false, false, false, false, false);

// setup ----------------------------------------------------------------------------
void setup()
{

  pinMode(2, OUTPUT); //use digital pin 2 as GND for Jumper
  digitalWrite(2, LOW);
  pinMode(JUMPER, INPUT_PULLUP);

  // Add ability to add jumper to enable / disable test mode
  pinMode(7, OUTPUT); // use digital pin 7 as GND for test Jumper
  digitalWrite(7, LOW);
  pinMode(JUMPERTEST, INPUT_PULLUP);

  Joystick.begin();

  // restore EEPROM saved calibrations
  unsigned int spoilersMin;
  unsigned int spoilersMax;
  unsigned int engineOneMin;
  unsigned int engineOneMax;
  unsigned int engineTwoMin;
  unsigned int engineTwoMax;
  unsigned int engineThreeMin;
  unsigned int engineThreeMax;
  unsigned int engineFourMin;
  unsigned int engineFourMax;
  unsigned int flapsMin;
  unsigned int flapsMax;

  if (SPOILERS) {
    spoilersMin = (EEPROM.read(0) | (EEPROM.read(1) << 8));
    spoilersMax = (EEPROM.read(2) | (EEPROM.read(3) << 8));
    Joystick.setXAxisRange(spoilersMin, spoilersMax);
  }
  if (ENGINES >= 1) {
    engineOneMin = (EEPROM.read(4) | (EEPROM.read(5) << 8));
    engineOneMax = (EEPROM.read(6) | (EEPROM.read(7) << 8));
    Joystick.setYAxisRange(engineOneMin, engineOneMax);
  }
  if (ENGINES >= 2) {
    engineTwoMin = (EEPROM.read(8) | (EEPROM.read(9) << 8));
    engineTwoMax = (EEPROM.read(10) | (EEPROM.read(11) << 8));
    Joystick.setZAxisRange(engineTwoMin, engineTwoMax);
  }
  if (ENGINES >= 3) {
    engineThreeMin = (EEPROM.read(12) | (EEPROM.read(13) << 8));
    engineThreeMax = (EEPROM.read(14) | (EEPROM.read(15) << 8));
    Joystick.setRxAxisRange(engineThreeMin, engineThreeMax);
  }
  if (ENGINES >= 4) {
    engineFourMin = (EEPROM.read(16) | (EEPROM.read(17) << 8));
    engineFourMax = (EEPROM.read(18) | (EEPROM.read(19) << 8));
    Joystick.setRyAxisRange(engineFourMin, engineFourMax);
  }
  if (FLAPS) {
    flapsMin = (EEPROM.read(20) | (EEPROM.read(21) << 8));
    flapsMax = (EEPROM.read(22) | (EEPROM.read(23) << 8));
    Joystick.setRzAxisRange(flapsMin, flapsMax);
  }
}

// loop ----------------------------------------------------------------------------
void loop()
{

  // get filtered ADC values and set Axis
  if (SPOILERS) {
    Joystick.setXAxis(filter(A0)); // Spoilers
  }
  if (ENGINES >= 1) {
    Joystick.setYAxis(filter(A1)); // Engine One
  }
  if (ENGINES >= 2) {
    Joystick.setZAxis(filter(A2)); // Engine Two
  }
  if (ENGINES >= 3) {
    Joystick.setRxAxis(filter(A3)); // Engine Three
  }
  if (ENGINES >= 4) {
    Joystick.setRyAxis(filter(A6)); // Engine Four
  }
  if (FLAPS) {
    Joystick.setRzAxis(filter(A7)); // Flaps
  }

  // Digital pins 2 & 3 shorted via jumper to enter calibration mode
  if (!digitalRead(JUMPER))
    calibration();

  // check if Jumper is set on pins 7 and 8 to enable test mode
  if (!digitalRead(JUMPERTEST))
    test();
}

//analog read with moving average filter ----------------------------------------------------------------------------
int filter(int channel)
{
  unsigned int sum = 0;
  unsigned int i = 0;

  for (i = 0; i < 50; i++)
  {
    sum = sum + analogRead(channel);
  }

  sum = sum / 50;
  return (sum);
}

// calibration ===================================================================================================
void calibration()
{
  unsigned int spoilersMin = 0;
  unsigned int spoilersMax = 1023;
  unsigned int engineOneMin = 0;
  unsigned int engineOneMax = 1023;
  unsigned int engineTwoMin = 0;
  unsigned int engineTwoMax = 1023;
  unsigned int engineThreeMin = 0;
  unsigned int engineThreeMax = 1023;
  unsigned int engineFourMin = 0;
  unsigned int engineFourMax = 1023;
  unsigned int flapsMin = 0;
  unsigned int flapsMax = 1023;

  unsigned int temp = 0;



  // capture max/min values as long as the jumper is connected
  while (!digitalRead(JUMPER))
  {

    if (SPOILERS) {
      temp = filter(A0);
      if (temp < spoilersMin)
      {
        spoilersMin = temp;
      }
      if (temp > spoilersMax)
      {
        spoilersMax = temp;
      }
    }
    if (ENGINES >= 1) {
      temp = filter(A1);
      if (temp < engineOneMin)
      {
        engineOneMin = temp;
      }
      if (temp > engineOneMax)
      {
        engineOneMax = temp;
      }
    }
    if (ENGINES >= 2) {
      temp = filter(A2);
      if (temp < engineTwoMin)
      {
        engineTwoMin = temp;
      }
      if (temp > engineTwoMax)
      {
        engineTwoMax = temp;
      }
    }
    if (ENGINES >= 3) {
      temp = filter(A3);
      if (temp < engineThreeMin)
      {
        engineThreeMin = temp;
      }
      if (temp > engineThreeMax)
      {
        engineThreeMax = temp;
      }
    }
    if (ENGINES >= 4) {
      temp = filter(A6);
      if (temp < engineFourMin)
      {
        engineFourMin = temp;
      }
      if (temp > engineFourMax)
      {
        engineFourMax = temp;
      }
    }
    if (FLAPS) {
      temp = filter(A7);
      if (temp < flapsMin)
      {
        flapsMin = temp;
      }
      temp = filter(A7);
      if (temp > flapsMax)
      {
        flapsMax = temp;
      }
    }
  }

  // save ranges
  if (SPOILERS) {
    Joystick.setXAxisRange(spoilersMin, spoilersMax);
  }
  if (ENGINES >= 1) {
    Joystick.setYAxisRange(engineOneMin, engineOneMax);
  }
  if (ENGINES >= 2) {
    Joystick.setZAxisRange(engineTwoMin, engineTwoMax);
  }
  if (ENGINES >= 3) {
    Joystick.setRxAxisRange(engineThreeMin, engineThreeMax);
  }
  if (ENGINES >= 4) {
    Joystick.setRyAxisRange(engineFourMin, engineFourMax);
  }
  if (FLAPS) {
    Joystick.setRzAxisRange(flapsMin, flapsMax);
  }
  if (SPOILERS) {
    EEPROM.write(0, lowByte(spoilersMin));
    EEPROM.write(1, highByte(spoilersMin));
    EEPROM.write(2, lowByte(spoilersMax));
    EEPROM.write(3, highByte(spoilersMax));
  }
  if (ENGINES >= 1) {
    EEPROM.write(4, lowByte(engineOneMin));
    EEPROM.write(5, highByte(engineOneMin));
    EEPROM.write(6, lowByte(engineOneMax));
    EEPROM.write(7, highByte(engineOneMax));
  }
  if (ENGINES >= 2) {
    EEPROM.write(8, lowByte(engineTwoMin));
    EEPROM.write(9, highByte(engineTwoMin));
    EEPROM.write(10, lowByte(engineTwoMax));
    EEPROM.write(11, highByte(engineTwoMax));
  }
  if (ENGINES >= 3) {
    EEPROM.write(12, lowByte(engineThreeMin));
    EEPROM.write(13, highByte(engineThreeMin));
    EEPROM.write(14, lowByte(engineThreeMax));
    EEPROM.write(15, highByte(engineThreeMax));
  }
  if (ENGINES >= 4) {
    EEPROM.write(16, lowByte(engineFourMin));
    EEPROM.write(17, highByte(engineFourMin));
    EEPROM.write(18, lowByte(engineFourMax));
    EEPROM.write(19, highByte(engineFourMax));
  }
  if (FLAPS) {
    EEPROM.write(20, lowByte(flapsMin));
    EEPROM.write(21, highByte(flapsMin));
    EEPROM.write(22, lowByte(flapsMax));
    EEPROM.write(23, highByte(flapsMax));
  }
  delay(100);
}

// testmode ===================================================================================================
void test()
{
  if (SPOILERS) {
    Serial.print("Spoilers: ");
    Serial.print(filter(A0));
    Serial.print("\n");
  }
  if (ENGINES >= 1) {
    Serial.print("Engine One: ");
    Serial.print(filter(A1));
    Serial.print("\n");
  }
  if (ENGINES >= 2) {
    Serial.print("Engine Two: ");
    Serial.print(filter(A2));
    Serial.print("\n");
  }
  if (ENGINES >= 3) {
    Serial.print("Engine Three: ");
    Serial.print(filter(A3));
    Serial.print("\n");
  }
  if (ENGINES >= 4) {
    Serial.print("Engine Four: ");
    Serial.print(filter(A6));
    Serial.print("\n");
  }
  if (FLAPS) {
    Serial.print("Flaps: ");
    Serial.print(filter(A7));
    Serial.print("\n");
  }
}

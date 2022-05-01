#include "Arduino.h"

// shifts out a byte onto the shift register
void updateShiftRegisters(byte pinRCLK, byte pinSRCLK, byte pinSER, byte val)
{
  digitalWrite(pinRCLK, LOW);
  
  shiftOut(pinSER, pinSRCLK, MSBFIRST, val);
  
  digitalWrite(pinRCLK, HIGH);
}

// generate shift register code for desired numeral
int getNumeralCode(byte numeral)
{
  byte numerals[11];
  byte segA, segB, segC, segD, segE, segF, segG;

  // numeral layout:
  //
  //  AAAAA
  // F     B
  // F     B
  //  GGGGG
  // E     C
  // E     C
  //  DDDDD

  segA = 0x01;
  segB = 0x02;
  segC = 0x04;
  segD = 0x08;
  segE = 0x10;
  segF = 0x20;
  segG = 0x40;

  // segment combinations used to make a lit numeral on the 7-segment digit
  numerals[0] = ~(segA + segB + segC + segD + segE + segF);
  numerals[1] = ~(segB + segC);
  numerals[2] = ~(segA + segB + segG + segE + segD);
  numerals[3] = ~(segA + segB + segG + segC + segD);
  numerals[4] = ~(segF + segG + segB + segC);
  numerals[5] = ~(segA + segF + segG + segC + segD);
  numerals[6] = ~(segF + segG + segE + segC + segD);
  numerals[7] = ~(segA + segB + segC);
  numerals[8] = ~(segA + segB + segC + segD + segE + segF + segG);
  numerals[9] = ~(segA + segF + segB + segG + segC);
  numerals[10] = segA + segB + segC + segD + segE + segF + segG;

  return numerals[numeral];
}

// generate shift register code for desired digit
int getDigitCode(byte digitNum)
{
  int digits[9];

  // digits layot
  //
  //     0 1 : 2 3
  //
  //  4 5    6    7 8
  //

  digits[0] = 0x0080;
  digits[1] = 0x0100;
  digits[2] = 0x0200;
  digits[3] = 0x0400;
  digits[4] = 0x0800;
  digits[5] = 0x1000;
  digits[6] = 0x2000;
  digits[7] = 0x4000;
  digits[8] = 0x8000;

  return digits[digitNum];
}

void writeNumber(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte digit, byte numeral)
{
  int digitCode, numeralCode, segCode;
  byte msb, lsb;

  digitCode = getDigitCode(digit);
  numeralCode = getNumeralCode(numeral);

  segCode = digitCode + (numeralCode & 0x7F);
  msb = (byte)(segCode >> 8);
  lsb = (byte)segCode;

  digitalWrite(pinOE, HIGH);
  updateShiftRegisters(pinRCLK, pinSRCLK, pinSER, msb);
  updateShiftRegisters(pinRCLK, pinSRCLK, pinSER, lsb);
  digitalWrite(pinOE, LOW);
}

void writeTimer(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, int t)
{
  int mm, ss;
  byte MSm, LSm, MSs, LSs;
  mm = t / 60;
  ss = t - mm*60;

  MSm = (byte) mm/10;
  LSm = (byte) (mm - MSm*10);
  MSs = (byte) ss/10;
  LSs = (byte) (ss - MSs*10);

  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 0, MSm);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 1, LSm);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 2, MSs);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 3, LSs);
  delay(2);
}

void bottomButtonResponse(byte pin, byte &value, unsigned long &timer, byte &count, byte limit)
{
  byte valNEW = digitalRead(pin);
  
  value = ((value << 1) | valNEW) & 0x03;

  switch (value)
  {
    case 0x02: // just pressed
      timer = millis();
      break;

    case 0x00: // still pressed
      if (millis() - timer > 1000)
      {
        count = 0;
        timer = 0;
      }
      break;

    case 0x01: // just released
      if (timer != 0)
        count++;
      break;

    case 0x03: // still released
      break;
  }
  if (count > limit)
    count = 0;
}

void timerButtonResponse(byte pin, byte &value, unsigned long &timer, bool &state, int &timerVALUE)
{
  byte valNEW = digitalRead(pin);
  
  value = ((value << 1) | valNEW) & 0x03;

  switch (value)
  {
    case 0x02: // just pressed
      timer = millis();
      break;

    case 0x00: // still pressed
      if (millis() - timer > 1000)
      {
        state = 0;
        timerVALUE = 0;
        timer = 0;
      }
      break;

    case 0x01: // just released
      if (timer != 0)
        state = !state;
      break;

    case 0x03: // still released
      break;
  }
}

void timerOff(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE)
{
  byte off_code = 10;
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 0, off_code);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 1, off_code);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 2, off_code);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 3, off_code);
  delay(2);
}

void writeCounters(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte cntSCORELEFT, byte cntSCORERIGHT, byte cntPERIOD)
{
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 4, cntSCORELEFT / 10);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 5, cntSCORELEFT % 10);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 6, cntSCORERIGHT / 10);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 7, cntSCORERIGHT % 10);
  delay(2);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 8, cntPERIOD);
  delay(2);
}

void flashTimer(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte cntSCORELEFT, byte cntSCORERIGHT, byte cntPERIOD)
{
  byte flash_on = 0;
  byte counter = 0;
  unsigned long startTime = millis();

  while (counter < 5)
  {
    if (millis() - startTime < 500)
    {
      timerOff(pinRCLK, pinSRCLK, pinSER, pinOE);
      writeCounters(pinRCLK, pinSRCLK, pinSER, pinOE, cntSCORELEFT, cntSCORERIGHT, cntPERIOD);
    }
    else if (millis() - startTime < 1000)
    {
      writeTimer(pinRCLK, pinSRCLK, pinSER, pinOE, flash_on);
      writeCounters(pinRCLK, pinSRCLK, pinSER, pinOE, cntSCORELEFT, cntSCORERIGHT, cntPERIOD);
    }
    else
    {
      startTime = millis();
      counter++;
    }
  }
}

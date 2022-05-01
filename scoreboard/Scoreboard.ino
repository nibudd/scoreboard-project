#include "ScoreboardUtils.h"

// pins
byte btnPERIOD = 2;
byte btnSCORELEFT = 3;
byte btnSCORERIGHT = 4;
byte btnPLAYPAUSE = 5;
byte pinSER = 6; // serial data in
byte pinRCLK = 7; // latch for allowing data entry
byte pinSRCLK = 8; // clock for timing data entry
byte pinOE = 9; // output enable (enabled on low)
byte pinKNOBA = 10;
byte pinKNOBB = 11;

// button timers
unsigned long timeNow = millis();
unsigned long tmrPERIOD = 0;
unsigned long tmrSCORELEFT = 0;
unsigned long tmrSCORERIGHT = 0;
unsigned long tmrPLAYPAUSE = 0;

// button histories
byte valPERIOD = 0x03;
byte valSCORELEFT = 0x03;
byte valSCORERIGHT = 0x03;
byte valPLAYPAUSE = 0x03;

// button counts
byte cntPERIOD = 0;
byte cntSCORELEFT = 0;
byte cntSCORERIGHT = 0;
bool sttPLAYPAUSE = 0;

// button limits
byte lmtPERIOD = 9;
byte lmtSCORE = 99;
byte lmtPLAYPAUSE = 1;

// timer
int timerVALUE = 0;
unsigned long startTIME = 0;
int delayMs = 2;

// knob
int encoded;
int lastEncoded = 0;
int encoderValue = 0;
int MSB;
int LSB;
int lastMSB;
int lastLSB;
int sum;

void setup()
{
  pinMode(btnPERIOD, INPUT_PULLUP);
  pinMode(btnSCORELEFT, INPUT_PULLUP);
  pinMode(btnSCORERIGHT, INPUT_PULLUP);
  pinMode(btnPLAYPAUSE, INPUT_PULLUP);
  pinMode(pinSER, OUTPUT);
  pinMode(pinRCLK, OUTPUT);
  pinMode(pinSRCLK, OUTPUT);
  pinMode(pinOE, OUTPUT);
  pinMode(pinKNOBA, INPUT_PULLUP);
  pinMode(pinKNOBB, INPUT_PULLUP);
  Serial.begin(9600);
}



void loop()
{
//  UNCOMMENT TO TURN ON ALL LEDS SIMULTANEOUSLY
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 0, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 1, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 2, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 3, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 4, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 5, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 6, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 7, 8);
//  delay(delayMs);
//  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 8, 8);
//  delay(delayMs);
  
  bottomButtonResponse(btnSCORELEFT, valSCORELEFT, tmrSCORELEFT, cntSCORELEFT, lmtSCORE);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 4, cntSCORELEFT / 10);
  delay(delayMs);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 5, cntSCORELEFT % 10);
  delay(delayMs);

  bottomButtonResponse(btnSCORERIGHT, valSCORERIGHT, tmrSCORERIGHT, cntSCORERIGHT, lmtSCORE);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 6, cntSCORERIGHT / 10);
  delay(delayMs);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 7, cntSCORERIGHT % 10);
  delay(delayMs);
  
  bottomButtonResponse(btnPERIOD, valPERIOD, tmrPERIOD, cntPERIOD, lmtPERIOD);
  writeNumber(pinRCLK, pinSRCLK, pinSER, pinOE, 8, cntPERIOD);
  delay(delayMs);

  timerButtonResponse(btnPLAYPAUSE, valPLAYPAUSE, tmrPLAYPAUSE, sttPLAYPAUSE, timerVALUE);
  if (sttPLAYPAUSE == 1)
  {
    if (valPLAYPAUSE == 0x02)
      startTIME = millis();
    
    if (millis() - startTIME >= 1000)
      {
        timerVALUE--;
        if (timerVALUE < 0)
        {
          timerVALUE = 0;
          sttPLAYPAUSE = 0;
          flashTimer(pinRCLK, pinSRCLK, pinSER, pinOE, cntSCORELEFT, cntSCORERIGHT, cntPERIOD);
        }
        startTIME = millis();
      }
  }
  else
  {
    MSB = digitalRead(pinKNOBA);
    LSB = digitalRead(pinKNOBB);
    encoded = ((MSB << 1) | LSB) & 0x03;
    if (encoded != lastEncoded)
    {
      sum = (lastEncoded << 2) | encoded;
      if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        timerVALUE = timerVALUE + 5;
    
      if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
      {
        timerVALUE = timerVALUE - 5;
        if (timerVALUE < 0)
          timerVALUE = 0; 
      }
    
      lastEncoded = encoded;
    }
  }
  writeTimer(pinRCLK, pinSRCLK, pinSER, pinOE, timerVALUE);
}

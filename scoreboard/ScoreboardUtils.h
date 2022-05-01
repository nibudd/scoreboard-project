void updateShiftRegisters(byte pinRCLK, byte pinSRCLK, byte pinSER, byte val);

int getNumeralCode(byte numeral);

int getDigitCode(byte digitNum);

void writeNumber(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte digit, byte numeral);

void writeTimer(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, int t);

void bottomButtonResponse(byte pin, byte &value, unsigned long &timer, byte &count, byte limit);

void timerButtonResponse(byte pin, byte &value, unsigned long &timer, bool &state, int &timerVALUE);

void timerOff(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE);

void writeCounters(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte cntSCORELEFT, byte cntSCORERIGHT, byte cntPERIOD);

void flashTimer(byte pinRCLK, byte pinSRCLK, byte pinSER, byte pinOE, byte cntSCORELEFT, byte cntSCORERIGHT, byte cntPERIOD);

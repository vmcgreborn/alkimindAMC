/*
	PUSH_BUTTON.h Library permite obtener funciones de un push button como latch presion
	de avance y continuos push
	Created by Victor Manuel Camacho.
*/
#ifndef PUSH_BUTTON
#define PUSH_BUTTON

#include "Arduino.h"

class PushButton
{
  private:
	
    boolean _pushed=false;
	boolean _hold=false;
	boolean _momentInt=false;
	boolean _latch=false;
	uint8_t _input;
	long _holdTime;
	long _momentIntTime;
  public:
	void begin(uint8_t input);
	boolean isOnePulse();
	boolean isHoldOnPulse(int time);
	boolean isHoldOnLatch(int time);
	boolean isMomentaryIntermitentOn();
	boolean isMomentary();
	boolean isLatch();
};

#endif

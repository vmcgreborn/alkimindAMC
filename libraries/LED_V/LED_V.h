/*
	LED V libreria para controlar leds y comportamientos
	Created by Victor Manuel Camacho.
*/
#ifndef LED_V
#define LED_V

#include "Arduino.h"

class Led
{
  private:
	int _output;
	long _blinkTime=0;
	long _dimTime;
	int _incremento=3;
	int _brillo;

  public:
	void begin(unsigned int output);
  /*
	*Comienza la animacion de el blink por el tiempo dado y cambia el valor de la variable que lo illumina
  */
	void blinkOff(int time,boolean &startBlink);
	/**
	 * para la ejecucion
	 */
	void delayBlinkOff(int time);

  /*
	*Comienza la animacion de el dim por el tiempo dado y cambia el valor de la variable que lo illumina
  */
	void dimOff(int time,boolean &startDim);
	
	void on();
	void booleanWrite(boolean val);
	void off();
	void pwmWrite(int val,int maxInput);
	
};

#endif

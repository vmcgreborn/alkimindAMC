/*
	POT v libreria con funciones avanzadas de potenciometros.
	Created by Victor Manuel Camacho.
*/
#ifndef POT_V
#define POT_V

#include "Arduino.h"
class PotV
{
  private:
	int _maxDegree;
	boolean _detach=true;
	uint8_t _analogPinNumber=A0;
	uint8_t _inputA;
	uint8_t _inputB;
	int atachDetachValue(int actualValue,int inputValue,int tolerance);
	int _lastValue;
	int _aLastState;

  public:
	PotV(int maxDegree,uint8_t analogPinInput);
	PotV(int maxDegree);
	PotV(int maxDegree,uint8_t inputA,uint8_t inputB);
	void detach();
	int quickPotDegree(int actualValue);
	int quickPotPosition(int numPos,int minPos,int actualValue);
	int quickMidiDegree(int actualValue,int input);
	int quickMidiPosition(int numPos,int minPos,int actualValue,int input);
	boolean encoderDegree(int &value);
	boolean encoderMap(int min,int max,int &value);
	boolean encoderMap(int min,int max,int &value,int increment,boolean circular);
	void setLastState();
	boolean potChange();
	boolean potMidiChange(int currentValue);
};

#endif

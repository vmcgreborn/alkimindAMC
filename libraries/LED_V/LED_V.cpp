/*
	LED V libreria para controlar leds y comportamientos
	Created by Victor Manuel Camacho.
	Created by Victor Manuel Camacho.
*/

#include "Arduino.h"
#include "LED_V.h"

#define BLINK_DELAY 30
#define DIM_DELAY 1
void Led::begin(unsigned int output){
	_output=output;
	pinMode(output, OUTPUT);
}
void Led::blinkOff(int time,boolean &startBlink){
	if(millis()-_blinkTime<time&&startBlink){
		delay(BLINK_DELAY);
		digitalWrite(_output, HIGH);
		delay(BLINK_DELAY);
		digitalWrite(_output, LOW);
	}else{
		_blinkTime=millis();
		startBlink=false;
	}
}
void Led::delayBlinkOff(int time){
	_blinkTime=millis();
		while(millis()-_blinkTime<time){
			delay(BLINK_DELAY);
			digitalWrite(_output, HIGH);
			delay(BLINK_DELAY);
			digitalWrite(_output, LOW);
		}
}
void Led::dimOff(int time,boolean &startDim){
	if(millis()-_dimTime<time&&startDim){
		analogWrite(_output, _brillo); //coloca el valor brillo en el pin 9
		_brillo = _brillo + _incremento; //brillo aumenta su valor en 5
		if (_brillo == 0 || _brillo == 255) { //si brillo llega a sus límites...
			_incremento = -_incremento; //...pasamos de ir creciendo a pasar...
		//...a ir decreciendo y al revés
		}
		delay(DIM_DELAY); /*hay que dar un pequeño tiempo entre valor y valor
			de brillo para que la variación no sea instantánea */
	}else{
		analogWrite(_output, 0);
		_dimTime=millis();
		startDim=false;
	}
}

void Led::booleanWrite(boolean val){
	digitalWrite(_output, val);
}
void Led::pwmWrite(int val,int maxInput){
	analogWrite(_output,map(val,0,maxInput,0,255));
}
void Led::on(){
	digitalWrite(_output, HIGH);
}

void Led::off(){
	digitalWrite(_output, LOW);
}


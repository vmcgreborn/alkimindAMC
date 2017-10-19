/*
	PUSH_BUTTON library permite obtener funciones de un push button como latch presion
	de avance y continuos push
	Created by Victor Manuel Camacho.
*/

#include "Arduino.h"
#include "PUSH_BUTTON.h"


#define MOMENTARY_INTERVAL 500

void PushButton::begin(uint8_t input){
	if(input!=13){//no es posible modo pullup para pin 13
		pinMode(input,  INPUT_PULLUP);
	}else{
		pinMode(input, INPUT);
	}
	_input=input;
}

//solo lanza una ves true en el loop cada vez que se presione y se suelte
boolean PushButton::isOnePulse(){

	if(digitalRead(_input)&&_pushed){
		_pushed=false;
		return true;
	}
	if(!digitalRead(_input)){
		_pushed=true;
	}	
	return false;
}
//cambia el valor en el loop cada vez que se presione y se suelte
boolean PushButton::isLatch(){

	if(digitalRead(_input)&&_pushed){
		_pushed=false;
		_latch=!_latch;
	}
	if(!digitalRead(_input)){
		_pushed=true;
	}
	return _latch;
}
//solo lanza una vez true en el loop cumplido el tiempo hasta que se suelte y se presione otravez por el tiempo
boolean PushButton::isHoldOnPulse(int time){
	if(digitalRead(_input)){
		_holdTime=millis();
		_hold=false;
	}
	if(((millis()-_holdTime)>time)&&!(_hold)){
		_hold=true;
		return true;
	}
	return false;
}
boolean PushButton::isHoldOnLatch(int time){
	if(digitalRead(_input)){
		_holdTime=millis();
		_hold=false;
	}
	if(((millis()-_holdTime)>time)&&!(_hold)){
		_hold=true;
		_latch=!_latch;
	}
	return _latch;
}
//lanza true y false mientras se presione si se suelta se queda en true en el loop
boolean PushButton::isMomentaryIntermitentOn(){
	
	if (!digitalRead(_input)){
		if((millis()-_momentIntTime) > MOMENTARY_INTERVAL) {
			if (_momentInt) {
				_momentInt = false;
			} else {
				_momentInt = true;
			}
			_momentIntTime=millis();
		}
	}else{
		_momentInt=false;
		_momentIntTime=millis();
	}
	
	
	return _momentInt;
}
//lanza true mientras se presione y false si se suelta.
boolean PushButton::isMomentary(){
	return !digitalRead(_input);
}

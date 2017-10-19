/*
 * POT_V.cpp
 *
 *  Created on: 10/10/2016
 *      Author: Victor Camacho
 */
#include "Arduino.h"
#include "POT_V.h"
#define MAX_ANALOG_INPUT 1023
#define MAX_MIDI_INPUT 127
#define DEEGRE_ATACH_TOLERANCE 5
#define DEGREE_INCREMENT -1
#define NORMAL_INCREMENT 1
#define PRECISION_TIME 500

PotV::PotV(int maxDegree,uint8_t analogPinInput){
	_maxDegree=maxDegree;
	_analogPinNumber=analogPinInput;
	_lastValue=analogRead(_analogPinNumber);
}
PotV::PotV(int maxDegree){

	_maxDegree=maxDegree;

}
PotV::PotV(int maxDegree,uint8_t inputA,uint8_t inputB){
	_maxDegree=maxDegree;
	_inputA=inputA;
	_inputB=inputB;
}
void PotV::detach(){
	_detach=true;
}
int PotV::quickPotDegree(int actualValue){

	int potMapValue=map(analogRead(_analogPinNumber),0,MAX_ANALOG_INPUT,0,_maxDegree);
	return atachDetachValue(actualValue,potMapValue,DEEGRE_ATACH_TOLERANCE);

}
//privado
int PotV::atachDetachValue(int actualValue,int inputValue,int tolerance){
	int diference=actualValue-inputValue;
	if(_detach){
		if((diference<=tolerance)&&(diference>=(-tolerance))){
			_detach=false;
		}
		return actualValue;
	}else{
		return inputValue;
	}
}
int PotV::quickPotPosition(int numPos,int minpos,int actualValue){
	int potMapValue=map(analogRead(_analogPinNumber),0,MAX_ANALOG_INPUT,minpos,numPos);
	return atachDetachValue(actualValue,potMapValue,numPos/20);
}
int PotV::quickMidiDegree(int actualValue,int input){

	int potMapValue=map(input,0,MAX_MIDI_INPUT,0,_maxDegree);
	return atachDetachValue(actualValue,potMapValue,DEEGRE_ATACH_TOLERANCE);
}
int PotV::quickMidiPosition(int numPos,int minPos,int actualValue,int input){
	int potMapValue=map(input,0,MAX_MIDI_INPUT,minPos,numPos);
	return atachDetachValue(actualValue,potMapValue,numPos/20);
}
boolean PotV::potChange(){
	int currentValue=analogRead(_analogPinNumber);
	if(currentValue!=_lastValue){
		_lastValue=currentValue;
		return true;
	}else{
		return false;
	}
}
boolean PotV::potMidiChange(int currentValue){
	if(currentValue!=_lastValue){
		_lastValue=currentValue;
		return true;
	}else{
		return false;
	}
}
boolean PotV::encoderDegree(int &value){

	return PotV::encoderMap(0,_maxDegree,value,DEGREE_INCREMENT,false);
}
boolean PotV::encoderMap(int min,int max,int &value){
	return PotV::encoderMap(min,max,value,NORMAL_INCREMENT,true);
}


boolean PotV::encoderMap(int min,int max,int &value,int increment,boolean circular){
	int lastValue=value;
	int aState = digitalRead(_inputA); // Reads the "current" state of the outputA

	// If the previous and the current state of the outputA are different, that means a Pulse has occured
	if (aState != _aLastState){
		// If the outputB state is different to the outputA state, that means the encoder is rotating clockwise

		if (digitalRead(_inputB) != aState) {
			value=value+increment;
		} else {
			value=value-increment;
		}
		if(circular){
			if(value<min){
				value=max;
			}
			if(value>max){
				value=min;
			}
		}else{
			if(value<min){
				value=min;
			}
			if(value>max){
				value=max;
			}
		}
	}
	_aLastState = aState; // Updates the previous state of the outputA with the current state
	if(lastValue!=value){

		return true;
	}else{
		return false;
	}
}
void PotV::setLastState(){
	_aLastState=digitalRead(_inputA);
}

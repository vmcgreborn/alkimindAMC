/*
 DEBUGGER_V_UNO Libreria avanzada para debuggear con el Serial1 port
 Created by Victor Manuel Camacho.
 */

#include "Arduino.h"
#include "DEBUGGER_V_UNO.h"
#include "ArduinoJson.h"

Debugger::Debugger(boolean debugg,boolean debuggInput,int pauseTime) {
	_debuggInput=debuggInput;
	_debugg = debugg;
	_pauseTime = pauseTime;
	_s_in=&Serial;
	_s_out=&Serial;
}
void Debugger::debuggPause(String val) {
	if (_debugg&&showTime()) {
		_s_out->println(val);
	}
}
void Debugger::debuggPause(int val) {
	if (_debugg&&showTime()) {
		_s_out->println(val);
	}
}
void Debugger::debuggPause(float val) {
	if (_debugg&&showTime()) {
		_s_out->println(val);
	}
}
void Debugger::debuggln(String val) {
	if (_debugg) {
		_s_out->println(val);
	}
}
void Debugger::debuggln(int val) {
	if (_debugg) {
		_s_out->println(val);
	}
}
void Debugger::debuggln(float val) {
	if (_debugg) {
		_s_out->println(val);
	}
}
void Debugger::debugg(String val) {
	if (_debugg) {
		_s_out->print(val);
	}
}
void Debugger::debuggParam(String str,int val){
	if (_debugg) {
		_s_out->print(str);
		_s_out->print(":");
		_s_out->println(val);
	}
}
void Debugger::debuggParam(String str,float val){
	if (_debugg) {
		_s_out->print(str);
		_s_out->print(":");
		_s_out->println(val);
	}
}
void Debugger::debugg(int val) {
	if (_debugg) {
		_s_out->println(val);
	}
}
void Debugger::debugg(float val) {
	if (_debugg) {
		_s_out->println(val);
	}
}
void Debugger::begin(unsigned int portOut,unsigned int portIn) {
	if (_debugg) {
		_s_out->begin(portOut);

	}
	if(_debuggInput){
		_s_in->begin(portIn);
	}
}

boolean Debugger::readSerialInputString(){

	if(_debuggInput){

		String serialInput=_s_in->readString();
		if(serialInput.length()>0){
			_serialInput=serialInput;
			debugg("serialInput:");
			debuggln(_serialInput);
			return true;
		}
	}
}
void Debugger::readJsonParam(int &value,String param){
	if(_debuggInput){
		if(_serialInput.length()>0){
			StaticJsonBuffer<200> _jsonBuffer;
			JsonObject& root = _jsonBuffer.parseObject(_serialInput);
			value=root[param];
		}
	}
}
void Debugger::cleanJson(){
	if(_debuggInput){
		_serialInput="{}";
	}
}

boolean Debugger::showTime(){
	if((millis()-_timer)>_pauseTime){
		return true;
	}else{
		return false;
	}
}
void Debugger::pauseStart(){
	if((millis()-_timer)>_pauseTime){
		_timer=millis();
	}
}

boolean Debugger::isDebuggInput(){
	return _debuggInput;
}
boolean Debugger::isDebuggOutput(){
	return _debugg;
}

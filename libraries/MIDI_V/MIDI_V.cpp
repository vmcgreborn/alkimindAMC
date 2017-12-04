/*
	LED V libreria para controlar leds y comportamientos
	Created by Victor Manuel Camacho.
	Created by Victor Manuel Camacho.
*/

#include "Arduino.h"
#include "MIDI_V.h"
#include "DEBUGGER_V.h"
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#define CC "cc"
#define PC  "pc"
#define CC_VALUE "ccValue"

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial,Serial2,MIDI);
boolean MidiV::_isReadCC=false;
boolean MidiV::_isReadPC=false;
int MidiV::_cc;
int MidiV::_pc;
int MidiV::_ccValue;

MidiV::MidiV(Debugger *debugger){
	_debugger=debugger;
}
void MidiV::readCCFunction(byte channel, byte number, byte value){
	MidiV::_cc=number;
	MidiV::_ccValue=value;

	MidiV::_isReadCC=true;
}
void MidiV::readPCFunction(byte channel, byte number){
	MidiV::_pc=number;
	MidiV::_isReadPC=true;
}
void MidiV::begin(int chanel){
	if(!_debugger->isDebuggInput()){
		_debugger->debuggln("Begin Midi...");
	  MIDI.begin(chanel); // Initialize the Midi Library.
	  MIDI.setHandleControlChange(MidiV::readCCFunction); // This command tells the MIDI Library
	  MIDI.setHandleProgramChange(MidiV::readPCFunction);
	}
}


int MidiV::getCC(){
	return _cc;
}
int MidiV::getPC(){
	return _pc;
}
int MidiV::getCCValue(){
	return _ccValue;
}
void MidiV::midiRead(){
	int cc=0;
	int pc=0;
	if(_debugger->isDebuggInput()){
		if(_debugger->readSerialInputString()){
			_debugger->readJsonParam(cc, CC);
			_debugger->readJsonParam(pc, PC);
			if(cc!=0){
				_debugger->debuggParam("Emulated CC",cc);
				_isReadCC=true;
				_cc=cc;
				_debugger->readJsonParam(_ccValue, CC_VALUE);
			}
			if(pc!=0){
				_debugger->debuggParam("Emulated PC",pc);
				_isReadPC=true;
				_pc=pc;
			}
			_debugger->cleanJson();
		}
	}else{
		MIDI.read();
	}

}
boolean MidiV::isReadCC(){
	if(_isReadCC){
		_isReadCC=false;
		return true;
	}
	return _isReadCC;
}
boolean MidiV::isReadPC(){
	if(_isReadPC){
		_isReadPC=false;
		return true;
	}
	return _isReadPC;
}

void MidiV::sendPC(unsigned inProgramNumber,unsigned inChannel){
	MIDI.sendProgramChange(inProgramNumber,inChannel);
}
void MidiV::sendCC(unsigned inControlNumber,unsigned inControlValue,unsigned inChannel){
	MIDI.sendControlChange(inControlNumber,inControlValue,inChannel);
}


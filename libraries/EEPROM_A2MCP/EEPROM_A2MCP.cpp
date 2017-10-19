/*
 EEPROM A2MCP libreria con funciones avanzadas para memoria de A2MCP
 Created by Victor Manuel Camacho.
 */

#include <EEPROM_A2MCP.h>
#include "Arduino.h"
#include "EEPROM.h"
#define BYTES_PER_BANK 8
#define CONTROLS 5
#define ONOFF 2
#define POTS 7
#define MAX_EEPROM_BANKS 99
#define MAX_DEGREE 260
#define MAX_BYTE_NUMBER 255
#define DEFAULT_CONTRAST 50
#define DEFAULT_START_BANK 1
#define REMOTE_DEFAULT_BANK 0
#define DEFAULT__Q_PEDAL_BANK 5
const int GLOBAL_CONFIG_EEPROM_START_POS = BYTES_PER_BANK * CONTROLS
		* MAX_EEPROM_BANKS;
const int EEPROM_CC_BANK_START_POSITION=GLOBAL_CONFIG_EEPROM_START_POS+1;
const int EEPROM_REMOTE_BANK_POSITION = GLOBAL_CONFIG_EEPROM_START_POS
		+ CONTROLS;
const int EEPROM_CONTRAST_BANK_POSITION = EEPROM_REMOTE_BANK_POSITION + 1;

const int EEPROM_SAFE_MODE_POSITION=EEPROM_CONTRAST_BANK_POSITION+1;

const int EEPROM_Q_PEDAL_POSITION=EEPROM_SAFE_MODE_POSITION+1;
void EEPROMA2MCP::saveAll(boolean onOffControls[], int potPositions[],
	int potSwitchPositions[], int bank, int control) {
	int memPos = calculateEEPROMPosition(bank, control);
	long firstByte = 0;
	//guarda si se ignora 0 no 1 si
	bitWrite(firstByte, 0, 1);
	bitWrite(firstByte, 1, 0); //no usado
	for (int i = 0; i < ONOFF; i++) { //on off
		bitWrite(firstByte, i+2, onOffControls[i]);
	}

	//valor de 2 bytes switch1
	bitWrite(firstByte, 4, bitRead(potSwitchPositions[0],0));
	bitWrite(firstByte, 5, bitRead(potSwitchPositions[0],1));
	//valor de 2 bytes switch3
	bitWrite(firstByte, 6, bitRead(potSwitchPositions[1],0));
	bitWrite(firstByte, 7, bitRead(potSwitchPositions[1],1));
	//se guarda el primer byte
	EEPROM.update(memPos, firstByte); //update solo guarda si el valor es diferente al previo.
	memPos++;
	for (int i = 0; i < POTS; i++) {
		EEPROM.update(memPos + i,
				map(potPositions[i], 0, MAX_DEGREE, 0, MAX_BYTE_NUMBER));

	}
}
boolean EEPROMA2MCP::loadAll(boolean *onOffControls,int * potPositions,
		int *switchPotPositions, int bank, int control) {
	int memPos = calculateEEPROMPosition(bank, control);
	boolean ignore;

	long firstByte = EEPROM.read(memPos);
	int potByte;
	//1 no ignora 0 si ignora
	ignore = bitRead(firstByte, 0);
	for (int i = 0; i < ONOFF ; i++) { //on off
		*(onOffControls+i) = bitRead(firstByte, i+2);
	}
	*switchPotPositions=0;
	bitWrite(*switchPotPositions,0,bitRead(firstByte,4));
	bitWrite(*switchPotPositions,1,bitRead(firstByte,5));
	*(switchPotPositions+1)=0;
	bitWrite(*(switchPotPositions+1),0,bitRead(firstByte,6));
	bitWrite(*(switchPotPositions+1),1,bitRead(firstByte,7));

	memPos++;

	for (int i = 0; i < POTS; i++) {
		potByte=map(EEPROM.read(memPos + i), 0, MAX_BYTE_NUMBER, 0,
				MAX_DEGREE);

		*(potPositions+i)=potByte;
	}
	return ignore;
}
int EEPROMA2MCP::calculateEEPROMPosition(int bank, int control) {
	return BYTES_PER_BANK * ((bank * CONTROLS) + control);
}

void EEPROMA2MCP::ignore(boolean ignore, int bank, int control) {
	int eepromPos = calculateEEPROMPosition(bank, control);
	long firstByte = EEPROM.read(eepromPos);
	//guarda si se ignora 0 no 1 si
	bitWrite(firstByte, 0, !ignore);
	EEPROM.update(eepromPos, firstByte);
}
void EEPROMA2MCP::factoryReset() {
	for (int i = 0; i < GLOBAL_CONFIG_EEPROM_START_POS; i++) {
		EEPROM.update(i, 0);
	}
	saveContrast(DEFAULT_CONTRAST);
	saveRemote(REMOTE_DEFAULT_BANK);
	saveMidiStartBank(DEFAULT_START_BANK);
	saveQMidiPedalCC(DEFAULT__Q_PEDAL_BANK);
	for (int i = 0; i < CONTROLS-1; i++) {
		saveMidiControl(i, i+1);
	}
}
void EEPROMA2MCP::saveContrast(int value) {
	EEPROM.update(EEPROM_CONTRAST_BANK_POSITION, value);
}
void EEPROMA2MCP::saveRemote(int bank) {
	EEPROM.update(EEPROM_REMOTE_BANK_POSITION, bank);
}
int EEPROMA2MCP::loadContrast() {
	return EEPROM.read(EEPROM_CONTRAST_BANK_POSITION);
}
int EEPROMA2MCP::loadRemote() {
	return EEPROM.read(EEPROM_REMOTE_BANK_POSITION);
}

int EEPROMA2MCP::loadMidiStartBankPosition() {
	return EEPROM.read(GLOBAL_CONFIG_EEPROM_START_POS);
}
void EEPROMA2MCP::saveMidiStartBank(int pos) {
	EEPROM.update(GLOBAL_CONFIG_EEPROM_START_POS, pos);
}

int EEPROMA2MCP::loadMidiControl(int controlNumber) {
	return EEPROM.read(EEPROM_CC_BANK_START_POSITION + controlNumber);
}
void EEPROMA2MCP::saveMidiControl(int controlNumber, int midiNumber) {
	EEPROM.update(EEPROM_CC_BANK_START_POSITION + controlNumber, midiNumber);
}
void EEPROMA2MCP::saveSafeMode(boolean onOff) {
	long byteConfig=0;
	bitWrite(byteConfig, 0, onOff);
	EEPROM.update(EEPROM_SAFE_MODE_POSITION,byteConfig);
}

boolean EEPROMA2MCP::loadSafeMode(){
	long byteConfig=EEPROM.read(EEPROM_SAFE_MODE_POSITION);
	return bitRead(byteConfig,0);
}
int EEPROMA2MCP::loadQMidiPedalCC(){
	return EEPROM.read(EEPROM_Q_PEDAL_POSITION);
}
void EEPROMA2MCP::saveQMidiPedalCC(int ccNumber){
	EEPROM.update(EEPROM_Q_PEDAL_POSITION, ccNumber);
}



/*
	EEPROM_A2MCP libreria con funciones avanzadas.
	Created by Victor Manuel Camacho.
*/
#ifndef EEPROM_A2MCP
#define EEPROM_A2MCP

#include "Arduino.h"
class EEPROMA2MCP
{
  private:
	int calculateEEPROMPosition( int bank, int control);
  public:
	void saveAll(boolean onOffControls[],int potPositions[],int potSwitchPositions[],int bank,int control);
	boolean loadAll(boolean *controls, int *potPositions,int *switchPotPositions, int bank, int control);
	void ignore(boolean ignore,int bank,int control);
	void factoryReset();
	void saveContrast(int value);
	void saveRemote(int bank);
	int loadContrast();
	int loadRemote();
	int loadMidiStartBankPosition();
	void saveMidiStartBank(int pos);
	int loadMidiControl(int controlNumber);
	/*starting from 0
	 *
	 */
	void saveMidiControl(int controlNumber,int midiNumber);
	void saveQMidiPedalCC(int ccNumber);
	int loadQMidiPedalCC();
	void saveSafeMode(boolean onOff);
	boolean loadSafeMode();

};
static EEPROMA2MCP EEA2MCP;
#endif

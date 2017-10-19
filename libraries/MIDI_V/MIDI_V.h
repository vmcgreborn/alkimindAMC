/*
	MIDI V libreria para controlar MIDIs y comportamientos
	Created by Victor Manuel Camacho.
*/
#ifndef MIDI_V
#define MIDI_V

#include "Arduino.h"
#include "DEBUGGER_V.h"
#include "MIDI.h"
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
class MidiV
{
  private:
  Debugger *_debugger;
  static boolean _isReadCC;
  static boolean _isReadPC;
  static int _cc;
  static int _pc;
  static int _ccValue;
  static void readCCFunction(byte channel, byte number, byte value);
  static void readPCFunction(byte channel, byte number);
  public:
	MidiV(Debugger *debugger);
	void begin(int chanel);

	int getCC();
	int getPC();
	int getCCValue();
	void midiRead();
	boolean isReadCC();
	boolean isReadPC();
};

#endif

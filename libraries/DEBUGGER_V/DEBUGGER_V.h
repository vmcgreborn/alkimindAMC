/*
	DEBUGGER_V Libreria avanzada para debuggear con el serial port
	Created by Victor Manuel Camacho.
*/

#ifndef DEBUGGER_V
#define DEBUGGER_V
#include "ArduinoJson.h"
#include "Arduino.h"

class Debugger
{
  private:
	String _serialInput;
    boolean _debugg=false;
    boolean _debuggInput=false;
	int _pauseTime;
	long _timer=0;
	boolean showTime();
	HardwareSerial *_s_in;
	HardwareSerial *_s_out;
  public:
	Debugger(boolean debugg,boolean debuggInput,int serialOut,int serialIn,int pauseTime);
	void begin(unsigned int portOut,unsigned int portIn);
	void debuggPause(String val);
	void debuggPause(int val);
	void debuggPause(float val);
	void debuggln(String val);
	void debuggln(int val);
	void debuggln(float val);
	void debuggParam(String str,int val);
	void debuggParam(String str,float val);
	void debugg(String val);
	void debugg(int val);
	void debugg(float val);
	boolean readSerialInputString();
	void readJsonParam(int &value,String param);
	void cleanJson();
	void pauseStart();
	boolean isDebuggInput();
	boolean isDebuggOutput();
};

#endif

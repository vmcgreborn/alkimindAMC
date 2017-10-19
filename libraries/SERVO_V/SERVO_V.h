/*
	Servo v libreria con funciones avanzadas de servos, transformaciones,
	y multiconmutacion.
	Created by Victor Manuel Camacho.
*/
#ifndef SERVO_V
#define SERVO_V

#include "Arduino.h"
#include "Servo.h"
class ServoV
{
  private:


	String _chanelModes[2][3]={{"Clean","Fat","Crunch"},{"MKIIC+","MKIV","Extreme"}};
	Servo *_outputServoList[8];
	unsigned int _potValList[];
	unsigned int _switchValList[];
	unsigned int _conmutatePosition[3];
	unsigned int _servoOutputs=8;
	unsigned int _chanels=2;


  public:
	String _servoNames[9]={"Gain","Treble","Mid","Bass","Presence","Master","Reverb","MODE","Save/Exit"};
	ServoV();
	ServoV(String _servoNames[],String _chanelModes[][3],int chanels,int servoOutputs);
	void begin(uint8_t addr[2]);
	//aplica el ciclo de encendido
	void onSafeCicle();
	//aplica ciclo apagado
	void offSafeCicle();
	//rota las posiciones de los pots y swich
	void loadAllPositions(int potValList[],int switchPosValue,int chanel);
	//move pot number in position degrees
	void loadPotDegrees(int number,int chanel,int degrees);
	//move switch number in postion number
	void loadSwitchPosition(int number,int position);
	//chake pot number
	void chakePot(int number,int chanel,int actualPos);
	//obtiene el nombre del pot
	String getPotName(int number);
	//obtiene el nombre de la posicion del switch
	String getSwitchPositionName(int chanel,int pos);
  private:
	int getServoI2cInDegress(int degress,int calPos);
	void standbyOn(boolean on);
	void powerOn(boolean on);
};

#endif

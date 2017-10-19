/*
 Servo v libreria con funciones avanzadas de servos, transformaciones,
 con dos drivers para 16 servos cada uno.
 Created by Victor Manuel Camacho.
 */

#include "Arduino.h"
#include "SERVO_V.h"
#include "Servo.h"
#include <Adafruit_PWMServoDriver.h>


#define CHAKE_MOV 10
#define CHAKE_DELAY 100
#define SERVO_DELAY 100
#define SB_DELAY 60000
#define MAX_ROTATION 270
#define SERVO_MAX_ROTATION 145
#define MID_ROTATION MAX_ROTATION/2
#define SERVO_MID_ROTATION SERVO_MAX_ROTATION/2
//TODO determinar posiciones
#define POWER_ON_POS 30
#define POWER_OFF_POS 60
#define SB_ON_POS 30
#define SB_OFF_POS 60


#define GAIN_OUTPUT_NUMBER 0
#define VOLUMEN_OUTPUT_NUMBER 5
#define POWER_SB_OUTPUT_NUMBER 7
#define MODE_OUTPUT_NUMBER 6
#define CHANEL_CLEAN 0
#define CHANEL_DISTOR 1
#define DUAL_POTS_NUMBER 7
const int CHANEL_MODE_POSITION[] = { 20, 60, 120 }; //TODO determinar el valor
const int SERVO_I2C_CALIBRATION[][2]={

		//chanell 1
		{172,565},{172,565},//gain
		{172,565},{172,565},//mid
		{172,565},{172,565},//master
		{172,565},{172,565},//treble
		{172,565},{172,565},//bass
		{172,565},{172,565},//presence
		{172,565},{172,565},//Reverb
		{172,565},{172,565},//chanel1,chanel2
		//apartir de aqui entra en el siguiente i2c
		//pos16
		//chanel 2
		{172,565},{172,565},//gain
		{172,565},{172,565},//mid
		{172,565},{172,565},//master
		{172,565},{172,565},//treble
		{172,565},{172,565},//bass
		{172,565},{172,565},//presence
		{172,565},{172,565},//Reverb
		{172,565},{172,565},//on,standby
};

Adafruit_PWMServoDriver servosDriver1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servosDriver2 = Adafruit_PWMServoDriver(0x44);
ServoV::ServoV() {

}

ServoV::ServoV(String _servoNames[], String _chanelModes[][3], int chanels,
		int servoOutputs) {
//TODO
}
//TODO ya no se nesesita esta construccion
void ServoV::begin(uint8_t addr[2]) {
	servosDriver1 = Adafruit_PWMServoDriver(addr[0]);
	servosDriver1.begin();
	servosDriver1.setPWMFreq(60);//60
	servosDriver2 = Adafruit_PWMServoDriver(addr[1]);
	servosDriver2.begin();
	servosDriver2.setPWMFreq(60);
}
//aplica el ciclo de encendido

void ServoV::onSafeCicle() {

	int initialPotPos[] = { 0, MID_ROTATION, MID_ROTATION, MID_ROTATION,
					MID_ROTATION, 0,MID_ROTATION};//vol gain 0 los demas en medio y switch mode 0
	//ON CICLE
	//Standby OFF
	standbyOn(false);
	delay(SERVO_DELAY);
	//LEVEL CICLE
	loadAllPositions(initialPotPos,0,CHANEL_CLEAN);
	delay(SERVO_DELAY);
	loadAllPositions(initialPotPos,0, CHANEL_DISTOR);
	delay(SERVO_DELAY);
	//Encendido on
	powerOn(true);
	//Calentar bulvos
	delay(SB_DELAY);
	//standby ON
	standbyOn(true);
}
void ServoV::standbyOn(boolean on){
	int pos;
	if(on){
		pos=SB_ON_POS;
	}else{
		pos=SB_OFF_POS;
	}
	servosDriver2.setPWM(15,0,getServoI2cInDegress(pos,31));
}
void ServoV::powerOn(boolean on){
	int pos;
	if(on){
		pos=POWER_ON_POS;
	}else{
		pos=POWER_OFF_POS;
	}
	servosDriver2.setPWM(14,0,getServoI2cInDegress(pos,30));
}
void ServoV::offSafeCicle() {
	//SAFE LEVEL CICLE
	int initialPotPos[] = { 0, MID_ROTATION, MID_ROTATION, MID_ROTATION,
			MID_ROTATION, 0,
			MID_ROTATION
			};//vol gain 0 los demas en medio

	loadAllPositions(initialPotPos,0, CHANEL_CLEAN);
	delay(SERVO_DELAY);
	loadAllPositions(initialPotPos,0, CHANEL_DISTOR);
	delay(SERVO_DELAY);
	//standby off
	standbyOn(false);
	//enfriando bulbos
	delay(SB_DELAY);
	//TURN OFF
	powerOn(false);

}
//rota las posiciones de los pots y swich
void ServoV::loadAllPositions(int potValList[],int switchPosValue, int chanel) {
	loadSwitchPosition(chanel,switchPosValue);
	for (unsigned int i=0; i < DUAL_POTS_NUMBER;i++) {
		delay(SERVO_DELAY);
		loadPotDegrees(i,chanel,potValList[i]);
	}
}
//move pot number in position degrees
void ServoV::loadPotDegrees(int number, int chanel, int degrees) {
	int out2 = 0;
	int out1 = 0;
	int i2cPos=0;

	int startRotationServo1=(MAX_ROTATION-SERVO_MAX_ROTATION)/2;//62
	int endRotationServo1=startRotationServo1+SERVO_MAX_ROTATION;//207

	if (degrees > startRotationServo1&& degrees < endRotationServo1) {
			out1 = degrees - startRotationServo1;
			out2 = SERVO_MID_ROTATION;
	} else {
		if (degrees <= startRotationServo1) {
				out1 = 0;
				out2 = degrees +SERVO_MID_ROTATION -  startRotationServo1;//28 a 90
		} else {
				out1 = SERVO_MAX_ROTATION;
				out2 = degrees + SERVO_MID_ROTATION -endRotationServo1;//90 a 118
		}
	}

	 i2cPos=number*2;

		if(chanel==0){
			servosDriver1.setPWM(i2cPos,0,getServoI2cInDegress(out1,i2cPos));
			servosDriver1.setPWM(i2cPos+1,0,getServoI2cInDegress(out2,i2cPos+1));
		}

		if(chanel==1){
			servosDriver2.setPWM(i2cPos,0,getServoI2cInDegress(out1,i2cPos+16));
			servosDriver2.setPWM(i2cPos+1,0,getServoI2cInDegress(out2,i2cPos+17));
		}
}
//move switch number in postion number
void ServoV::loadSwitchPosition(int chanel, int position) {
	servosDriver1.setPWM(
						chanel+14,
						0,
						getServoI2cInDegress(CHANEL_MODE_POSITION[position],chanel+14)
						);
}
//chake pot number
void ServoV::chakePot(int number, int chanel, int actalPos) {
	int tempPos = actalPos;

	if (actalPos < CHAKE_MOV) {
		tempPos = CHAKE_MOV;
	}
	if (actalPos > MAX_ROTATION - CHAKE_MOV) {
		tempPos = MAX_ROTATION - CHAKE_MOV;
	}

	loadPotDegrees(number, chanel, tempPos - CHAKE_MOV);
	delay(CHAKE_DELAY);
	loadPotDegrees(number, chanel, tempPos + CHAKE_MOV);
	delay(CHAKE_DELAY);
	loadPotDegrees(number, chanel, actalPos + CHAKE_MOV);
}
//obtiene el nombre del pot
String ServoV::getPotName(int number) {
	return _servoNames[number];
}
//obtiene el nombre de la posicion del switch
String ServoV::getSwitchPositionName(int chanel, int pos) {

	return _chanelModes[chanel][pos];
}
/**
 * obtiene el valor para el i2c de grados
 */
int ServoV::getServoI2cInDegress(int degress,int calPos){
	return map(degress,0,180,SERVO_I2C_CALIBRATION[calPos][0],SERVO_I2C_CALIBRATION[calPos][1]);
}


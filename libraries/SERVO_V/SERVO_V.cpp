/*
 Servo v libreria con funciones avanzadas de servos, transformaciones,
 con dos drivers para 16 servos cada uno.
 Created by Victor Manuel Camacho.
 */

#include "Arduino.h"
#include "SERVO_V.h"
#include "Servo.h"
#include <Adafruit_PWMServoDriver.h>
/*
#include <Wire.h>
#include "PCA9685.h"
*/
#define CHAKE_MOV 10
#define CHAKE_DELAY 100
#define SERVO_DELAY 100
#define SB_DELAY 60000
#define MAX_ROTATION 270
#define MID_ROTATION MAX_ROTATION/2
//TODO determinar posiciones
#define POWER_ON_POS 30
#define POWER_OFF_POS 60
#define SB_ON_POS 30
#define SB_OFF_POS 60
#define PWM_FREQ 50

#define GAIN_OUTPUT_NUMBER 0
#define VOLUMEN_OUTPUT_NUMBER 5
#define POWER_SB_OUTPUT_NUMBER 7
#define MODE_OUTPUT_NUMBER 6
#define CHANEL_CLEAN 0
#define CHANEL_DISTOR 1
#define DUAL_POTS_NUMBER 7
#define I2C_MAX_RES 4096
const int CHANEL_MODE_POSITION[] = { 20, 60, 120 }; //TODO determinar el valor

const float SERVO_I2C_CALIBRATION_CONST=(PWM_FREQ/100)*I2C_MAX_RES;
const float SERVO_MS_MIN_SPEC=.5;//500us
const float SERVO_MS_MAX_SPEC=2.5;//2500us
const int SERVO_I2C_CALIBRATION[2]={static_cast<int>(SERVO_I2C_CALIBRATION_CONST*SERVO_MS_MIN_SPEC),static_cast<int>(SERVO_I2C_CALIBRATION_CONST*SERVO_MS_MAX_SPEC)};//106,622
//const int SERVO_I2C_CALIBRATION[2]={106,622};
Adafruit_PWMServoDriver servosDriver1 ;
Adafruit_PWMServoDriver servosDriver2 ;
/*
PCA9685 pwmController;
PCA9685 pwmController2;
*/
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
	servosDriver1.setPWMFreq(PWM_FREQ);//60
	servosDriver2 = Adafruit_PWMServoDriver(addr[1]);
	servosDriver2.begin();
	servosDriver2.setPWMFreq(PWM_FREQ);
/*

	 Wire.begin();                       // Wire must be started first
	 Wire.setClock(400000);              // Supported baud rates are 100kHz, 400kHz, and 1000kHz

	 pwmController.resetDevices();       // Software resets all PCA9685 devices on Wire line

	 pwmController.init(B000000);        // Address pins A5-A0 set to B000000
	 pwmController.setPWMFrequency(PWM_FREQ);

	 pwmController2.resetDevices();       // Software resets all PCA9685 devices on Wire line

	 pwmController2.init(B000001);
	 pwmController2.setPWMFrequency(PWM_FREQ);

*/
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
	servosDriver2.setPWM(1,0,getServoI2cInDegress(pos));
	//pwmController2.setChannelPWM(1,getServoI2cInDegress(pos));
}
void ServoV::powerOn(boolean on){
	int pos;
	if(on){
		pos=POWER_ON_POS;
	}else{
		pos=POWER_OFF_POS;
	}
	servosDriver2.setPWM(0,0,getServoI2cInDegress(pos));
	//pwmController2.setChannelPWM(0,getServoI2cInDegress(pos));
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
		servosDriver1.setPWM(number+(chanel*7),0,getServoI2cInDegress(degrees));
		//pwmController.setChannelPWM(number+(chanel*7),getServoI2cInDegress(degrees));
}
//move switch number in postion number
void ServoV::loadSwitchPosition(int chanel, int position) {
	servosDriver1.setPWM(
						chanel+14,
						0,
						getServoI2cInDegress(CHANEL_MODE_POSITION[position])
						);
	/*

	pwmController.setChannelPWM(
			chanel+14,
			getServoI2cInDegress(CHANEL_MODE_POSITION[position]));*/
}
//chake pot number
void ServoV::chakePot(int number, int chanel, int actalPos) {
	/*int tempPos = actalPos;

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
	*/
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
int ServoV::getServoI2cInDegress(int degress){
	return map(degress,0,MAX_ROTATION,SERVO_I2C_CALIBRATION[0],SERVO_I2C_CALIBRATION[1]);
}


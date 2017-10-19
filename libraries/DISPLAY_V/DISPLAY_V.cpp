/*
 Display v libreria con funciones avanzadas
 Created by Victor Manuel Camacho.
 */

#include "Arduino.h"
#include "DISPLAY_V.h"
#include "LiquidCrystal.h"
#define DEFAULT_CONTRAST 50
#define MAX_CONTRAST 100
#define MAX_PWM_OUTPUT 160
#define ANIMATION_DELAY 60
#define COLS 16
#define YES "YES"
#define NO "CANCEL"
#define YESNO "  CANCEL   YES  "

byte CHAR_ARROW_FORDWARD=B01111110;

byte ALLKIMIND_1[8] = {
B00000,
B00011,
B00100,
B01000,
B01000,
B10000,
B10000,
B10000,
};

byte ALLKIMIND_2[8] = {
B11111,
B00000,
B11110,
B11111,
B11001,
B11001,
B11001,
B11001,
};

byte ALLKIMIND_3[8] = {
B00000,
B11000,
B00100,
B00010,
B00010,
B00001,
B00001,
B00001,
};
byte ALLKIMIND_4[8] = {
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B00100,
B00110,
};
byte ALLKIMIND_5[8] = {
B10111,
B10100,
B10110,
B01011,
B01001,
B00100,
B00011,
B00000,
};
byte ALLKIMIND_6[8] = {
B11111,
B11001,
B11001,
B11001,
B11001,
B00000,
B00000,
B11111,
};
byte ALLKIMIND_7[8] = {
B11111,
B00001,
B00001,
B10010,
B10010,
B00100,
B11000,
B00000,
};
byte ALLKIMIND_8[8] = {
B11111,
B00110,
B00100,
B00000,
B00000,
B00000,
B00000,
B00000
};


	DisplayV::DisplayV(LiquidCrystal *lcd){

		lcd->createChar(0,ALLKIMIND_1);
		lcd->createChar(1,ALLKIMIND_2);
		lcd->createChar(2,ALLKIMIND_3);
		lcd->createChar(3,ALLKIMIND_4);
		lcd->createChar(4,ALLKIMIND_5);
		lcd->createChar(5,ALLKIMIND_6);
		lcd->createChar(6,ALLKIMIND_7);
		lcd->createChar(7,ALLKIMIND_8);

		_lcd=lcd;
	}
	void DisplayV::beginContrast(int contrastPin){
		pinMode(contrastPin, OUTPUT);
		 _contrastPin=contrastPin;
		 analogWrite(_contrastPin,map(DEFAULT_CONTRAST,0,MAX_CONTRAST,0,MAX_PWM_OUTPUT));
	}
	void DisplayV::setContrast(int value){
		analogWrite(_contrastPin,map(value,0,MAX_CONTRAST,0,MAX_PWM_OUTPUT));
	}
	LiquidCrystal  DisplayV::getLcd(){
		return *_lcd;
	}
	void DisplayV::printAnimateLeft(String text,String text2){
		getLcd().clear();
		getLcd().setCursor(COLS,0);
		getLcd().print(text);
		getLcd().setCursor(COLS,1);
		getLcd().print(text2);
		for(int i=0;i<COLS;i++){
			getLcd().scrollDisplayLeft();
			delay(ANIMATION_DELAY);
		}

	}
	void DisplayV::printAnimateRight(String text,String text2){
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().print(text);
		getLcd().setCursor(0,1);
		getLcd().print(text2);
		for(int i=0;i<COLS;i++){
			getLcd().scrollDisplayLeft();
		}
		for(int i=0;i<COLS;i++){
			getLcd().scrollDisplayRight();
			delay(ANIMATION_DELAY);
		}
	}
	void DisplayV::setMenu(String items[],int size){
		_menuSize=size;
			for(int i=0;i<size;i++){
				 _menu[i]=items[i];
			}
	}
	void DisplayV::setMenuAndShow(String items[],int size,boolean left){

		setMenu(items,size);
		if(left){
			printAnimateLeft(_menu[0],_menu[1]);
		}else{
			printAnimateRight(_menu[0],_menu[1]);
		}

		getLcd().clear();
		_menuPosition=0;
		getLcd().setCursor(0,0);
		getLcd().write(CHAR_ARROW_FORDWARD);
		getLcd().print(_menu[0]);
		getLcd().setCursor(1,1);
		getLcd().print(_menu[1]);
	}

	void DisplayV::yesNoMenu(String legend){
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().print(legend);
		selectNo();
		getLcd().setCursor(2,1);
		getLcd().write(NO);
		getLcd().setCursor(11,1);
		getLcd().write(YES);
	}
	void DisplayV::selectYes(){
		getLcd().setCursor(1,1);//CANCEL
		getLcd().print(" ");
		getLcd().setCursor(10,1);//YES
		getLcd().write(CHAR_ARROW_FORDWARD);
	}
	void DisplayV::selectNo(){
		getLcd().setCursor(10,1);//YES
		getLcd().print(" ");
		getLcd().setCursor(1,1);//CANCEL
		getLcd().write(CHAR_ARROW_FORDWARD);
	}
	void DisplayV::menuCursorUp(){
		getLcd().clear();
		getLcd().setCursor(0,1);
		getLcd().print(_menu[_menuPosition]);
		if(_menuPosition==0){
			_menuPosition=_menuSize;
		}
		_menuPosition--;
		getLcd().setCursor(0,0);
		getLcd().write(CHAR_ARROW_FORDWARD);
		getLcd().print(_menu[_menuPosition]);
	}
	void DisplayV::setMenuPositionAndShow(int position){
		int secondPos=0;
		_menuPosition=position;
		getLcd().clear();
		if(_menuPosition+1<_menuSize){
			secondPos=_menuPosition+1;
		}
		printAnimateRight(_menu[position],_menu[secondPos]);
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().write(CHAR_ARROW_FORDWARD);
		getLcd().print(_menu[_menuPosition]);
		getLcd().setCursor(0,1);
		getLcd().print(_menu[secondPos]);
	}
	void DisplayV::setMenuPosition(int position){
		_menuPosition=position;
	}
	void DisplayV::menuCursorDown(){
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().print(_menu[_menuPosition]);
		_menuPosition++;
		if(_menuPosition==_menuSize){
			_menuPosition=0;
		}
		getLcd().setCursor(0,1);
		getLcd().write(CHAR_ARROW_FORDWARD);
		getLcd().print(_menu[_menuPosition]);
	}

	void DisplayV::showOnAnimation(){
		getLcd().clear();
		getLcd().setCursor(COLS,0);
		getLcd().write(byte(0));
		getLcd().write(byte(1));
		getLcd().write(byte(2));
		getLcd().write(byte(3));
		getLcd().print("LLKIMIND.");
		getLcd().setCursor(COLS,1);
		getLcd().write(byte(4));
		getLcd().write(byte(5));
		getLcd().write(byte(6));
		getLcd().write(byte(7));
		getLcd().print("2MCP V:1.0");
		for(int i=0;i<COLS;i++){
			getLcd().scrollDisplayLeft();
			delay(ANIMATION_DELAY);
		}

	}
	void DisplayV::showOffAnimation(){
		getLcd().clear();
		printAnimateLeft("SHUTING DOWN...","  PLEASE WAIT");
	}
	void DisplayV::displayModParam(String title,int val){
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().print(title);
		getLcd().setCursor(5,1);
		getLcd().print("[");
		getLcd().print(val);
		getLcd().setCursor(9,1);
		getLcd().print("]");
	}
	void DisplayV::displayModParam(String title,String val){
		getLcd().clear();
		getLcd().setCursor(0,0);
		getLcd().print(title);
		getLcd().setCursor(0,1);
		getLcd().print("[");
		getLcd().print(val);
		getLcd().print("]");
	}
	int DisplayV::getMenuPosition(){
		return _menuPosition;
	}
	int DisplayV::getMenuSize(){
		return _menuSize;
	}

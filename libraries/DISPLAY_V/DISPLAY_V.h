/*
	Display v libreria con funciones avanzadas de pantalla.
	Created by Victor Manuel Camacho.
*/
#ifndef DISPLAY_V
#define DISPLAY_V

#include "Arduino.h"
#include "LiquidCrystal.h"
class DisplayV
{
  private:
	int _contrastPin=0;
	int _menuPosition=0;
	String _menu[20];
	int _menuSize=0;
	LiquidCrystal *_lcd;
  public:
	DisplayV(LiquidCrystal *lcd);
	void beginContrast(int contrastPin);
	void setContrast(int value);
	LiquidCrystal  getLcd();
	void printAnimateLeft(String text,String text2);
	void printAnimateRight(String text,String text2);
	void setMenu(String items[],int size);
	void setMenuAndShow(String items[],int size,boolean left);
	void yesNoMenu(String legend);
	void selectYes();
	void selectNo();
	void menuCursorUp();
	void menuCursorDown();
	void showOnAnimation();
	void showOffAnimation();
	void displayModParam(String title,int val);
	void displayModParam(String title,String val);
	int getMenuPosition();
	void setMenuPositionAndShow(int position);
	void setMenuPosition(int position);
	int getMenuSize();
};

#endif

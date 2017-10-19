/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <DISPLAY_V.h>

#include "Arduino.h"
#include <POT_V.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
PotV pot(270,A0,A1);
DisplayV disp(&lcd);
int contrast =50;
int aument=1;
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
   disp.beginContrast(8);
   disp.showOnAnimation();
   delay(3000);
  disp.displayModParam("****Contrast****",contrast);
  pinMode(7,  INPUT_PULLUP);
  

}

void loop() {
  
  int lastContrast=contrast;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  
  
  if(!digitalRead(7)){
    contrast=0;
  }
  pot.encoderMap(0,100,contrast);
  /*delay(100);
  if(contrast==101){
    aument=-1;
  }
    if(contrast==-1){
    aument=1;
  }
  contrast=contrast+aument;*/
  if(lastContrast!=contrast){
       disp.displayModParam("****Contrast****",contrast);
  }
  disp.setContrast(contrast);
}

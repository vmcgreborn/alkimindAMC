#include <LED_V.h>
#include <DEBUGGER_V.h>
#include <EEPROM_A2MCP.h>
#include <PUSH_BUTTON.h>
#include <SERVO_V.h>
#include <POT_V.h>
#include <LiquidCrystal.h>
#include <DISPLAY_V.h>
#include <MIDI_V.h>
#define DEBUGG_OUTPUT true
#define DEBUGG_INPUT false
#define SERIAL_OUTPUT 0
#define SERIAL_INPUT 1
#define DEBUGG_PAUSE_TIME 2000
#define SERIAL_PORT 9600
#define MAX_POT_ANGLE 260//EPROM SERVO
#define LCD_COLS 16
#define LCD_ROWS 2
#define MIN_REMOTE_BANK 0
#define MAX_REMOTE_BANK 200
#define MIN_PC_BANK 0
#define MAX_PC_BANK 99
//menus
#define MAIN_MENU_SIZE 5
#define CONFIG_MENU_SIZE 8
#define CC_MENU_SIZE 5
//delays
#define SAVE_BLINK_DELAY 2000
#define MESSAGE_DELAY 3000
//Inputs 12 13 1
#define PUSH_A 6
#define PUSH_B 5
#define PUSH_S A2
#define ENCODER_A A1
#define ENCODER_B A0
#define LCD_DB_4 22
#define LCD_DB_5 23
#define LCD_DB_6 24
#define LCD_DB_7 25
#define LCD_CONTRAST 8
#define LCD_RS 9
#define LCD_E 10
#define ON_INPUT 7
//Outputs
#define LED_A 4
#define LED_B 3
#define LED_S 2

#define TOTAL_POTS 8
#define TOTAL_CONTROLS 2
#define TOTAL_SWITCH 2
#define MODE_POSITION 3
#define SWITCH_POSITIONS 3
#define QUICK_SAVE_PUSH_TIME 2000



uint8_t SERVO_I2C_ADRESS[2]={0x40,0x41};
//menus
String MAIN_MENU[MAIN_MENU_SIZE]={"Manual Set","Remote Set","Configuration","Active/ignore","Exit"};
String CONFIG_MENU[CONFIG_MENU_SIZE]={"Contrast","Remote Bank","Start Bank PC","Controls CC","Quick Pedal CC","Safe Cicle","Factory Reset","Save and Exit"};
String CC_MENU[CC_MENU_SIZE]={"CTL 1","CTL 2","CTL 3","CTL 4","EXIT"};
//CC NAMES
String CC_NAMES[]={"N/A","CTL 1","CTL 2","CTL 3","CTL 4"};

//Current Pots banks switch controls
boolean currentNoIgnore=true;
boolean currentRelayControls[2];
int currentPots[7];
int currentSwitchs[2];
int currentBank =-1;
int currentCC =0;
//configuration
int currentContrast;
int currentRemoteBank;
int currentStarBankPC;
int currentControlsCC[4];
int currentQPedalCC;
boolean safeCicle=false;

Debugger logger(DEBUGG_OUTPUT,DEBUGG_INPUT,SERIAL_OUTPUT,SERIAL_INPUT,DEBUGG_PAUSE_TIME);
PushButton pushA;
PushButton pushB;
PushButton pushS;
PushButton latchOnOff;
PotV pot(MAX_POT_ANGLE,ENCODER_A,ENCODER_B);
Led ledA;
Led ledB;
Led ledS;
LiquidCrystal mainLcd(LCD_RS,LCD_E,LCD_DB_4,LCD_DB_5,LCD_DB_6,LCD_DB_7);
DisplayV dv(&mainLcd);
ServoV servos;
MidiV midiV(&logger);
void setup() {
  logger.begin(SERIAL_PORT,SERIAL_PORT);
  logger.debuggln("Begin...");
  pushA.begin(PUSH_A);
  pushB.begin(PUSH_B);
  pushS.begin(PUSH_S);
  latchOnOff.begin(ON_INPUT);
  ledA.begin(LED_A);
  ledB.begin(LED_B);
  ledS.begin(LED_S);
  mainLcd.begin(LCD_COLS,LCD_ROWS);
  dv.beginContrast(LCD_CONTRAST);
  servos.begin(SERVO_I2C_ADRESS);
  midiV.begin(MIDI_CHANNEL_OMNI);
  pot.setLastState();
  //Start
  logger.debuggln("Initializing...");
  dv.showOnAnimation();
  delay(MESSAGE_DELAY);
  logger.debuggln("ATX ON...");
  while(!latchOnOff.isMomentary()){
    ledS.off();
    //Modo Espera
  }
  loadConfig();
  logger.debuggln("Config loaded...");
  if(safeCicle){
    logger.debuggln("On safe cicle...");
    dv.printAnimateLeft("***Safe Cicle***","Please Wait 1min");
    servos.onSafeCicle();
    logger.debuggln("On safe cicle end...");
    logger.debuggln("END startup");
  }
  logger.debuggln("Setup End...");
}
boolean _mainExit;
void loop() {
  pot.detach();
  ledS.on();
  _mainExit=false;
  listenMode();
}

void loadAllServosControls(){
  currentNoIgnore=EEA2MCP.loadAll(currentRelayControls,currentPots,currentSwitchs,currentBank,currentCC);
  if(currentNoIgnore){
    ledA.booleanWrite(currentRelayControls[0]);
    ledB.booleanWrite(currentRelayControls[1]);
    logger.debuggln("loading servos...");
    servos.loadAllPositions(currentPots,currentSwitchs[currentRelayControls[0]],currentRelayControls[0]);

  }
}

boolean continueIn(boolean exit){
  if(exit||_mainExit){
    return false;
  }
  return true;
}

void offMode(){
  if(!latchOnOff.isMomentary()){
    logger.debuggln("Shooting down");
    dv.showOffAnimation();
    delay(MESSAGE_DELAY);
    logger.debuggln("off cicle start");
    if(safeCicle){
      dv.printAnimateLeft("*OFF Safe Cicle*","Please Wait 1min");
      servos.offSafeCicle();
    }
    dv.getLcd().clear();
    logger.debuggln("ofcicle end");
    while(!latchOnOff.isMomentary()){
      ledS.off();
      //MODO ESPERA
    }
    dv.showOnAnimation();
    delay(MESSAGE_DELAY);
    if(safeCicle){
      dv.printAnimateLeft("***Safe Cicle***","Please Wait 1min");
      servos.onSafeCicle();
    }
    _mainExit=true;
  }
}


void listenMode(){
  listenModePrint(currentBank,currentCC,currentNoIgnore);
  boolean exit=false;
  pot.detach();
  while(continueIn(exit)){
    //do menu mode operations and exit whith true
    offMode();
    if(pushS.isOnePulse()){
      menuMode();
    }
    //lectura de midi o debugg input
    listenModeMidiInputRead();
    if(pushA.isOnePulse()){
      currentRelayControls[0]=!currentRelayControls[0];
      if(currentRelayControls[0]){
        ledA.on();
      }else{
        ledA.off();
      }
      servos.loadSwitchPosition(currentRelayControls[0],currentSwitchs[currentRelayControls[0]]);
    }
    if(pushB.isOnePulse()){
      currentRelayControls[1]=!currentRelayControls[1];
      if(currentRelayControls[1]){
        ledB.on();
      }else{
        ledB.off();
      }
    }

  }

}

void listenModeMidiInputRead(){
  int inputRead=false;
  int lastBank=currentBank;
  int lastCC=currentCC;
  int lastNoIgnore=currentNoIgnore;
  boolean foundcc=false;
  //input
  midiV.midiRead();
  if(midiV.isReadPC()){
    logger.debuggln("ReadingPC...");
    logger.debuggParam("pc:",midiV.getPC());
    if(midiV.getPC()!=currentRemoteBank){
      currentBank=midiV.getPC();
      currentBank=currentBank-currentStarBankPC;
      //limitar bank
      if(currentBank+1>100||currentBank<0){
        currentBank=lastBank;
      }
      inputRead=true;
      if(lastBank!=currentBank||currentBank==-1){
        currentCC=0;
      }
    }else{
      remoteSetMenu();
    }
  }else{
    if(midiV.isReadCC()&&midiV.getCCValue()>0){
      logger.debuggln("ReadingCC...");
      logger.debuggParam("cc:",midiV.getCC());
      logger.debuggParam("ccValue:",midiV.getCCValue());
      currentCC=midiV.getCC();
      for(int i=0;i<4;i++){
          if(currentControlsCC[i]==currentCC){
            currentCC=i+1;
            foundcc=true;
            break;
          }
        }
        //Si se presiona el mismo regresar a N/A
        if(currentCC==lastCC){
          currentCC=0;
        }
        //si no corresponde obtener el anterior
        if(!foundcc){
          currentCC=lastCC;
        }else{
          inputRead=true;
        }
    }
  }
  if(inputRead){
    inputRead=false;
    loadAllServosControls();
    listenModePrint(currentBank,currentCC,currentNoIgnore);
    if(!currentNoIgnore){
      currentBank=lastBank;
      currentCC=lastCC;
      currentNoIgnore=true;
      listenModePrint(currentBank,currentCC,currentNoIgnore);
      currentNoIgnore=lastNoIgnore;
    }
  }

  //end input
}

void listenModePrint(int bank,int cc,boolean noIgnore){
  String bankString="Bank: ";
  String controlString="Control: ";
  if(bank==-1){
    bankString.concat("N/A");
  }else{
    bankString.concat(bank+1);
  }

  controlString.concat(CC_NAMES[cc]);
  dv.printAnimateLeft(bankString,controlString);
  delay(500);
  if(!noIgnore){
    dv.printAnimateLeft("*****Ignored****","");
  }
  delay(500);

}
void menuMode(){
  int menuPos;
  dv.setMenuAndShow(MAIN_MENU,MAIN_MENU_SIZE,true);
  boolean exit=false;
  pot.detach();
  while(continueIn(exit)){
    offMode();
    //do menu mode operations and exit whith true
    menuComunControls(false);
    if(pushS.isOnePulse()){
      menuPos=dv.getMenuPosition();
      switch(menuPos){
      case 0:
        manualSetMenu();
        break;
      case 1:
        remoteSetMenu();
        break;
      case 2:
        configuration();
        break;
      case 3:
        ignoreMenu();
        break;
      case 4:
        exit=true;
        _mainExit=true;
        break;
      }
    }
  }
}

void menuComunControls(boolean quickSave){

  int menuPos=0;
  if (pushS.isHoldOnPulse(QUICK_SAVE_PUSH_TIME)&&quickSave) {
    save();
  }
  if(pushB.isOnePulse()){
    dv.menuCursorUp();
  }
  if(pushB.isMomentaryIntermitentOn()){
    dv.menuCursorUp();
  }
  if(pushA.isMomentaryIntermitentOn()){
    dv.menuCursorDown();
  }

  if(pushA.isOnePulse()){
    dv.menuCursorDown();
  }
  if(pot.encoderMap(-1,1,menuPos,1,false)){
    if(menuPos>0){
      dv.menuCursorUp();
    }
    if(menuPos<0){
      dv.menuCursorDown();
    }
  }
}


void ignoreMenu(){
  int displayBank=currentBank;
  int displayCC=currentCC;
  boolean exit=false;
  selectPcCCMessage();
  listenModePrint(displayBank,displayCC,true);
  pot.detach();
  String ignoreMessage;
  while(continueIn(exit)){
    offMode();
    simpleMidiInputRead(displayBank,displayCC);
    if(pushS.isOnePulse()){
      if(displayBank==-1){
        selectPcCCMessage();
        listenModePrint(displayBank,displayCC,true);
      }else{
        if(displayBank==currentBank&&displayCC==currentCC){
          EEA2MCP.ignore(true,displayBank,displayCC);
          ignoreMessage="Ignore and saved";
        }else{
          EEA2MCP.ignore(false,displayBank,displayCC);
          ignoreMessage="Active and saved";
        }
        linealdisplayBankCC(ignoreMessage,displayBank,displayCC);
        exit=true;
        dv.setMenuPositionAndShow(3);
      }
    }
  }
}

void manualSetMenu(){
  int potNumber;
  boolean exit=false;
  dv.setMenuAndShow(servos._servoNames,TOTAL_POTS+1,true);
  pot.detach();
  while(continueIn(exit)){
    offMode();
    menuComunControls(true);
    if(pushS.isOnePulse()){
      potNumber=dv.getMenuPosition();
      if(TOTAL_POTS==potNumber){//Exit
        save();
      }else{
        setParameterManualSubMenu(potNumber);
      }
    }
  }
}


void setParameterManualSubMenu(int potNumber){
  boolean exit=false;
  boolean isSwitch=false;
  int memoryPosAdjust=0;
  int chanel=currentRelayControls[0];
  String title=servos.getPotName(potNumber);
  pot.detach();
  dv.printAnimateLeft(title,"");
  if(potNumber>MODE_POSITION){
    memoryPosAdjust=1;
  }
  if(potNumber!=MODE_POSITION){
    servos.chakePot(potNumber,chanel,currentPots[potNumber-memoryPosAdjust]);
    dv.displayModParam(title,map(currentPots[potNumber-memoryPosAdjust],0,MAX_POT_ANGLE,0,100));
  }else{
    dv.displayModParam(title,
        servos.getSwitchPositionName(chanel,currentSwitchs[chanel])
    );
    isSwitch=true;
  }

  while(continueIn(exit)){
    offMode();
    if(isSwitch){
      if(pot.encoderMap(0,SWITCH_POSITIONS-1,currentSwitchs[chanel])){
        dv.displayModParam(
            title,
            servos.getSwitchPositionName(chanel,currentSwitchs[chanel])
        );
        servos.loadSwitchPosition(chanel,currentSwitchs[chanel]);
      }
    }else{
      if(pot.encoderDegree(currentPots[potNumber-memoryPosAdjust])){
        dv.displayModParam(title,map(currentPots[potNumber-memoryPosAdjust],0,MAX_POT_ANGLE,0,100));
        servos.loadPotDegrees(potNumber,chanel,currentPots[potNumber-memoryPosAdjust]);
      }
    }

    if(pushS.isOnePulse()){
      exit=true;
      dv.setMenuPositionAndShow(potNumber);
    }
  }
}

void remoteSetMenu(){
  int increment=0;
  String remoteMenu[11];
  remoteMenu[0]="Amp controls";
  for(int i=1;i<TOTAL_POTS+2;i++){
    remoteMenu[i]=servos._servoNames[i-1];
  }
  remoteMenu[TOTAL_POTS+1]="Exit";
  remoteMenu[TOTAL_POTS+2]="Save";
  boolean exit=false;
  dv.printAnimateLeft("Remote mode use","midi pedal board");
  delay(1000);
  dv.setMenuAndShow(remoteMenu,TOTAL_POTS+3,true);
  pot.detach();
  while(continueIn(exit)){
    offMode();
    remoteControls();
    if(pushS.isOnePulse()){
      _mainExit=true;
    }
    if(midiV.isReadPC()&&midiV.getPC()==currentRemoteBank){
      switch (dv.getMenuPosition()){
      case 0:
        //Amp Control
        ampControl();
        break;
      case 4:
        remoteParams(currentSwitchs[currentRelayControls[0]],remoteMenu[4],0,false);
        break;
      case 9:
        _mainExit=true;
        break;
      case 10:
        if(currentBank!=-1){
          EEA2MCP.saveAll(currentRelayControls,currentPots,currentSwitchs,currentBank,currentCC);
          linealdisplayBankCC("Saved...",currentBank,currentCC);

        }else{
          dv.printAnimateLeft("first load a","Bank or and CC");
          delay(1000);
        }
        _mainExit=true;
        break;
      default:
        if(dv.getMenuPosition()>4){
          increment=1;
        }else{
          increment=0;
        }
        remoteParams(currentPots[dv.getMenuPosition()-1-increment],remoteMenu[dv.getMenuPosition()],dv.getMenuPosition()-1,true);
        break;
      }
      if(!_mainExit){
        dv.setMenuPositionAndShow(dv.getMenuPosition());
        pot.detach();
      }
    }
  }
}
void remoteParams(int &value,String title,int potNumber,boolean isPot){
  boolean exit=false;
  int chanel=currentRelayControls[0];
  pot.detach();
  if(isPot){
    dv.displayModParam(title,map(value,0,MAX_POT_ANGLE,0,100));
    servos.chakePot(potNumber,chanel,value);
  }else{
    dv.displayModParam(title,servos.getSwitchPositionName(chanel,currentSwitchs[chanel]));
  }
  while(continueIn(exit)){
    midiV.midiRead();
    if(midiV.isReadPC()){
      if(midiV.getPC()==currentRemoteBank){
        exit=true;
      }
    }
    if(midiV.isReadCC()&&midiV.getCC() == currentQPedalCC
        && pot.potMidiChange(midiV.getCCValue())){
      if(isPot){
        value=pot.quickMidiDegree(value,midiV.getCCValue());
        dv.displayModParam(title,map(value,0,MAX_POT_ANGLE,0,100));
        servos.loadPotDegrees(potNumber,chanel,value);
      }else{
        value=pot.quickMidiPosition(2,0,value,midiV.getCCValue());
        dv.displayModParam(title,servos.getSwitchPositionName(chanel,currentSwitchs[chanel]));
        servos.loadSwitchPosition(chanel,value);
      }
    }
  }
}

void ampControl(){
  boolean exit=false;
  pot.detach();
  dv.printAnimateLeft("Press Ctr1 or","Ctr2 for ON/OFF");
  while(continueIn(exit)){
    midiV.midiRead();
    if(midiV.isReadPC()&&midiV.getPC()==currentRemoteBank){
      exit=true;
    }
    if(midiV.isReadCC()){
      if(midiV.getCC()==currentControlsCC[0]&&midiV.getCCValue()>0){
        currentRelayControls[0]=!currentRelayControls[0];
        if(currentRelayControls[0]){
          dv.printAnimateLeft("Amp control A:","ON");
          ledA.on();
        }else{
          dv.printAnimateLeft("Amp control A:","OFF");
          ledA.off();
        }
      }

      if(midiV.getCC()==currentControlsCC[1]&&midiV.getCCValue()>0){
        currentRelayControls[1]=!currentRelayControls[1];
        if(currentRelayControls[1]){
          ledB.on();
          dv.printAnimateLeft("Amp control B:","ON");
        }else{
          ledB.off();
          dv.printAnimateLeft("Amp control B:","OFF");
        }
      }

    }
  }
}
void remoteControls(){
  int quickPos;
  midiV.midiRead();
  if(midiV.isReadCC()){
    logger.debuggParam("cc Value",midiV.getCCValue());

    if(midiV.getCC()==currentControlsCC[0]&&midiV.getCCValue()>0){
      dv.menuCursorUp();
    }

    if(midiV.getCC()==currentControlsCC[1]&&midiV.getCCValue()>0){
      dv.menuCursorDown();
    }
  }

  if (midiV.getCC() == currentQPedalCC&& pot.potMidiChange(midiV.getCCValue())) {
    quickPos=pot.quickMidiPosition(dv.getMenuSize()-1,0,dv.getMenuPosition(),midiV.getCCValue());
    while(quickPos!=dv.getMenuPosition()){
      if(quickPos>dv.getMenuPosition()){
        dv.menuCursorDown();
      }else{
        dv.menuCursorUp();
      }
    }
  }
}
void factoryReset(){
  EEA2MCP.factoryReset();
  loadConfig();
  dv.printAnimateLeft("  Factory Reset ","****************");
  ledS.delayBlinkOff(SAVE_BLINK_DELAY);
  ledS.on();
}
void configuration(){
  boolean exit=false;
  dv.setMenuAndShow(CONFIG_MENU,CONFIG_MENU_SIZE,true);
  pot.detach();
  while(continueIn(exit)){
    offMode();
    menuComunControls(false);
    if(pushS.isOnePulse()){
      switch(dv.getMenuPosition()){
      case 0:
        contrastConfig();
        break;
      case 1:
        currentRemoteBank=setComunParameter(CONFIG_MENU[dv.getMenuPosition()],MAX_REMOTE_BANK,MIN_REMOTE_BANK,currentRemoteBank);
        break;
      case 2:
        currentStarBankPC=setComunParameter(CONFIG_MENU[dv.getMenuPosition()],MAX_PC_BANK,MIN_PC_BANK,currentStarBankPC);
        break;
      case 3:
        //CC
        ccConfig();
        dv.setMenuPosition(3);
        break;
      case 4:
        currentQPedalCC=setComunParameter(CONFIG_MENU[dv.getMenuPosition()],100,1,currentQPedalCC);
        break;
      case 5:
        //ON OF SAFE
        safeCicle=!safeCicle;
        if(safeCicle){
          dv.printAnimateLeft(" Safe Cicle ON","Please Wait 1min");
          ledS.off();
          servos.onSafeCicle();
          ledS.on();
        }else{
          dv.printAnimateLeft(" Safe Cicle OFF","****************");
          ledS.delayBlinkOff(SAVE_BLINK_DELAY);
          ledS.on();
        }
        break;
      case 6:
        //factory
        factoryReset();
        break;
      case 7:
        exit=true;
        break;
      }
      if(!exit&&!_mainExit){
        dv.setMenuPositionAndShow(dv.getMenuPosition());
      }
    }
  }
  if(!_mainExit){
    //saveconfig
    eepromSaveConfig();
    //pinta menu anterior
    dv.setMenu(MAIN_MENU,MAIN_MENU_SIZE);
    dv.setMenuPositionAndShow(2);
    //en la posicion que se quedo
  }
}
void loadConfig(){
  currentContrast=EEA2MCP.loadContrast();
  currentStarBankPC=EEA2MCP.loadMidiStartBankPosition();
  currentRemoteBank=EEA2MCP.loadRemote();
  safeCicle=EEA2MCP.loadSafeMode();
  currentQPedalCC=EEA2MCP.loadQMidiPedalCC();
  for(int i=0;i<4;i++){
    currentControlsCC[i]=EEA2MCP.loadMidiControl(i);
  }
}

void ccConfig(){
  int posicion=0;
  int lastCCValue;
  boolean exist=false;
  boolean exit=false;
  dv.setMenuAndShow(CC_MENU,CC_MENU_SIZE,true);
  pot.detach();
  while(continueIn(exit)){
    offMode();
    menuComunControls(false);

    if(pushS.isOnePulse()){
      posicion=dv.getMenuPosition();
      if(posicion!=4){
        lastCCValue=currentControlsCC[posicion];
        currentControlsCC[posicion]=setComunParameter(CC_MENU[posicion],100,1,currentControlsCC[posicion]);
        for(int i=0;i<4;i++){
          if(i!=posicion&&currentControlsCC[i]==currentControlsCC[posicion]){
            exist=true;
            break;
          }
        }
        if(exist){
          exist=false;
          String mess="Error: CC: ";
          mess.concat(currentControlsCC[posicion]);
          dv.printAnimateLeft(mess,"Is alredy asigned");
          delay(1000);
          currentControlsCC[posicion]=lastCCValue;
        }
        dv.setMenuPositionAndShow(posicion);
      }else{
        exit=true;
        dv.setMenu(CONFIG_MENU,CONFIG_MENU_SIZE);
      }
    }
  }
}


void contrastConfig(){
  boolean exit=false;
  pot.detach();
  dv.displayModParam("Contrast",currentContrast);
  while(continueIn(exit)){
    offMode();
    if(pot.potChange()){
      pot.encoderMap(0,100,currentContrast);
      dv.displayModParam("Contrast",currentContrast);
      dv.setContrast(currentContrast);
    }

    if(pushS.isOnePulse()){
      exit=true;
    }
  }
}

int setComunParameter(String title,int maxValue,int minValue,int value){
  boolean exit=false;
  pot.detach();
  dv.displayModParam(title,value);
  while(continueIn(exit)){
    offMode();
    if(pot.potChange()){
      pot.encoderMap(minValue,maxValue,value);
      dv.displayModParam(title,value);
    }
    if(pushS.isOnePulse()){
      exit=true;
    }
  }
  return value;
}

void eepromSaveConfig(){
  EEA2MCP.saveContrast(currentContrast);
  EEA2MCP.saveMidiStartBank(currentStarBankPC);
  EEA2MCP.saveRemote(currentRemoteBank);
  EEA2MCP.saveSafeMode(safeCicle);
  for(int i=0;i<4;i++){
    EEA2MCP.saveMidiControl(i,currentControlsCC[i]);
  }
  dv.printAnimateLeft("  Config Saved","****************");
  ledS.delayBlinkOff(SAVE_BLINK_DELAY);
  ledS.on();
}

void save(){
  boolean exit=false;
  boolean isSave=false;
  dv.yesNoMenu("save");
  pot.detach();
  //when is caled from S hold this avoid s is pushed until is relased
  while(pushS.isMomentary()){
    //cicle until relased
  }
  pushS.isOnePulse();
  ///
  //save loop
  while(continueIn(exit)){
    offMode();
    saveControls(isSave);
  }
}
void saveControls(boolean &isSave){
  if(pushA.isOnePulse()){
    dv.selectNo();
    isSave=false;
  }
  if(pushB.isOnePulse()){
    dv.selectYes();
    isSave=true;
  }
  if(pushS.isOnePulse()){
    if(isSave){
      bankControlSave();
    }
    _mainExit=true;
  }
}

void bankControlSave(){
  boolean exit=false;
  int displayBank=currentBank;
  int displayCC=currentCC;
  selectPcCCMessage();
  listenModePrint(currentBank,currentCC,true);
  pot.detach();
  while(continueIn(exit)){
    offMode();
    simpleMidiInputRead(displayBank,displayCC);
    if(pushS.isOnePulse()){
      if(displayBank==-1){
        selectPcCCMessage();
        listenModePrint(displayBank,displayCC,true);
      }else{
        EEA2MCP.saveAll(currentRelayControls,currentPots,currentSwitchs,displayBank,displayCC);
        currentBank=displayBank;
        currentCC=displayCC;
        linealdisplayBankCC("Saved...",displayBank,displayCC);
        exit=true;
      }
    }
  }
}
void linealdisplayBankCC(String message,int bank,int cc){
  String output="Bank:";
  output.concat(bank+1);
  output.concat(" CC:");
  output.concat(CC_NAMES[cc]);
  dv.printAnimateLeft(output,message);
  ledS.delayBlinkOff(SAVE_BLINK_DELAY);
  ledS.on();
}
void selectPcCCMessage(){
  dv.printAnimateLeft("Please push a ","PC/CC from board");
  delay(1000);
}

void simpleMidiInputRead(int &bank,int &cc){
  int inputRead=false;
  int lastBank=bank;
  int lastCC=cc;
  boolean foundcc=false;
  //input
  midiV.midiRead();
  if(midiV.isReadPC()){
    logger.debuggParam("pc:",midiV.getPC());
    bank=midiV.getPC();
    bank=bank-currentStarBankPC;
    //limitar bank
    if(bank+1>100||bank<0){
      bank=lastBank;
    }
    if(lastBank!=bank||bank==-1){
      cc=0;
    }
    inputRead=true;
  }else{
    if(midiV.isReadCC()&&midiV.getCCValue()>0){
      logger.debuggParam("cc:",midiV.getCC());
      logger.debuggParam("ccValue:",midiV.getCCValue());
      cc=midiV.getCC();
      for(int i=0;i<4;i++){
            if(currentControlsCC[i]==cc){
              cc=i+1;
              foundcc=true;
              inputRead=true;
              break;
            }
          }
          //Si se presiona el mismo regresar a N/A
          if(cc==lastCC){
            cc=0;
          }
          //si no corresponde obtener el anterior
          if(!foundcc){
            cc=lastCC;
          }

    }
  }
  if(inputRead){
    inputRead=false;
    listenModePrint(bank,cc,true);
  }
}

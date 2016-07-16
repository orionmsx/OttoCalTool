//***************************
//* Otto's Calibration Tool
//* (c) Rafael López Verdejo
//* rlopezverdejo@gmail.com
//* GPL license
//* 16/07/2016
//***************************

#include <Servo.h>
#include <Oscillator.h>
#include <EEPROM.h>
#include <SerialUI.h>

#define N_SERVOS 4 // # servos

#define PIN_RA 5 // Right ankle
#define PIN_LA 4 // Left ankle
#define PIN_RH 3 // Right hip
#define PIN_LH 2 // Left hip

// MENU CONFIGURATION
SUI_DeclareString(device_greeting, "+++ Welcome to the Otto's calibration tool +++\r\n+++ by Rafa López [rlopezverdejo@gmail.com] +++\r\nWrite ? for help.");
SUI_DeclareString(top_menu_title, "Main menu");

SUI_DeclareString(ti_key, "ti");
SUI_DeclareString(ti_help, "Ajuste del tobillo izquierdo");
SUI_DeclareString(tiplus_key, "+");
SUI_DeclareString(tiplus_help, "Ajuste positivo del tobillo izquierdo");
SUI_DeclareString(timinus_key, "-");
SUI_DeclareString(timinus_help, "Ajuste negativo del tobillo izquierdo");

SUI_DeclareString(td_key, "td");
SUI_DeclareString(td_help, "Ajuste del tobillo derecho");
SUI_DeclareString(tdplus_key, "+");
SUI_DeclareString(tdplus_help, "Ajuste positivo del tobillo derecho");
SUI_DeclareString(tdminus_key, "-");
SUI_DeclareString(tdminus_help, "Ajuste negativo del tobillo derecho");

SUI_DeclareString(ci_key, "ci");
SUI_DeclareString(ci_help, "Ajuste de la cadera izquierda");
SUI_DeclareString(ciplus_key, "+");
SUI_DeclareString(ciplus_help, "Ajuste positivo de la cadera izquierda");
SUI_DeclareString(ciminus_key, "-");
SUI_DeclareString(ciminus_help, "Ajuste negativo de la cadera izquierda");

SUI_DeclareString(cd_key, "cd");
SUI_DeclareString(cd_help, "Ajuste de la cadera derecha");
SUI_DeclareString(cdplus_key, "+");
SUI_DeclareString(cdplus_help, "Ajuste positivo de la cadera derecha");
SUI_DeclareString(cdminus_key, "-");
SUI_DeclareString(cdminus_help, "Ajuste negativo de la cadera derecha");

SUI_DeclareString(info_key, "info");
SUI_DeclareString(info_help, "Muestra los valores de calibración en memoria");

SUI_DeclareString(save_key, "save");
SUI_DeclareString(save_help, "Guarda los valores de calibración en la EEPROM");

SUI_DeclareString(load_key, "load");
SUI_DeclareString(load_help, "Carga los valores de calibración desde la EEPROM");

SUI_DeclareString(reset_key, "reset");
SUI_DeclareString(reset_help, "Inicializa los valores de calibración en memoria");

SUI::SerialUI mySUI = SUI::SerialUI(device_greeting);

Oscillator servo[N_SERVOS];

int cal_RA = 0;
int cal_LA = 0;
int cal_RH = 0;
int cal_LH = 0;

void setup()
{
  mySUI.begin(9600);
  mySUI.setMaxIdleMs(60000);

  servo[0].attach(PIN_RA);
  servo[1].attach(PIN_LA);
  servo[2].attach(PIN_RH);
  servo[3].attach(PIN_LH);

  servo[0].SetTrim(cal_RA);
  servo[1].SetTrim(cal_LA);
  servo[2].SetTrim(cal_RH);
  servo[3].SetTrim(cal_LH);

  SUI::Menu * mainMenu = mySUI.topLevelMenu();
  mainMenu->setName(top_menu_title);
  SUI::Menu * tiMenu = mainMenu->subMenu(ti_key, ti_help);
  tiMenu->addCommand(tiplus_key, CB_leftAnklePlus, tiplus_help);
  tiMenu->addCommand(timinus_key, CB_leftAnkleMinus, timinus_help);
  SUI::Menu * tdMenu = mainMenu->subMenu(td_key, td_help);
  tdMenu->addCommand(tdplus_key, CB_rightAnklePlus, tdplus_help);
  tdMenu->addCommand(tdminus_key, CB_rightAnkleMinus, tdminus_help);
  SUI::Menu * ciMenu = mainMenu->subMenu(ci_key, ci_help);
  ciMenu->addCommand(ciplus_key, CB_leftHipPlus, ciplus_help);
  ciMenu->addCommand(ciminus_key, CB_leftHipMinus, ciminus_help);
  SUI::Menu * cdMenu = mainMenu->subMenu(cd_key, cd_help);
  cdMenu->addCommand(cdplus_key, CB_rightHipPlus, cdplus_help);
  cdMenu->addCommand(cdminus_key, CB_rightHipMinus, cdminus_help);
  mainMenu->addCommand(info_key, CB_info, info_help);
  mainMenu->addCommand(save_key, CB_save, save_help);
  mainMenu->addCommand(load_key, CB_load, load_help);
  mainMenu->addCommand(reset_key, CB_reset, reset_help);
}

void loop()
{
  if (mySUI.checkForUser(150)) {
    mySUI.enter();

    while (mySUI.userPresent())
    {
      mySUI.handleRequests();
    }
  }
}

/************ CALLBACKS *************/

void CB_leftAnklePlus()
{
  setServoTrim(1, ++cal_LA);
}

void CB_leftAnkleMinus()
{
  setServoTrim(1, --cal_LA);
}

void CB_rightAnklePlus()
{
  setServoTrim(0, ++cal_RA);
}

void CB_rightAnkleMinus()
{
  setServoTrim(0, --cal_RA);
}

void CB_leftHipPlus()
{
  setServoTrim(3, ++cal_LH);
}

void CB_leftHipMinus()
{
  setServoTrim(3, --cal_LH);
}

void CB_rightHipPlus()
{
  setServoTrim(2, ++cal_RH);
}

void CB_rightHipMinus()
{
  setServoTrim(2, --cal_RH);
}

void CB_info()
{
  showTrims();
}

void CB_save()
{
  // SAVE EEPROM VALUES

  EEPROM.write(0, cal_RA);
  EEPROM.write(1, cal_LA);
  EEPROM.write(2, cal_RH);
  EEPROM.write(3, cal_LH);

  mySUI.println("Calibración grabada");
}

void CB_load()
{
  int trim;

  // LOAD EEPROM VALUES

  for (int x = 0; x < N_SERVOS; x++)
  {
    trim = EEPROM.read(x);
    if (trim > 128) trim = trim - 256;
    if (x == 0) {
      cal_RA = trim;
    }
    else if (x == 1) {
      cal_LA = trim;
    }
    else if (x == 2) {
      cal_RH = trim;
    }
    else if (x == 3) {
      cal_LH = trim;
    }
    servo[x].SetTrim(trim);
  }

  showTrims();
  centerServos();
}

void CB_reset()
{
  // RESET MEMORY VALUES

  cal_RA = 0;
  cal_LA = 0;
  cal_RH = 0;
  cal_LH = 0;

  servo[0].SetTrim(cal_RA);
  servo[1].SetTrim(cal_LA);
  servo[2].SetTrim(cal_RH);
  servo[3].SetTrim(cal_LH);

  showTrims();
  centerServos();
}


/************ FUNCTIONS *************/

void centerServos()
{
  for (int i = 0; i < N_SERVOS; i++) servo[i].SetPosition(90);
  delay(200);
}

void setServoTrim(int theServo, int theTrim)
{
  servo[theServo].SetTrim(theTrim);
  servo[theServo].SetPosition(90);
}

void showTrims()
{
  mySUI.print("Right ankle / Tobillo derecho: ");
  mySUI.println(cal_RA);
  mySUI.print("Left ankle / Tobillo izquierdo: ");
  mySUI.println(cal_LA);
  mySUI.print("Right hip / Cadera derecha: ");
  mySUI.println(cal_RH);
  mySUI.print("Left hip / Cadera izquierda: ");
  mySUI.println(cal_LH);
}


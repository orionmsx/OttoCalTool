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
SUI_DeclareString(device_greeting, "+++ Welcome to the Otto's calibration tool +++\r\n+++ by Rafael Lopez Verdejo [rlopezverdejo@gmail.com] +++\r\nWrite ? for help.");
SUI_DeclareString(top_menu_title, "Main menu");

SUI_DeclareString(la_key, "la");
SUI_DeclareString(la_help, "Left ankle trim");
SUI_DeclareString(la_plus_key, "+");
SUI_DeclareString(la_plus_help, "Left ankle trim +1");
SUI_DeclareString(la_minus_key, "-");
SUI_DeclareString(la_minus_help, "Left ankle trim -1");

SUI_DeclareString(ra_key, "ra");
SUI_DeclareString(ra_help, "Right ankle trim");
SUI_DeclareString(ra_plus_key, "+");
SUI_DeclareString(ra_plus_help, "Right ankle trim +1");
SUI_DeclareString(ra_minus_key, "-");
SUI_DeclareString(ra_minus_help, "Right ankle trim -1");

SUI_DeclareString(lh_key, "lh");
SUI_DeclareString(lh_help, "Left hip trim");
SUI_DeclareString(lh_plus_key, "+");
SUI_DeclareString(lh_plus_help, "Left hip trim +1");
SUI_DeclareString(lh_minus_key, "-");
SUI_DeclareString(lh_minus_help, "Left hip trim -1");

SUI_DeclareString(rh_key, "rh");
SUI_DeclareString(rh_help, "Right hip trim");
SUI_DeclareString(rh_plus_key, "+");
SUI_DeclareString(rh_plus_help, "Right hip trim +1");
SUI_DeclareString(rh_minus_key, "-");
SUI_DeclareString(rh_minus_help, "Right hip trim -1");

SUI_DeclareString(info_key, "info");
SUI_DeclareString(info_help, "Show the trim values loaded into memory");

SUI_DeclareString(save_key, "save");
SUI_DeclareString(save_help, "Save memory trim values to EEPROM");

SUI_DeclareString(load_key, "load");
SUI_DeclareString(load_help, "Load memory trim values from EEPROM");

SUI_DeclareString(reset_key, "reset");
SUI_DeclareString(reset_help, "Reset memory trim values");

SUI::SerialUI mySUI = SUI::SerialUI(device_greeting);

Oscillator servo[N_SERVOS];

int cal_RA = 0;
int cal_LA = 0;
int cal_RH = 0;
int cal_LH = 0;

int eepromBaseAddress = 0;

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
  SUI::Menu * laMenu = mainMenu->subMenu(la_key, la_help);
  laMenu->addCommand(la_plus_key, CB_leftAnklePlus, la_plus_help);
  laMenu->addCommand(la_minus_key, CB_leftAnkleMinus, la_minus_help);
  SUI::Menu * raMenu = mainMenu->subMenu(ra_key, ra_help);
  raMenu->addCommand(ra_plus_key, CB_rightAnklePlus, ra_plus_help);
  raMenu->addCommand(ra_minus_key, CB_rightAnkleMinus, ra_minus_help);
  SUI::Menu * lhMenu = mainMenu->subMenu(lh_key, lh_help);
  lhMenu->addCommand(lh_plus_key, CB_leftHipPlus, lh_plus_help);
  lhMenu->addCommand(lh_minus_key, CB_leftHipMinus, lh_minus_help);
  SUI::Menu * rhMenu = mainMenu->subMenu(rh_key, rh_help);
  rhMenu->addCommand(rh_plus_key, CB_rightHipPlus, rh_plus_help);
  rhMenu->addCommand(rh_minus_key, CB_rightHipMinus, rh_minus_help);
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

  EEPROM.write(eepromBaseAddress + 0, cal_RA);
  EEPROM.write(eepromBaseAddress + 1, cal_LA);
  EEPROM.write(eepromBaseAddress + 2, cal_RH);
  EEPROM.write(eepromBaseAddress + 3, cal_LH);

  mySUI.println("Trim values saved");
}

void CB_load()
{
  int trim;

  // LOAD EEPROM VALUES

  for (int x = eepromBaseAddress; x < (eepromBaseAddress + N_SERVOS); x++)
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
  mySUI.print("Right ankle: ");
  mySUI.println(cal_RA);
  mySUI.print("Left ankle: ");
  mySUI.println(cal_LA);
  mySUI.print("Right hip: ");
  mySUI.println(cal_RH);
  mySUI.print("Left hip: ");
  mySUI.println(cal_LH);
}


/***************************************************
 *
 *  Baja Vehicle Siren
 *
 *  Created by Alexander Huegler based on the
 *  "GetStarted.cpp" example from the 
 *  "DFRobotDFPlayerMini.h" library. 2026.
 * 
 ****************************************************/

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>

#define FPSerial Serial1

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// Button pin assignment
int buttonSkip = 25;
int buttonBack = 26;
int buttonPauseOrPlay = 27;

// Button "time since program start at last press" variables
unsigned long buttonSkip_Timer = millis();
unsigned long buttonBack_Timer = millis();
unsigned long buttonPauseOrPlay_Timer = millis();

// Status variables
bool playerIsPaused = false;

// Function declarations
void checkPlayerButtons();

void setup()
{

  // Initialize button pins
  pinMode(buttonSkip, INPUT_PULLUP);
  pinMode(buttonBack, INPUT_PULLUP);
  pinMode(buttonPauseOrPlay, INPUT_PULLUP);

  // Initialize serial connection to DFPlayer
  int rx_pin = 16;
  int tx_pin = 17;
  FPSerial.begin(9600, SERIAL_8N1, rx_pin, tx_pin);

  // Initialize serial
  Serial.begin(115200);

  // Startup messages
  Serial.println();
  Serial.println(F("Starting Baja Vehicle Siren, pass the aux."));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
  { // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(15); // Set volume value. From 0 to 30
  myDFPlayer.playFolder(1, 1);    // Play SD:/01/01.mp3
  myDFPlayer.loopFolder(1);
}

void loop()
{

  // Check if buttons have been pressed and
  checkPlayerButtons();
}

// Function for printing error/status messages from the DFPlayer
void printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError:"));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}


// Function for checking if buttons have been pressed and performing the required actions
void checkPlayerButtons()
{
  bool buttonSkip_Pressed = false;
  bool buttonBack_Pressed = false;
  bool buttonPauseOrPlay_Pressed = false;

  unsigned long timerThreshold = 400;
  unsigned long timerCurrent;

  // Detect if buttons pressed
  buttonSkip_Pressed = !digitalRead(buttonSkip);
  buttonBack_Pressed = !digitalRead(buttonBack);
  buttonPauseOrPlay_Pressed = !digitalRead(buttonPauseOrPlay);

  // Skip button logic
  timerCurrent = millis();
  if (buttonSkip_Pressed && ((timerCurrent - buttonSkip_Timer) > timerThreshold))
  {
    myDFPlayer.next();
    Serial.println("Skipping the current song.");
    buttonSkip_Timer = millis();
  }

  // Back button logic
  timerCurrent = millis();
  if (buttonBack_Pressed && ((timerCurrent - buttonBack_Timer) > timerThreshold))
  {
    myDFPlayer.previous();
    Serial.println("Playing the previous song.");
    buttonBack_Timer = millis();
  }

  // Pause/Play button logic
  timerCurrent = millis();
  if (buttonPauseOrPlay_Pressed && ((timerCurrent - buttonPauseOrPlay_Timer) > timerThreshold))
  {
    if (!playerIsPaused)
    {
      myDFPlayer.pause();
      playerIsPaused = true;
      Serial.println("Pausing playback.");
    }
    else if (playerIsPaused)
    {
      myDFPlayer.start();
      playerIsPaused = false;
      Serial.println("Unpausing playback.");
    }
    buttonPauseOrPlay_Timer = millis();
  }
}
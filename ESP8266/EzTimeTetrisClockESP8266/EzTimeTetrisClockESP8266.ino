/*******************************************************************
    Tetris clock that fetches its time Using the EzTimeLibrary

    NOTE: THIS IS CURRENTLY CRASHING!

    For use with an ESP8266
 *                                                                 *
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP32 set up
// ----------------------------

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "secret.h"

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------




// Adafruit GFX library is a dependancy for the PxMatrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library

#include <TetrisMatrixDraw.h>
// This library :)
// https://github.com/toblum/TetrisAnimation

#include <ezTime.h>
// Library used for getting the time and adjusting for DST
// Search for "ezTime" in the Arduino Library manager
// https://github.com/ropg/ezTime





Ticker display_ticker;
Ticker timer_ticker;

TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

Timezone myTZ;
unsigned long oneSecondLoopDue = 0;

bool showColon = true;
bool finishedAnimating = false;
bool displayIntro = true;
bool animateFlag = false;

String lastDisplayedTime = "";
String lastDisplayedAmPm = "";

// This method is needed for driving the display
void display_updater() {
  display.display(70);
}

void setAnimateFlag() {
  animateFlag = true;
}

// This method is for controlling the tetris library draw calls
void animationHandler()
{
  // Not clearing the display and redrawing it when you
  // dont need to improves how the refresh rate appears
  if (!finishedAnimating) {
    display.clearDisplay();
    if (displayIntro) {
      finishedAnimating = tetris.drawText(1, 21);
    } else {
      if (twelveHourFormat) {
        // Place holders for checking are any of the tetris objects
        // currently still animating.
        bool tetris1Done = false;
        bool tetris2Done = false;
        bool tetris3Done = false;

        tetris1Done = tetris.drawNumbers(-6, 26, showColon);
        tetris2Done = tetris2.drawText(56, 25);

        // Only draw the top letter once the bottom letter is finished.
        if (tetris2Done) {
          tetris3Done = tetris3.drawText(56, 15);
        }

        finishedAnimating = tetris1Done && tetris2Done && tetris3Done;

      } else {
        finishedAnimating = tetris.drawNumbers(2, 26, showColon);
      }
    }

  }
}

void drawIntro(int x = 0, int y = 0)
{
  tetris.drawChar("W", x, y, tetris.tetrisCYAN);
  tetris.drawChar("r", x + 5, y, tetris.tetrisMAGENTA);
  tetris.drawChar("i", x + 11, y, tetris.tetrisYELLOW);
  tetris.drawChar("t", x + 17, y, tetris.tetrisGREEN);
  tetris.drawChar("t", x + 22, y, tetris.tetrisBLUE);
  tetris.drawChar("e", x + 27, y, tetris.tetrisRED);
  tetris.drawChar("n", x + 32, y, tetris.tetrisWHITE);
  tetris.drawChar(" ", x + 37, y, tetris.tetrisMAGENTA);
  tetris.drawChar("b", x + 42, y, tetris.tetrisYELLOW);
  tetris.drawChar("y", x + 47, y, tetris.tetrisGREEN);
}

void drawConnecting(int x = 0, int y = 0)
{
  tetris.drawChar("C", x, y, tetris.tetrisCYAN);
  tetris.drawChar("o", x + 5, y, tetris.tetrisMAGENTA);
  tetris.drawChar("n", x + 11, y, tetris.tetrisYELLOW);
  tetris.drawChar("n", x + 17, y, tetris.tetrisGREEN);
  tetris.drawChar("e", x + 22, y, tetris.tetrisBLUE);
  tetris.drawChar("c", x + 27, y, tetris.tetrisRED);
  tetris.drawChar("t", x + 32, y, tetris.tetrisWHITE);
  tetris.drawChar("i", x + 37, y, tetris.tetrisMAGENTA);
  tetris.drawChar("n", x + 42, y, tetris.tetrisYELLOW);
  tetris.drawChar("g", x + 47, y, tetris.tetrisGREEN);
}

void setup() {
  Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Do not set up display before WiFi connection
  // as it will crash!

  // Intialise display library
  display.begin(16);
  if(driver=="FM6126A") display.setDriverChip(FM6126A);
  display.clearDisplay();

  // Setup ticker for driving display
  display_ticker.attach(0.002, display_updater);
  yield();
  display.clearDisplay();

  // "connecting"
  drawConnecting(5, 10);

  // Setup EZ Time
  setDebug(INFO);
  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(F(MYTIMEZONE));
  Serial.print(F("Time in your set timezone:         "));
  Serial.println(myTZ.dateTime());

  display.clearDisplay();
   //"Powered By"
  drawIntro(6, 12);
  delay(2000);

  // Start the Animation Timer
  tetris.setText("B. LOUGH");
  timer_ticker.attach(0.1, animationHandler);

  // Wait for the animation to finish
  while (!finishedAnimating)
  {
    delay(10); //waiting for intro to finish
  }
  delay(2000);
  //timer_ticker.attach(0.1, setAnimateFlag);
  finishedAnimating = false;
  displayIntro = false;
  tetris.scale = 2;
}

void setMatrixTime() {
  String timeString = "";
  String AmPmString = "";
  if (twelveHourFormat) {
    // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
    // Check the EZTime Github page for info on
    // time formatting
    timeString = myTZ.dateTime("g:i");

    //If the length is only 4, pad it with
    // a space at the beginning
    if (timeString.length() == 4) {
      timeString = " " + timeString;
    }

    //Get if its "AM" or "PM"
    AmPmString = myTZ.dateTime("A");
    if (lastDisplayedAmPm != AmPmString) {
      Serial.println(AmPmString);
      lastDisplayedAmPm = AmPmString;
      // Second character is always "M"
      // so need to parse it out
      tetris2.setText("M", forceRefresh);

      // Parse out first letter of String
      tetris3.setText(AmPmString.substring(0, 1), forceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    Serial.println(timeString);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, forceRefresh);

    // Must set this to false so animation knows
    // to start again
    finishedAnimating = false;
  }
}


void loop() {
  unsigned long now = millis();
//  if(false && animateFlag){
//    animateFlag = false;
//    animationHandler();
//  }

 // animationHandler();
  if (now > oneSecondLoopDue) {
    // We can call this often, but it will only
    // update when it needs to
    setMatrixTime();
    showColon = !showColon;
    oneSecondLoopDue = now + 10000;
  }
}

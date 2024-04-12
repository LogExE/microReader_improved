#include "ReaderModeTime.h"

#include <ESP8266WiFi.h>
#include <GyverNTP.h>

extern ReaderEquipment eq;
extern ReaderSettings sets;

extern uint32_t batTimer;

String timeModeStr = "WATCHES MODE";
GyverNTP gntp;

void drawGNTPDateTime();

void RMTimeStart() {
  eq.oled.clear();
  eq.oled.home();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(sets.staSSID, sets.staPass);
  eq.oled.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    eq.oled.print(".");
    eq.oled.update();
    delay(500);
  }

  gntp.setGMT(sets.gmt);
  gntp.begin();
}

void RMTimeTick() {
  int mi = millis();
  if (mi - batTimer > STATUSBAR_TIME) {
    drawStatus(timeModeStr);
    batTimer = mi;
  }
  gntp.tick();
  if (gntp.second() % 5 == 0) {
    drawGNTPDateTime();
  }
}

void RMTimeSuspend() {
  gntp.end();
  WiFi.mode(WIFI_OFF);
}

void drawGNTPDateTime() {
  eq.oled.clear(0, 16, 127, 63);
  eq.oled.setCursor(0, 2);

  eq.oled.println(gntp.dateString());
  eq.oled.println(gntp.timeString());
  eq.oled.update();
}

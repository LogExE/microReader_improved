
#pragma once

#include <GyverPortal.h>                      // Либа веб морды
#include <GyverOLED.h>                        // Либа олед-дисплея
#include <EncButton.h>                        // Либа кнопок

#define UP_BTN_PIN        14                  // Номер GPIO для кнопки ВВЕРХ         
#define OK_BTN_PIN        12                  // Номер GPIO для кнопки ОК
#define DWN_BTN_PIN       13                  // Номер GPIO для кнопки ВНИЗ

struct ReaderEquipment
{
  GyverPortal ui;
  GyverOLED<SSD1306_128x64> oled;
  ButtonT <UP_BTN_PIN> up;
  ButtonT <OK_BTN_PIN> ok;
  ButtonT <DWN_BTN_PIN> down;
};

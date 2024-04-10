#include "ReaderCommon.h"

extern ReaderEquipment eq;

extern int batMV;

void drawStatus(String str) {
  eq.oled.clear(0, 0, 127, 15);
  eq.oled.home();

  eq.oled.print(str);

  batMV = ESP.getVcc();
  
  byte charge =  
      constrain( 
          map(   
              batMV, VBAT_EMPTY_MV,
              VBAT_FULL_MV, 
              0, 12
              ),
          0, 12
      );

  eq.oled.setCursorXY(110, 0);       // Положение на экране
  eq.oled.drawByte(0b00111100);      // Пипка
  eq.oled.drawByte(0b00111100);      // 2 штуки
  eq.oled.drawByte(0b11111111);      // Передняя стенка
  for (uint8_t i = 0; i < 12; i++) { // 12 градаций
    if (i < 12 - charge) {
      eq.oled.drawByte(0b10000001);
    } 
    else {
      eq.oled.drawByte(0b11111111);
    }
  }
  eq.oled.drawByte(0b11111111); // Задняя стенка

  eq.oled.line(0, 10, 127, 10);
  eq.oled.update(0, 0, 127, 15);
}

int translateContrast(byte x) { return map(x, 10, 100, 1, 255); }

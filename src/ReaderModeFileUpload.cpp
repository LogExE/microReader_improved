#include "ReaderModeFileUpload.h"

#include <ESP8266WiFi.h>
#include <LittleFS.h>

extern ReaderEquipment eq;
extern ReaderSettings sets;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FILE_UPLOAD("file_upl");     // кнопка загрузки
  GP.FOLDER_UPLOAD("folder_upl"); // кнопка загрузки
  GP.FILE_MANAGER(&LittleFS);     // файловый менеджер

  GP.BUILD_END();
}

/*
void action(GyverPortal& p) {       // Подсос значений со страницы
  if (p.form("/cfg")) {             // Если есть сабмит формы - копируем все в переменные
    p.copyStr("apSsid", sets.apSsid);
    p.copyStr("apPass", sets.apPass);
    p.copyStr("staSsid", sets.staSsid);
    p.copyStr("staPass", sets.staPass);
    p.copyBool("staEn", sets.staModeEn);

    byte con = map(sets.dispContrast, 10, 100, 1, 255);
    oled.setContrast(con);         // Тут же задаем яркость оледа
    EEPROM.put(1, sets);           // Сохраняем все настройки в EEPROM
    EEPROM.commit();               // Записываем
  }
}
*/

void RMFileUploadStart() {
  eq.oled.clear();
  eq.oled.home();
  eq.oled.print("SSID: ");
  eq.oled.println(sets.staSSID);
  eq.oled.update();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(sets.staSSID, sets.staPass);

  eq.oled.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    eq.oled.print(".");
    eq.oled.update();
  }
  
  eq.oled.println();
  eq.oled.print("IP: ");
  eq.oled.println(WiFi.localIP());
  eq.oled.update();

  eq.ui.attachBuild(build);
  // ui.attach(action);
  eq.ui.start();
}

void RMFileUploadTick() {
  eq.ui.tick();
}

void RMFileUploadSuspend() {
  WiFi.mode(WIFI_OFF);
}

#include "ReaderModeFileUpload.h"

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <EEPROM.h>

extern ReaderEquipment eq;
extern ReaderSettings sets;

extern uint32_t batTimer;

String upModeStr = "UPLOAD MODE";

// конструктор страницы
void build() {
  GP.BUILD_END();

  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("Wi-Fi Reader");    // Обзываем титл
  GP.FORM_BEGIN("/cfg");            // Начало формы
  GP.GRID_RESPONSIVE(600);          // Отключение респонза при узком экране
  M_BLOCK(                          // Общий блок-колонка для WiFi
    GP.SUBMIT("SUBMIT SETTINGS");   // Кнопка отправки формы
    M_BLOCK_TAB(                    // Конфиг для AP режима -> текстбоксы (логин + пароль)
      "AP-Mode",                    // Имя + тип DIV
      GP.TEXT("apSSID", "Login", sets.apSSID, "", 20);
      GP.BREAK();
      GP.TEXT("apPass", "Password", sets.apPass, "", 20);
      GP.BREAK();
    );
    M_BLOCK_TAB(                    
      "STA-Mode",                   
      GP.TEXT("staSSID", "Login", sets.staSSID, "", 20);
      GP.BREAK();
      GP.TEXT("staPass", "Password", sets.staPass, "", 20);
      GP.BREAK();
      M_BOX(GP_CENTER, GP.LABEL("STA Enable"); GP.SWITCH("staModeFlag", sets.staModeFlag););
    );
    GP.FORM_END();                  // <- Конец формы (костыль)
    M_BLOCK_TAB(                    // Блок с OTA-апдейтом
      "ESP UPDATE",                 // Имя + тип DIV
      GP.OTA_FIRMWARE();            // Кнопка с OTA начинкой
    );
    M_BLOCK_TAB(                    // Блок с файловым менеджером
      "FILE MANAGER",               // Имя + тип DIV
      GP.FILE_UPLOAD("file_upl");   // Кнопка для загрузки файла
      GP.FILE_MANAGER(&LittleFS);   // Файловый менеджер
    );
  );
  GP.BUILD_END();                   // Конец билда страницы
}

void action(GyverPortal& p) {       // Подсос значений со страницы
  if (p.form("/cfg")) {             // Если есть сабмит формы - копируем все в переменные
    p.copyStr("apSSID", sets.apSSID);
    p.copyStr("apPass", sets.apPass);
    p.copyStr("staSSID", sets.staSSID);
    p.copyStr("staPass", sets.staPass);
    p.copyBool("staModeFlag", sets.staModeFlag);

    byte con = map(sets.dispContrast, 10, 100, 1, 255);
    eq.oled.setContrast(con);         // Тут же задаем яркость оледа
    EEPROM.put(1, sets);           // Сохраняем все настройки в EEPROM
    EEPROM.commit();               // Записываем
  }
}

void RMFileUploadStart() {
  eq.oled.clear();
  eq.oled.home();

  drawStatus(upModeStr);

  eq.oled.setCursor(0, 2);
  if (sets.staModeFlag) {
    eq.oled.print("SSID: ");
    eq.oled.println(sets.staSSID);
    eq.oled.print("Connecting");
    eq.oled.update();
    
    WiFi.begin(sets.staSSID, sets.staPass);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1500);
      eq.oled.print(".");
      eq.oled.update();
    }
  } else {
    eq.oled.print("AP SSID: ");
    eq.oled.println(sets.apSSID);
    eq.oled.println("Enabling AP");
    eq.oled.update();
    
    WiFi.softAP(sets.apSSID, sets.apPass);
  }

  eq.oled.println();
  eq.oled.print("IP: ");
  if (sets.staModeFlag) {
        eq.oled.println(WiFi.localIP());
  } else {
        eq.oled.println(WiFi.softAPIP());
  }
  eq.oled.update();

  eq.ui.attachBuild(build);
  eq.ui.attach(action);
  eq.ui.start();
  eq.ui.enableOTA();
}

void RMFileUploadTick() {
  eq.ui.tick();
  unsigned long mi = millis();
  if (mi - batTimer > STATUSBAR_TIME) {
    drawStatus(upModeStr);
    batTimer = mi;
  }
}

void RMFileUploadSuspend() {
  eq.ui.stop();
  WiFi.mode(WIFI_OFF);
}

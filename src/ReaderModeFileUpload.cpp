#include "ReaderModeFileUpload.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

extern ReaderEquipment eq;
extern ReaderSettings sets;

extern uint32_t batTimer;

String upModeStr = "UPLOAD MODE";

int preContrast;
bool choosingWiFiMode;
bool isSTA;

// конструктор страницы
void build() {
  GP.BUILD_END();

  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("Wi-Fi Reader"); // Обзываем титл
  GP.FORM_BEGIN("/cfg");         // Начало формы
  GP.GRID_RESPONSIVE(600); // Отключение респонза при узком экране
  M_BLOCK( // Общий блок-колонка для WiFi
      GP.SUBMIT("SUBMIT SETTINGS"); // Кнопка отправки формы
      M_BLOCK_TAB( // Конфиг для AP режима -> текстбоксы (логин + пароль)
          "AP-Mode", // Имя + тип DIV
          GP.TEXT("apSSID", "Login", sets.apSSID, "", 20);
          GP.BREAK(); GP.TEXT("apPass", "Password", sets.apPass, "", 20);
          GP.BREAK(););
      M_BLOCK_TAB("STA-Mode", GP.TEXT("staSSID", "Login", sets.staSSID, "", 20);
                  GP.BREAK();
                  GP.TEXT("staPass", "Password", sets.staPass, "", 20);
                  GP.BREAK(););
      M_BOX(GP_CENTER, GP.LABEL("Timezone");
            GP.NUMBER("gmt", "GMT", sets.gmt););
      GP.FORM_END();         // <- Конец формы (костыль)
      M_BLOCK_TAB(           // Блок с OTA-апдейтом
          "ESP UPDATE",      // Имя + тип DIV
          GP.OTA_FIRMWARE(); // Кнопка с OTA начинкой
      );
      M_BLOCK_TAB(        // Блок с файловым менеджером
          "FILE MANAGER", // Имя + тип DIV
          GP.FILE_UPLOAD("file_upl"); // Кнопка для загрузки файла
          GP.FOLDER_UPLOAD("folder_upl");
          GP.FILE_MANAGER(&LittleFS); // Файловый менеджер
      ););
  GP.BUILD_END(); // Конец билда страницы
}

void action(GyverPortal &p) { // Подсос значений со страницы
  if (p.form("/cfg")) { // Если есть сабмит формы - копируем все в переменные
    p.copyStr("apSSID", sets.apSSID);
    p.copyStr("apPass", sets.apPass);
    p.copyStr("staSSID", sets.staSSID);
    p.copyStr("staPass", sets.staPass);
    p.copyInt("gmt", sets.gmt);

    EEPROM.put(1, sets); // Сохраняем все настройки в EEPROM
    EEPROM.commit();     // Записываем
  }
}

void printNetInfo() {
  eq.oled.clear(0, 16, 127, 63);
  eq.oled.setCursor(0, 2);

  if (isSTA) {
    eq.oled.print("SSID: ");
    eq.oled.println(sets.staSSID);
  } else {
    eq.oled.print("AP SSID: ");
    eq.oled.println(sets.apSSID);
    eq.oled.print("PASS: ");
    eq.oled.println(sets.apPass);
  }

  eq.oled.println();
  eq.oled.print("IP: ");
  if (isSTA) {
    eq.oled.println(WiFi.localIP());
  } else {
    eq.oled.println(WiFi.softAPIP());
  }
  eq.oled.update();
}

void RMFileUploadStart() {
  preContrast = sets.dispContrast;

  eq.oled.clear();
  eq.oled.home();

  eq.oled.println("WiFi or host?");
  eq.oled.println("UP: WiFi");
  eq.oled.println("DOWN: host AP");
  eq.oled.update();
  choosingWiFiMode = true;
}

void startNetwork() {
  if (isSTA) {
    eq.oled.clear();
    eq.oled.home();

    drawStatus(upModeStr);

    eq.oled.setCursor(0, 2);

    eq.oled.println("Connecting");
    eq.oled.print("SSID: ");
    eq.oled.println(sets.staSSID);
    eq.oled.update();

    WiFi.mode(WIFI_STA);
    WiFi.begin(sets.staSSID, sets.staPass);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1500);
      eq.oled.print(".");
      eq.oled.update();
      ++i;
      if (i > 9) {
        isSTA = false;
        break;
      }
    }
  }
  if (!isSTA) {
    eq.oled.clear();
    eq.oled.home();

    drawStatus(upModeStr);

    eq.oled.setCursor(0, 2);

    eq.oled.println("Enabling AP");
    eq.oled.print("AP SSID: ");
    eq.oled.println(sets.apSSID);
    eq.oled.print("PASS: ");
    eq.oled.println(sets.apPass);
    eq.oled.update();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(sets.apSSID, sets.apPass);
  }

  eq.oled.println();
  eq.oled.print("IP: ");
  if (isSTA) {
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
  if (choosingWiFiMode) {
    if (eq.up.click()) {
      isSTA = true;
      startNetwork();
      choosingWiFiMode = false;
    } else if (eq.down.click()) {
      isSTA = false;
      startNetwork();
      choosingWiFiMode = false;
    }
  } else {
    eq.ui.tick();
    
    unsigned long mi = millis();
    if (mi - batTimer > STATUSBAR_TIME) {
      drawStatus(upModeStr);
      batTimer = mi;
    }

    if (eq.up.click() && preContrast < 100) {
      preContrast += 10;
      byte con = translateContrast(preContrast);
      eq.oled.setContrast(con); // Тут же задаем яркость оледа
    } else if (eq.down.click() && preContrast > 10) {
      preContrast -= 10;
      byte con = translateContrast(preContrast);
      eq.oled.setContrast(con); // Тут же задаем яркость оледа
    } else if (eq.ok.hold(2)) {
      sets.dispContrast = preContrast;
      EEPROM.put(1, sets);
      EEPROM.commit();
      eq.oled.clear(0, 16, 127, 63);
      eq.oled.setCursor(0, 2);
      eq.oled.println("contrast saved!");
      eq.oled.println(sets.dispContrast);
      eq.oled.update();
      delay(1000);
      printNetInfo();
    }
  }
}

void RMFileUploadSuspend() {
  eq.ui.stop();
  WiFi.mode(WIFI_OFF);

  if (sets.dispContrast != preContrast) {
    byte con = translateContrast(preContrast);
    eq.oled.setContrast(con); // Тут же задаем яркость оледа
  }
}

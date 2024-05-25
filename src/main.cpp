
#include <EEPROM.h>   // Либа EEPROM
#include <LittleFS.h> // Либа файловой системы
#include <Wire.h>     // Либа I2C

#include "ReaderCommon.h"
#include "ReaderEquipment.h"
#include "ReaderSettings.h"

#include "ReaderModeFileRead.h"
#include "ReaderModeFileUpload.h"
#include "ReaderModeTime.h"

#define EE_KEY 0x69 // поставьте другое значение, чтобы настройки сбросились

#define AP_DEFAULT_SSID "ReAdEr"
#define AP_DEFAULT_PASS "00000000"

#define STA_DEFAULT_SSID "<nonexistant>"
#define STA_DEFAULT_PASS "12345678"

#define SCR_CONTRAST 100
#define UNLOCK_PASS "000012" // пароль для читалки, пока нигде не используется
#define DEF_GMT 4 // часовой пояс для режима часов

#define EEPROM_SIZE 125

#define IIC_SDA_PIN 4
#define IIC_SCL_PIN 5

#define _EB_DEB 25
#define HOLD_TIME 1500


ReaderEquipment eq = {
  GyverPortal(&LittleFS),
  GyverOLED<SSD1306_128x64>(0x3C),
  ButtonT<UP_BTN_PIN>(INPUT_PULLUP),
  ButtonT<OK_BTN_PIN>(INPUT_PULLUP),
  ButtonT<DWN_BTN_PIN>(INPUT_PULLUP)
};

ReaderSettings sets = { // Структура со всеми настройками
    AP_DEFAULT_SSID,  AP_DEFAULT_PASS,
    STA_DEFAULT_SSID, STA_DEFAULT_PASS,
    SCR_CONTRAST,
    UNLOCK_PASS,
    DEF_GMT
};

int batMV = 3000; // Напряжение питания ESP

uint32_t batTimer = STATUS_TIME_MIN; // Таймер опроса АКБ

ADC_MODE(ADC_VCC); // Режим работы АЦП - измерение VCC

void initOled() {
  eq.oled.init(IIC_SDA_PIN, IIC_SCL_PIN); // Инициализация оледа
  eq.oled.autoPrintln(true);
  eq.oled.clear();                        // Очистка оледа
  eq.oled.update(); // Вывод пустой картинки
}

void eepromInit() {
  EEPROM.begin(EEPROM_SIZE); // Инициализация EEPROM
  if (EEPROM.read(0) != EE_KEY) { // Если ключ еепром не совпадает
    EEPROM.write(0, EE_KEY); // Пишем ключ
    EEPROM.put(1, sets); // Пишем дефолтные настройки
    EEPROM.commit();         // Запись
  } else {                   // Если ключ совпадает
    EEPROM.get(1, sets); // Читаем настройки
  }
}

int curMode;
bool choosing;

ReaderMode readerModes[] = {
  {"read files", RMFileReadStart, RMFileReadTick, RMFileReadSuspend},
  {"upload files", RMFileUploadStart, RMFileUploadTick, RMFileUploadSuspend},
  {"watch time", RMTimeStart, RMTimeTick, RMTimeSuspend}
};
int modeCount = sizeof(readerModes) / sizeof(readerModes[0]);

void drawMode();

void ReaderStart();
void ReaderTick();

void setup() {
  eq.down.setHoldTimeout(HOLD_TIME);
  eq.ok.setHoldTimeout(HOLD_TIME);
  eq.up.setHoldTimeout(HOLD_TIME);

  initOled();

  LittleFS.begin(); // Инициализация файловой системы

  eepromInit();

  eq.oled.setContrast(
      translateContrast(sets.dispContrast)); // Тут же задаем яркость оледа

  batMV = ESP.getVcc(); // Читаем напряжение питания

  eq.oled.println("microReader improved");
  eq.oled.println();
  eq.oled.println("enjoy :)");
  eq.oled.update();
  delay(1000);
  
  ReaderStart();
}

void loop() {
  eq.up.tick();
  eq.ok.tick();
  eq.down.tick();

  ReaderTick();
}

void ReaderStart() {
  curMode = 0;
  choosing = true;
  
  drawMode();
}

void ReaderTick() {
  if (choosing) {
    if (eq.ok.click()) {
      choosing = false;
      readerModes[curMode].start();
    } else if (eq.down.click() && curMode < modeCount - 1) {
      curMode += 1;
      drawMode();
    } else if (eq.up.click() && curMode > 0) {
      curMode -= 1;
      drawMode();
    }
  } else {
    if (eq.up.holding() && eq.down.holding() && eq.ok.holding()) {
      readerModes[curMode].suspend();
      choosing = true;
      drawMode();
    } else {
      readerModes[curMode].tick();
    }
  }
}

void drawMode() {
    eq.oled.clear();
    eq.oled.home();
    eq.oled.println(readerModes[curMode].name);
    eq.oled.update();
}


#include <EEPROM.h>   // Либа EEPROM
#include <LittleFS.h> // Либа файловой системы
#include <Wire.h>     // Либа I2C

#include "ReaderEquipment.h"
#include "ReaderModeManager.h"
#include "ReaderSettings.h"

#define AP_DEFAULT_SSID "ReAdEr"
#define AP_DEFAULT_PASS "00000000"
#define STA_DEFAULT_SSID "slowashell"
#define STA_DEFAULT_PASS "brutesomebacon"

#define STA_CONNECT_EN 0

#define IIC_SDA_PIN 4
#define IIC_SCL_PIN 5
#define EE_KEY 0x10
#define _EB_DEB 25
#define SCR_CONTRAST 100
#define EEPROM_SIZE 100
#define UNLOCK_PASS "000012"

ReaderSettings settings = { // Структура со всеми настройками
    AP_DEFAULT_SSID,  AP_DEFAULT_PASS,

    STA_DEFAULT_SSID, STA_DEFAULT_PASS,

    STA_CONNECT_EN,

    SCR_CONTRAST,

    UNLOCK_PASS};

int batMV = 3000; // Напряжение питания ESP

int translateContrast(byte x) { return map(x, 10, 100, 1, 255); }

ReaderEquipment eq = {GyverPortal(&LittleFS), GyverOLED<SSD1306_128x64>(0x3C),
                      ButtonT<UP_BTN_PIN>(INPUT_PULLUP),
                      ButtonT<OK_BTN_PIN>(INPUT_PULLUP),
                      ButtonT<DWN_BTN_PIN>(INPUT_PULLUP)};

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
    EEPROM.put(1, settings); // Пишем дефолтные настройки
    EEPROM.commit();         // Запись
  } else {                   // Если ключ совпадает
    EEPROM.get(1, settings); // Читаем настройки
  }
}

ReaderModeManager modeMan(eq, settings);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Пара подмигиваний
  for (uint8_t i = 0; i < 6; i++) { // Для индикации запуска
    digitalWrite(LED_BUILTIN, LOW);
    delay(30);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
  }
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(LED_BUILTIN, INPUT); // Выключаем и продолжаем

  eq.ok.setHoldTimeout(1500); // Длинное удержание кнопки ОК - 1.5 секунды

  initOled();

  LittleFS.begin(); // Инициализация файловой системы

  eepromInit();

  eq.oled.setContrast(
      translateContrast(settings.dispContrast)); // Тут же задаем яркость оледа

  batMV = ESP.getVcc(); // Читаем напряжение питания
  modeMan.start();
}

void loop() {
  eq.up.tick();
  eq.ok.tick();
  eq.down.tick();

  modeMan.tick();
}

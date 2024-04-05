#include "ReaderModeFileUpload.h"

#include <ESP8266WiFi.h>
#include <LittleFS.h>

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FILE_UPLOAD("file_upl");     // кнопка загрузки
  GP.FOLDER_UPLOAD("folder_upl"); // кнопка загрузки
  GP.FILE_MANAGER(&LittleFS);     // файловый менеджер

  GP.BUILD_END();
}

ReaderModeFileUpload::ReaderModeFileUpload(ReaderEquipment &eq,
                                           ReaderSettings &sets)
    : ReaderMode(eq, sets) {}

void ReaderModeFileUpload::start() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(sets.staSSID, sets.staPass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    eq.oled.print(".");
    eq.oled.update();
  }
  eq.oled.println();
  eq.oled.println(WiFi.localIP());
  eq.oled.update();

  eq.ui.attachBuild(build);
  // ui.attach(action);
  eq.ui.start();
}

void ReaderModeFileUpload::tick() {
  eq.ui.tick();
  if (eq.up.pressing() && eq.down.pressing() && eq.ok.pressing())
    ;
}

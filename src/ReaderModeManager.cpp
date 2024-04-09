
#include "ReaderModeManager.h"

void ReaderModeManager::drawMode() {
    eq.oled.clear();
    eq.oled.home();
    switch (chState) {
    case CH_STATE_READ:
      eq.oled.println("read files");
      break;
    case CH_STATE_UPLOAD:
      eq.oled.println("upload files");
      break;
    default:
      //bruh
      break;
    }
    eq.oled.update();
}

ReaderMode* ReaderModeManager::stToMode() {
  switch (chState) {
    case CH_STATE_READ:
      return &fileRead;
    case CH_STATE_UPLOAD:
      return &fileUpload;
    default:
      return NULL;
    }
}

ReaderModeManager::ReaderModeManager(ReaderEquipment &eq, ReaderSettings &sets):fileRead(eq, sets), fileUpload(eq, sets), eq(eq), sets(sets) {  
}

void ReaderModeManager::start() {
  curMode = NULL;
  chState = CH_STATE_READ;
  
  drawMode();
}

void ReaderModeManager::tick() {
  if (curMode == NULL) {
    if (eq.ok.click()) {
      curMode = stToMode();
      curMode->start();
    } else if (eq.down.click() && chState != CH_STATE_LAST) {
      chState += 1;
      drawMode();
    } else if (eq.up.click() && chState > 0) {
      chState -= 1;
      drawMode();
    }
  } else {
    if (eq.up.holding() && eq.down.holding() && eq.ok.holding()) {
      curMode->suspend();
      curMode = NULL;
      drawMode();
    } else {
      curMode->tick();
    }
  }
}

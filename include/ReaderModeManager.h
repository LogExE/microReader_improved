#pragma once

#include "ReaderModeFileRead.h"
#include "ReaderModeFileUpload.h"

#define CH_STATE_READ 0
#define CH_STATE_UPLOAD 1
#define CH_STATE_LAST 2

class ReaderModeManager {
private:
  ReaderMode *curMode;
  ReaderModeFileRead fileRead;
  ReaderModeFileUpload fileUpload;
  int chState;
  
  ReaderEquipment &eq;
  ReaderSettings &sets;

public:
  ReaderModeManager(ReaderEquipment &eq, ReaderSettings &sets);

  void drawMode();

  ReaderMode* stToMode();
  
  void start();
  void tick();
};

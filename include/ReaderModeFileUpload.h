#pragma once

#include "ReaderMode.h"

class ReaderModeFileUpload : public ReaderMode {
  public:
  ReaderModeFileUpload(ReaderEquipment &eq, ReaderSettings &sets);

  void start();
  void tick();
  void suspend();
};

#pragma once

#include "ReaderEquipment.h"
#include "ReaderSettings.h"

#include "ReaderCommon.h"

class ReaderMode {
 public:
  ReaderMode(ReaderEquipment &eq, ReaderSettings &sets);
  virtual void start() = 0;
  virtual void tick() = 0;
  
 protected:
  ReaderEquipment &eq;
  ReaderSettings &sets;
};

#pragma once

#include "ReaderCommon.h"

struct ReaderMode {
  void (*start)();
  void (*tick)();
  void (*suspend)();
};

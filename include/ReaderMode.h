#pragma once

#include "ReaderCommon.h"

struct ReaderMode {
  String name;
  void (*start)();
  void (*tick)();
  void (*suspend)();
};

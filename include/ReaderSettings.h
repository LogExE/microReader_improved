#pragma once

struct ReaderSettings
{
  char apSSID[21];
  char apPass[21];
  char staSSID[21];
  char staPass[21];
  bool staModeFlag;
  int dispContrast;
  char password[11];
};

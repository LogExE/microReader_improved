#pragma once

#include <LittleFS.h>

#include "ReaderMode.h"

class ReaderModeFileRead : public ReaderMode {
public:
  ReaderModeFileRead(ReaderEquipment &eq, ReaderSettings &sets);

  const static int FILES_ON_PAGE = 6;
  const static int SLINE = 2;

  const static int BYTES_IN_PIC = 8 * 128; // 8 строк по 128 байт

  const static int MAX_FILES = 100;

  const static int MAX_TEXT_PAGES = 100;

  constexpr static const char *picRes = ".pic";
  constexpr static const char *textRes = ".txt";

  enum State { READ_TEXT, READ_PIC, MENU } curState;

  // переменные состояния меню
  String fileNames[MAX_FILES];
  int fileCnt;
  int cursor;

  // перменные, валидные при просмотре текста
  int txtPageStartByte[MAX_TEXT_PAGES];
  int curPage = 0;
  int lastPage = MAX_TEXT_PAGES;

  File curFile;

  uint32_t batTimer = STATUS_TIMEMIN; // Таймер опроса АКБ
  String status = "";
  bool statusFirst = true;

  void start();

  void tick();

  void enterFile();

  void exitFile();

  void drawMenu();

  void drawText();

  void drawPic();
};

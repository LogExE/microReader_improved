#include "ReaderModeFileRead.h"

#include <LittleFS.h>

extern ReaderEquipment eq;
extern ReaderSettings sets;

extern uint32_t batTimer;

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
int curPage;
int lastPage;

File curFile;

void drawMenu();
void drawStatus();

void drawText();
void drawPic();

void enterFile();
void exitFile();

String readModeStr = "READING MODE";

void RMFileReadStart() {
  // заполняем инфу о файлах в файловой системе
  Dir root = LittleFS.openDir(FOLDER_ROOT);
  fileCnt = 0;
  for (int i = 0; root.next(); ++i) {
    fileNames[i] = root.fileName();
    ++fileCnt;
  }

  // ставим курсор на первый файл
  cursor = 0;
  // пока что только в меню
  curState = MENU;

  drawStatus(readModeStr);
  drawMenu();
}

void RMFileReadTick() {
  if (curState == MENU) {
    unsigned long mi = millis();
    if (mi - batTimer >= STATUSBAR_TIME) {
      drawStatus(readModeStr);
      batTimer = mi;
    }
    
    if (eq.ok.click()) {
      enterFile();
    } else if (eq.up.click() && cursor > 0) {
      cursor--;
      drawMenu();
    } else if (eq.down.click() && cursor < fileCnt - 1) {
      cursor++;
      drawMenu();
    }
  } else {
    if (eq.ok.click()) {
      exitFile();
    }
    if (curState == READ_TEXT) {
      if ((eq.up.click() || eq.up.holding()) && curPage > 0) {
        curPage--;
        drawText();
      } else if ((eq.down.click() || eq.down.holding()) && curPage < lastPage) {
        curPage++;
        drawText();
      }
    } else {
    }
  }
}

void RMFileReadSuspend() {
  if (curState != MENU) {
    curFile.close();
  }
}

void enterFile() {
  String name = fileNames[cursor];
  curFile = LittleFS.open(FOLDER_ROOT "/" + name, "r");
  if (name.endsWith(textRes)) {
    txtPageStartByte[0] = 0;
    curPage = 0;
    lastPage = MAX_TEXT_PAGES;
    
    curState = READ_TEXT;
    
    drawText();
  } else if (name.endsWith(picRes)) {
    curState = READ_PIC;
    
    drawPic();
  } else {
    eq.oled.println("WTF");
    eq.oled.update();
  }
}

void exitFile() {
  curFile.close();
  
  curState = MENU;
  drawMenu();
  
  batTimer = STATUS_TIME_MIN;
}

void drawMenu() {
  eq.oled.clear(0, 16, 127, 63);
  eq.oled.setCursor(0, SLINE);
  if (fileCnt == 0) {
    eq.oled.println("-empty-");
  } else {
    int from = cursor - cursor % FILES_ON_PAGE;
    for (int i = 0; i < FILES_ON_PAGE; ++i) {
      if (cursor == i) {
        eq.oled.print("*");
      } else {
        eq.oled.print(" ");
      }
      eq.oled.println(fileNames[from + i]);
      if (from + i == fileCnt - 1)
        break;
    }
  }
  eq.oled.update(0, 16, 127, 63);
}

void drawText() {
  eq.oled.clear();
  eq.oled.home();

  curFile.seek(txtPageStartByte[curPage]);
  
  int soughtBs = 0;
  while (curFile.available() && !eq.oled.isEnd()) {
    ++soughtBs;
    char b = curFile.read();
    if (b == '\n') {
      eq.oled.write('\r');
      //      eq.oled.write('\n');
    }
    eq.oled.write(b);
  }

  if (curPage < MAX_TEXT_PAGES - 1) {
    txtPageStartByte[curPage + 1] = txtPageStartByte[curPage] + soughtBs;
  }
  if (!curFile.available()) {
    lastPage = curPage;
  }
  eq.oled.update();
}

void drawPic() {
  eq.oled.clear();
  eq.oled.home();
  char picBytes[BYTES_IN_PIC];
  curFile.readBytes(picBytes, BYTES_IN_PIC);
  eq.oled.drawBitmap(0, 0, (uint8_t *)picBytes, 128, 64, BITMAP_NORMAL,
                     BUF_ADD);
  eq.oled.update();
}

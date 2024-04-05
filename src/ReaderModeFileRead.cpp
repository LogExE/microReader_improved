#include "ReaderModeFileRead.h"

ReaderModeFileRead::ReaderModeFileRead(ReaderEquipment &eq,
                                       ReaderSettings &sets)
    : ReaderMode(eq, sets) {}

void ReaderModeFileRead::start() {
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

  drawMenu();
}

void ReaderModeFileRead::tick() {
  if (curState == MENU) {
    unsigned long mi = millis();
    if (statusFirst || mi - batTimer >= STATUSBAR_TIME) {
      eq.oled.clear(0, 0, 127, 15);
      eq.oled.home();
      eq.oled.println(millis());
      eq.oled.line(0, 10, 127, 10);
      eq.oled.update(0, 0, 127, 15);
      batTimer = mi;
      if (statusFirst)
        statusFirst = false;
    }
    if (eq.ok.click()) {
      enterFile();
    } else if (eq.up.click() && cursor <= fileCnt - 1) {
      cursor++;
      drawMenu();
    } else if (eq.down.click() && cursor > 0) {
      cursor--;
      drawMenu();
    }
  } else {
    if (eq.ok.click()) {
      exitFile();
    }
    if (curState == READ_TEXT) {
      if (eq.up.click() && curPage != 0) {
        curPage--;
        drawText();
      } else if (eq.down.click() && curPage != lastPage) {
        curPage++;
        drawText();
      }
    } else {
    }
  }
}

void ReaderModeFileRead::enterFile() {
  String name = fileNames[cursor];
  curFile = LittleFS.open(name, "r");
  if (name.endsWith(textRes)) {
    txtPageStartByte[0] = 0;
    curPage = 0;
    curState = READ_TEXT;
    drawText();
  } else if (name.endsWith(picRes)) {
    curState = READ_PIC;
    drawPic();
  } else {
    eq.oled.println("WTF");
  }
}

void ReaderModeFileRead::exitFile() {
  curFile.close();
  curState = MENU;
  drawMenu();
  batTimer = STATUS_TIMEMIN;
}

void ReaderModeFileRead::drawMenu() {
  eq.oled.clear(0, 16, 127, 63);
  eq.oled.setCursor(0, SLINE);
  if (fileCnt == 0) {
    eq.oled.println("-empty-");
  } else {
    int from = cursor - cursor % FILES_ON_PAGE;
    for (int i = 0; i < FILES_ON_PAGE; ++i) {
      if (cursor == i)
        eq.oled.print("* ");
      else
        eq.oled.print("  ");
      eq.oled.println(fileNames[from + i]);
      if (from + i == fileCnt - 1)
        break;
    }
  }
  eq.oled.update(0, 16, 127, 63);
}

void ReaderModeFileRead::drawText() {
  eq.oled.clear();
  eq.oled.home();
  int curLine = 0;
  int curCol = 0;
  int soughtBs = 0;
  curFile.seek(txtPageStartByte[curPage], SeekSet);
  while (curFile.available()) {
    ++soughtBs;
    char b = curFile.read();
    if (b == '\n') {
      curCol = 21;
    } else if (b == '\r') {
      // bruh
    } else {
      eq.oled.print(b);
      curCol++;
    }

    // строка переполнилась
    if (curCol == 21) {
      curLine++;
      curCol = 0;
      // страница прочтена
      if (curLine == 8) {
        break;
      }
      eq.oled.setCursor(curCol, curLine);
    }
  }
  if (curPage < MAX_TEXT_PAGES - 1) {
    txtPageStartByte[curPage + 1] = soughtBs;
  }
  if (!curFile.available()) {
    lastPage = curPage;
  }
  eq.oled.update();
}

void ReaderModeFileRead::drawPic() {
  eq.oled.clear();
  eq.oled.home();
  char picBytes[BYTES_IN_PIC];
  curFile.readBytes(picBytes, BYTES_IN_PIC);
  eq.oled.drawBitmap(0, 0, (uint8_t *)picBytes, 128, 64, BITMAP_NORMAL,
                     BUF_ADD);
  eq.oled.update();
}

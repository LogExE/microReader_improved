#pragma once

#include "ReaderEquipment.h"
#include "ReaderSettings.h"

#define VBAT_FULL_MV 3600 // Напряжение питания при заряженном аккуме в (мВ)
#define VBAT_EMPTY_MV 2600 // Напряжение питания при севшем аккуме в (мВ)
#define STATUSBAR_TIME 5000
#define STATUS_TIME_MIN 0
#define FOLDER_ROOT "/" 

void drawStatus(String str);

int translateContrast(byte x);

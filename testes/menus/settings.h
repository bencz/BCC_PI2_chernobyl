#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

bool settings_load();
void settings_unload();
void settings_update();
void settings_draw();
bool settings_start();

#endif
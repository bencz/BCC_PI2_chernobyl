#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

bool level_load();
void level_unload();
void level_update();
void level_draw();
bool level_start();

#endif
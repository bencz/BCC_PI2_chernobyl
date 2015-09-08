#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "game.h"

float pulseTempo2;

bool settings_load();
void settings_unload();
bool settings_update();
void settings_draw();

bool settings_start() {
	if (!settings_load()) {
		return false;
	}
	scene.unload = &settings_unload;
	scene.update = &settings_update;
	scene.draw = &settings_draw;
	
	pulseTempo2 = 1;
	
	return true;
}

bool settings_load() {
	return true;
}

void settings_unload() {
	//
}

bool settings_update() {
	//temp: alternar entre as cenas
	if (input.left->press) {
		sceneLoad(MENU);
	} else if (input.right->press) {
		sceneLoad(LEVEL);
	}
	
	//igual o menu, ainda n tem nada, mas dá pra pulsar a ovelha tb
	if (input.space->press) {
		pulseTempo2 = 1.3;
	} else if (input.space->release) {
		pulseTempo2 = 1.5;
	} else if (input.space->hold) {
		pulseTempo2 = lerp(pulseTempo2,1.1,DELTA*10);
	} else {
		pulseTempo2 = lerp(pulseTempo2,1,DELTA*10);
	}
	
	return true;
}

void settings_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,0,ALLEGRO_ALIGN_LEFT,"configurações");
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,30,ALLEGRO_ALIGN_LEFT,"esquerda: menu");
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,60,ALLEGRO_ALIGN_LEFT,"direita: fase");
	al_draw_scaled_bitmap(data.sprite_test,0,0,256,256,64,64,pulseTempo2*256,pulseTempo2*256,0);
}
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "game.h"

float pulseTempo;

bool menu_load();
void menu_unload();
bool menu_update();
void menu_draw();

bool menu_start() {
	if (!menu_load()) {
		return false;
	}
	scene.unload = &menu_unload;
	scene.update = &menu_update;
	scene.draw = &menu_draw;
	
	pulseTempo = 1;
	
	return true;
}

bool menu_load() {
	return true;
}

void menu_unload() {
	//
}

bool menu_update() {
	//temp: alternar entre as cenas
	if (input.right->press) {
		sceneLoad(SETTINGS);
	}
	
	//n tem nada ainda, mas dá pra apertar espaço e pulsar a ovelha
	if (input.space->press) {
		pulseTempo = 1.3;
	} else if (input.space->release) {
		pulseTempo = 1.5;
	} else if (input.space->hold) {
		pulseTempo = lerp(pulseTempo,1.1,DELTA*10);
	} else {
		pulseTempo = lerp(pulseTempo,1,DELTA*10);
	}
	
	return true;
}

void menu_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,0,ALLEGRO_ALIGN_LEFT,"menu");
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,30,ALLEGRO_ALIGN_LEFT,"direita: configurações");
	al_draw_scaled_bitmap(data.sprite_test,0,0,256,256,64,64,pulseTempo*256,pulseTempo*256,0);
}
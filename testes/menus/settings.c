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

bool settings_load();
void settings_unload();
void settings_update();
void settings_draw();

bool settings_start() {
	if (!settings_load()) {
		return false;
	}
	scene.unload = &settings_unload;
	scene.update = &settings_update;
	scene.draw = &settings_draw;

	pulseTempo = 1;

	return true;
}

bool settings_load() {
	return true;
}

void settings_unload() {
	//
}

void settings_update() {
	if (scene.tempo <= 0) {
		if (input.enter->press) {
			sceneLoad(MENU);
		}

		//n tem nada ainda, mas dá pra apertar o botão pra cima e pulsar a ovelha
		if (input.up->press) {
			pulseTempo = 1.3;
		} else if (input.up->release) {
			pulseTempo = 1.5;
		} else if (input.up->hold) {
			pulseTempo = lerp(pulseTempo,1.1,DELTA*10);
		} else {
			pulseTempo = lerp(pulseTempo,1,DELTA*10);
		}
	} else {
		pulseTempo = lerp(pulseTempo,1,DELTA*10);
	}
}

void settings_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);

	//ovelha
	al_draw_scaled_bitmap(data.sprite_test,0,0,256,256,SCREEN_W/2-pulseTempo*128,SCREEN_H/2-pulseTempo*128,pulseTempo*256,pulseTempo*256,0);

	//textos
	al_draw_textf(data.font_regular,colorButton,SCREEN_W/2,30,ALLEGRO_ALIGN_CENTRE,"configurações");
	al_draw_textf(data.font_regular,colorButton2,SCREEN_W/2,SCREEN_H-60,ALLEGRO_ALIGN_CENTRE,"voltar");
}

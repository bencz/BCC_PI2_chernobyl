#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "input.h"
#include "game.h"
#include "draw.h"

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
	scene.showLetterbox = true;

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
		if (input.enter->press || input.escape->press) {
			sceneLoad(MENU);
		}
		//n tem nada ainda, mas dá pra apertar espaço e pulsar a ovelha
		if (input.space->press) {
			pulseTempo = 1.3;
		} else if (input.space->release) {
			pulseTempo = 1.5;
		} else if (input.space->hold) {
			pulseTempo = lerp(pulseTempo,1.1,game.delta*10);
		} else {
			pulseTempo = lerp(pulseTempo,1,game.delta*10);
		}
	} else {
		pulseTempo = lerp(pulseTempo,1,game.delta*10);
	}
}

void settings_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);

	//ovelha
	drawBitmap(data.bitmap_test,.5,.5,pulseTempo*.25,pulseTempo*.25,0,0,0);

	//textos
	al_draw_text(data.font_Regular52,colorButton,px(.5),py(.05),ALLEGRO_ALIGN_CENTRE,"configurações");
	al_draw_text(data.font_Regular52,colorButton2,px(.5),py(.9),ALLEGRO_ALIGN_CENTRE,"voltar");
}

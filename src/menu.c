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

int selection;

bool menu_load();
void menu_unload();
void menu_update();
void menu_draw();

bool menu_start() {
	if (!menu_load()) {
		return false;
	}
	scene.unload = &menu_unload;
	scene.update = &menu_update;
	scene.draw = &menu_draw;
	scene.showLetterbox = true;

	selection = 0;

	return true;
}

bool menu_load() {
	return true;
}

void menu_unload() {
	//
}

void menu_update() {
	if (scene.tempo <= 0) {
		if (input.escape->press) exitGame();
		if (input.up->repeat && selection > 0) selection--;
		if (input.down->repeat && selection < 2) selection++;
		if (input.enter->press) switch (selection) {
			case 0: sceneLoad(LEVEL);
			case 1: sceneLoad(SETTINGS);
			default: exitGame();
		}
	}
}

void menu_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);

	//textos
	al_draw_text(data.font_Regular52,colorButton,px(.5),py(.05),ALLEGRO_ALIGN_CENTRE,"projeto chernobyl");
	al_draw_text(data.font_Regular52,(selection == 0)?colorButton2:colorButton,px(.5),py(.45),ALLEGRO_ALIGN_CENTRE,"jogar");
	al_draw_text(data.font_Regular52,(selection == 1)?colorButton2:colorButton,px(.5),py(.5),ALLEGRO_ALIGN_CENTRE,"configurações");
	al_draw_text(data.font_Regular52,(selection == 2)?colorButton2:colorButton,px(.5),py(.55),ALLEGRO_ALIGN_CENTRE,"sair");
}

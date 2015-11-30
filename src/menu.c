#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "utils.h"
#include "input.h"
#include "game.h"
#include "draw.h"

int selection;
float animTempo;

void menu_sound(bool b) {
	al_play_sample(b?data.sample_select2:data.sample_select,game.volumeSfx,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
}

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
	animTempo = 0;
	
	return true;
}

bool menu_load() {
	LOADBITMAP(data.bitmap_logo,logo.png);
	return true;
}

void menu_unload() {
	UNLOADBITMAP(data.bitmap_logo);
}

void menu_update() {
	if (scene.tempo <= 0) {
		if (input.escape->press) exitGame();
		if (input.up->repeat && selection > 0) {
			selection--;
			menu_sound(false);
		}
		if (input.down->repeat && selection < 2) {
			selection++;
			menu_sound(false);
		}
		if (input.enter->press) switch (selection) {
			case 0: sceneLoad(LEVEL);
			case 1: sceneLoad(SETTINGS);
			default: exitGame();
			menu_sound(true);
		}
	}
	animTempo += game.delta*.25;
}

void menu_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	drawBitmap(data.bitmap_parallax1,.5+sinf(animTempo)*.03,.5+cosf(animTempo)*.03,game.idealProp*1.1,1.1,0,0,0);
	drawBitmap(data.bitmap_logo,.5,.2,game.idealProp,.27,0,0,0);
	
	float l = easeOut((scene.tempo > 0)?(scene.tempo):(1+scene.tempo));
	drawBox(.5,.625,.3,.2*l,COLOR_HGHL,COLOR_SCND);
	al_draw_text(data.font_Regular52,(selection == 0)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.625,.55,l)),ALLEGRO_ALIGN_CENTRE,"jogar");
	al_draw_text(data.font_Regular52,(selection == 1)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.625,.6,l)),ALLEGRO_ALIGN_CENTRE,"configurações");
	al_draw_text(data.font_Regular52,(selection == 2)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.625,.65,l)),ALLEGRO_ALIGN_CENTRE,"sair");
	BLENDALPHA();
	if (selection > 0) {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.5,lerp(.625,.475,l)-fabs(sinf(animTempo*16))*.007,1./18,1./18,4,2,4,0,0,0);
	}
	if (selection < 2) {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.5,lerp(.625,.775,l)+fabs(sinf(animTempo*16))*.007,1./18,1./18,4,2,5,0,0,0);
	}
	drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.675+fabs(sinf(animTempo*16))*.007,.625,1./18,1./18,4,2,0,0,0,0);
	BLENDDEFAULT();
}

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

float pauseTempo;
bool unpausing;
int selection;
bool confirm;
float confirmTempo;
int selection2;

float pulseTempo;

void startPause(bool ingame) {
	unpausing = false;
	confirm = false;
	pauseTempo = 0;
	selection = ingame?-1:0;
	confirmTempo = 0;
}

void updateSlider(float *value) {
	if (input.left->hold == input.right->hold) return;
	if (input.left->hold) {
		if (*value > 0) {
			*value -= game.delta;
			if (*value < 0) *value = 0;
		}
	}
	if (input.right->hold) {
		if (*value < 1) {
			*value += game.delta;
			if (*value > 1) *value = 1;
		}
	}
}

void drawSlider(float y,float value,bool glow,const char *str) {
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);
	float width = .18;
	float height = .05;
	float x5 = dx(.5);
	float x65 = dx(.5+width);
	float xlerp = dx(lerp(.5,.5+width,value));
	float y05 = dy(y+height);
	al_draw_filled_triangle(
		x5,y05,
		x65,y05,
		x65,dy(y),
		al_map_rgb(204,204,204)
	);
	al_draw_filled_triangle(
		x5,y05,
		xlerp,y05,
		xlerp,dy(lerp(y+height,y,value)),
		glow?colorButton2:colorButton
	);
	//al_draw_textf(data.font_Regular52,glow?colorButton2:colorButton,px(.5),py(y),ALLEGRO_ALIGN_CENTRE,"%.2f",value);
	al_draw_text(data.font_Regular52,glow?colorButton2:colorButton,px(.49),py(y),ALLEGRO_ALIGN_RIGHT,str);
}

bool updatePause(bool ingame) {
	if (confirm) {
		if (confirmTempo < 1) {
			confirmTempo += game.delta*8;
			if (confirmTempo > 1) confirmTempo = 1;
		}
	} else {
		if (confirmTempo > 0) {
			confirmTempo -= game.delta*8;
			if (confirmTempo < 0) confirmTempo = 0;
		}
	}
	if (unpausing) {
		if (pauseTempo > 0) {
			pauseTempo -= game.delta*4;
			if (pauseTempo <= 0) {
				pauseTempo = 0;
				return true;
			}
		}
		return false;
	} else if (ingame) {
		if (pauseTempo < 1) {
			pauseTempo += game.delta*4;
			if (pauseTempo > 1) {
				pauseTempo = 1;
			}
		}
	}
	if (scene.tempo <= 0) {
		if (confirm) {
			if (input.up->repeat && selection2 > 0) selection2--;
			if (input.down->repeat && selection2 < 1) selection2++;
			if (input.enter->press) {
				if (selection2 == 0) {
					unpausing = true;
					confirm = false;
					sceneLoad(MENU);
				} else {
					confirm = false;
				}
			} else if (input.escape->press) {
				confirm = false;
			}
			return false;
		}
		if (input.up->repeat && selection > (ingame?-1:0)) selection--;
		if (input.down->repeat && selection < 2) selection++;
		if ((selection == (ingame?-1:2) && input.enter->press) || input.escape->press) {
			if (ingame) {
				unpausing = true;
				return false;
			}
			sceneLoad(MENU);
		}
		if (selection == 0) {
			updateSlider(&game.volumeBgm);
		} else if (selection == 1) {
			updateSlider(&game.volumeSfx);
		} else if (ingame && selection == 2) {
			if (input.enter->press) {
				confirm = true;
				selection2 = 1;
			}
		}
		/*
		if (input.space->press) {
			pulseTempo = 1.3;
		} else if (input.space->release) {
			pulseTempo = 1.5;
		} else if (input.space->hold) {
			pulseTempo = lerp(pulseTempo,1.1,game.delta*10);
		} else {
			pulseTempo = lerp(pulseTempo,1,game.delta*10);
		}
		*/
	} else {
		//pulseTempo = lerp(pulseTempo,1,game.delta*10);
	}
	return false;
}

void drawPause(bool ingame) {
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);
	ALLEGRO_COLOR bg = al_map_rgba(255,255,255,204);
	float l = easeOut(pauseTempo);
	
	//ovelha
	//drawBitmap(data.bitmap_test,.5,.5,pulseTempo*.25*l,pulseTempo*.25*l,0,0,0);
	
	//textos
	if (ingame) {
		BLENDALPHA();
		al_draw_filled_rectangle(px(.3),py(lerp(.5,.3,l)),px(.7),py(lerp(.5,.7,l)),bg);
		BLENDDEFAULT();
		al_draw_text(data.font_Regular37,colorButton,px(.5),py(lerp(.5,.315,l)),ALLEGRO_ALIGN_CENTRE,"pause");
		al_draw_text(data.font_Regular52,(selection == -1)?colorButton2:colorButton,px(.5),py(lerp(.5,.38,l)),ALLEGRO_ALIGN_CENTRE,"continuar");
		al_draw_text(data.font_Regular52,(selection == 2)?colorButton2:colorButton,px(.5),py(lerp(.5,.62,l)),ALLEGRO_ALIGN_CENTRE,"sair ao menu");
	} else {
		al_draw_text(data.font_Regular52,colorButton,px(.5),py(lerp(.5,.05,l)),ALLEGRO_ALIGN_CENTRE,"configurações");
		al_draw_text(data.font_Regular52,(selection == 2)?colorButton2:colorButton,px(.5),py(lerp(.5,.9,l)),ALLEGRO_ALIGN_CENTRE,"voltar");
	}
	
	//sliders
	drawSlider(lerp(.5,.46,l),game.volumeBgm,selection == 0,"volume bgm");
	drawSlider(lerp(.5,.54,l),game.volumeSfx,selection == 1,"volume sfx");
	
	//tela de confirmação
	if (confirmTempo > 0) {
		float m = easeOut(confirmTempo);
		BLENDALPHA();
		al_draw_filled_rectangle(px(.25),py(lerp(.5,.4,m)),px(.75),py(lerp(.5,.6,m)),bg);
		BLENDDEFAULT();
		al_draw_text(data.font_Regular37,colorButton,px(.5),py(lerp(.5,.415,m)),ALLEGRO_ALIGN_CENTRE,"tem certeza de que deseja sair?");
		al_draw_text(data.font_Regular52,(selection2 == 0)?colorButton2:colorButton,px(.5),py(lerp(.5,.46,m)),ALLEGRO_ALIGN_CENTRE,"sim");
		al_draw_text(data.font_Regular52,(selection2 == 1)?colorButton2:colorButton,px(.5),py(lerp(.5,.52,m)),ALLEGRO_ALIGN_CENTRE,"nem");
	}
}

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
	startPause(false);
	pauseTempo = 1;
	
	return true;
}

bool settings_load() {
	return true;
}

void settings_unload() {
	//
}

void settings_update() {
	updatePause(false);
}

void settings_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	drawPause(false);
}

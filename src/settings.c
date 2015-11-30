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
#include "settings.h"

bool unpausing;
int selection;
bool confirm;
float confirmTempo;
int selection2;
float animTempo;

void settings_sound(bool b) {
	al_play_sample(b?data.sample_select2:data.sample_select,game.volumeSfx,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
}

void startPause(bool ingame) {
	unpausing = false;
	confirm = false;
	pauseTempo = 0;
	selection = ingame?-1:0;
	confirmTempo = 0;
	animTempo = 0;
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
	float width = .18;
	float height = .05;
	float x5 = dx(.5);
	float x65 = dx(.5+width);
	float xlerp = dx(lerp(.5,.5+width,value));
	float y05 = dy(y+height);
	al_draw_filled_triangle(x5,y05,x65,y05,x65,dy(y),COLOR_SCND);
	al_draw_filled_triangle(x5,y05,xlerp,y05,xlerp,dy(lerp(y+height,y,value)),glow?COLOR_HGHL:COLOR_TEXT);
	al_draw_text(data.font_Regular52,glow?COLOR_HGHL:COLOR_TEXT,px(.49),py(y),ALLEGRO_ALIGN_RIGHT,str);
}

bool updatePause(bool ingame) {
	animTempo += game.delta*.25;
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
			if (input.up->repeat && selection2 > 0) {
				selection2--;
				settings_sound(false);
			}
			if (input.down->repeat && selection2 < 1) {
				selection2++;
				settings_sound(false);
			}
			if (input.enter->press) {
				if (selection2 == 0) {
					unpausing = true;
					confirm = false;
					sceneLoad(MENU);
				} else {
					confirm = false;
				}
				settings_sound(true);
			} else if (input.escape->press) {
				confirm = false;
				settings_sound(true);
			}
			return false;
		}
		if (input.up->repeat && selection > (ingame?-1:0)) {
			selection--;
			settings_sound(false);
		}
		if (input.down->repeat && selection < 3) {
			selection++;
			settings_sound(false);
		}
		if ((selection == (ingame?-1:3) && input.enter->press) || input.escape->press) {
			settings_sound(true);
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
		} else if (selection == 2) {
			if (input.enter->press) {
				game.showPopups = !game.showPopups;
				settings_sound(true);
			}
		} else if (ingame && selection == 3) {
			if (input.enter->press) {
				confirm = true;
				selection2 = 1;
				settings_sound(true);
			}
		}
	}
	return false;
}

void drawPause(bool ingame) {
	ALLEGRO_COLOR bg = al_map_rgba(255,255,255,204);
	float l = easeOut(pauseTempo);
	
	//textos
	if (ingame) {
		BLENDALPHA();
		al_draw_filled_rectangle(px(0),py(0),px(1),py(1),al_map_rgba_f(0,0,0,.375*l));
		BLENDDEFAULT();
		drawBox(.5,.5,.4,.5*l,COLOR_HGHL,COLOR_SCND);
		al_draw_text(data.font_Regular37,COLOR_TEXT,px(.5),py(lerp(.5,.26,l)),ALLEGRO_ALIGN_CENTRE,"pause");
		al_draw_text(data.font_Regular52,(selection == -1)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.34,l)),ALLEGRO_ALIGN_CENTRE,"continuar");
		al_draw_text(data.font_Regular52,(selection == 3)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.66,l)),ALLEGRO_ALIGN_CENTRE,"sair ao menu");
	} else {
		drawBitmapTinted(data.bitmap_parallax1,al_map_rgb_f(1,1,.8),.5-sinf(animTempo)*.03,.5-cosf(animTempo)*.03,game.idealProp*1.1,1.1,0,0,0);
		l = easeOut((scene.tempo > 0)?(scene.tempo):(1+scene.tempo));
		drawBox(.5,.5,.4,.5*l,COLOR_HGHL,COLOR_SCND);
		al_draw_text(data.font_Regular52,COLOR_TEXT,px(.5),py(lerp(.5,.27,l)),ALLEGRO_ALIGN_CENTRE,"configurações");
		al_draw_text(data.font_Regular52,(selection == 3)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.66,l)),ALLEGRO_ALIGN_CENTRE,"voltar");
	}
	
	BLENDALPHA();
	if (selection > (ingame?-1:0)) {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.5,lerp(.5,.2,l)-fabs(sinf(animTempo*16))*.007,1./18,1./18,4,2,4,0,0,0);
	}
	if (selection < 3) {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.5,lerp(.5,.8,l)+fabs(sinf(animTempo*16))*.007,1./18,1./18,4,2,5,0,0,0);
	}
	if (selection < 0 || selection > 1) {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.725+fabs(sinf(animTempo*16))*.007,.5,1./18,1./18,4,2,0,0,0,0);
	} else {
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.275-fabs(sinf(animTempo*16))*.007,.5,1./18,1./18,4,2,6,0,0,0);
		drawSpriteSheetTinted(data.bitmap_keys,al_map_rgba_f(1,1,1,.5),.725+fabs(sinf(animTempo*16))*.007,.5,1./18,1./18,4,2,7,0,0,0);
	}
	BLENDDEFAULT();
	
	//sliders
	drawSlider(lerp(.5,.42,l),game.volumeBgm,selection == 0,"volume bgm");
	drawSlider(lerp(.5,.5,l),game.volumeSfx,selection == 1,"volume sfx");
	
	//toggle
	al_draw_text(data.font_Regular52,(selection == 2)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.58,l)),ALLEGRO_ALIGN_CENTRE,game.showPopups?"desligar tutorial":"ligar tutorial");
	
	//tela de confirmação
	if (confirmTempo > 0) {
		float m = easeOut(confirmTempo);
		BLENDALPHA();
		al_draw_filled_rectangle(px(0),py(0),px(1),py(1),al_map_rgba_f(0,0,0,.375*m));
		BLENDDEFAULT();
		drawBox(.5,.5,.5,.2*m,COLOR_HGHL,COLOR_SCND);
		al_draw_text(data.font_Regular37,COLOR_TEXT,px(.5),py(lerp(.5,.415,m)),ALLEGRO_ALIGN_CENTRE,"tem certeza de que deseja sair?");
		al_draw_text(data.font_Regular52,(selection2 == 0)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.46,m)),ALLEGRO_ALIGN_CENTRE,"sim");
		al_draw_text(data.font_Regular52,(selection2 == 1)?COLOR_HGHL:COLOR_TEXT,px(.5),py(lerp(.5,.52,m)),ALLEGRO_ALIGN_CENTRE,"nem");
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

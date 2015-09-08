#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "utils.h"
#include "game.h"
#include "menu.h"
#include "settings.h"
#include "level.h"

bool load() {
	ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
	LOADFONT(data.font_regular,28,Ubuntu-R.ttf);
	LOADFONT(data.font_mono,28,UbuntuMono-B.ttf);
	LOADSPRITE(data.sprite_test,test.png);
	return true;
}

void unload() {
	UNLOADSPRITE(data.sprite_test);
}

key *initKey() {
	key *k = (key*)malloc(sizeof(key));
	k->press = k->hold = k->release = false;
	return k;
}

void resetKey(key *k) {
	k->press = k->release = false;
}

bool start() {
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	
	//seleciona a cena inicial. se retornar false, deu algum erro ao carregar
	if (!sceneSelect(MENU)) {
		return false;
	}
	scene.tempo = -1;
	
	//inicia as teclas
	input.up = initKey();
	input.down = initKey();
	input.left = initKey();
	input.right = initKey();
	input.space = initKey();
	
	return true;
}

bool update() {
	//verifica as transições
	bool sceneLoaded = false;
	if (scene.tempo > 0) {
		scene.tempo -= DELTA*1.75;
		if (scene.tempo <= 0) {
			scene.tempo--;
			//se retornar false, deu algum erro ao carregar
			if (!sceneForceLoad(scene.nextScene)) {
				return false;
			}
			sceneLoaded = true;
			al_clear_to_color(al_map_rgb(0,0,0));
			al_flip_display();
		}
	} else if (scene.tempo < 0) {
		scene.tempo += DELTA*1.75;
		if (scene.tempo > 0) scene.tempo = 0;
	}
	
	//no caso de nenhuma cena nova ter sido chamada
	if (!sceneLoaded) {
		//updates da cena. se retornar false, o jogo é terminado
		if (!(*scene.update)()) {
			return false;
		}
		(*scene.draw)();
		
		//efeito de fade in/out
		if (scene.tempo > 0) {
			al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba_f(0,0,0,ease((1-scene.tempo)*1.125)));
		} else if (scene.tempo < 0) {
			al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba_f(0,0,0,ease(-scene.tempo*1.125)));
		}
		al_flip_display();
	}
	
	//reseta as teclas
	resetKey(input.up);
	resetKey(input.down);
	resetKey(input.left);
	resetKey(input.right);
	resetKey(input.space);
	
	return true;
}

int main() {
	//inicia tudo q o alegro precisa pra iniciar
	if (!al_init()) {
		fprintf(stderr,"erro: o allegro não pôde ser inicializado\n");
		return -1;
	}
	if (!al_install_mouse()) {
		fprintf(stderr,"erro: o mouse não pôde ser inicializado\n");
		return -1;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr,"erro: o teclado não pôde ser inicializado\n");
		return -1;
	}
	if (!al_init_primitives_addon()) {
		fprintf(stderr,"erro: o addon primitives não pôde ser inicializado\n");
		return -1;
	}
	if (!al_init_image_addon()) {
		fprintf(stderr,"erro: o addon image não pôde ser inicializado\n");
		return -1;
	}
	al_init_font_addon();
	al_init_ttf_addon();
	game.timer = al_create_timer(DELTA);
	if (!game.timer) {
		fprintf(stderr,"erro: o timer não pôde ser criado\n");
		return -1;
	}
	game.display = al_create_display(SCREEN_W,SCREEN_H);
	if (!game.display) {
		fprintf(stderr,"erro: o display não pôde ser criado\n");
		al_destroy_timer(game.timer);
		return -1;
	}
	game.eventQueue = al_create_event_queue();
	if (!game.eventQueue) {
		fprintf(stderr,"erro: a fila de eventos não pôde ser criada\n");
		al_destroy_timer(game.timer);
		al_destroy_display(game.display);
		return -1;
	}
	al_register_event_source(game.eventQueue,al_get_display_event_source(game.display));
	al_register_event_source(game.eventQueue,al_get_timer_event_source(game.timer));
	al_register_event_source(game.eventQueue,al_get_mouse_event_source());
	al_register_event_source(game.eventQueue,al_get_keyboard_event_source());
	
	//início do programa
	if (!load() || !start()) {
		unload();
		al_destroy_timer(game.timer);
		al_destroy_display(game.display);
		al_destroy_event_queue(game.eventQueue);
		return -1;
	}
	
	//update
	al_start_timer(game.timer);
	bool upd = true;
	while (1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(game.eventQueue,&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			upd = true;
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_KEY_UP) {
			key* k;
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP: k = input.up; break;
				case ALLEGRO_KEY_DOWN: k = input.down; break;
				case ALLEGRO_KEY_LEFT: k = input.left; break;
				case ALLEGRO_KEY_RIGHT: k = input.right; break;
				case ALLEGRO_KEY_SPACE: k = input.space; break;
				default: k = NULL; break;
			}
			if (k != NULL) {
				if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
					k->press = true;
					k->hold = true;
				} else {
					k->release = true;
					k->hold = false;
				}
			}
		}
		if (upd && al_is_event_queue_empty(game.eventQueue)) {
			upd = false;
			if (!update()) {
				break;
			}
		}
	}
	
	//fim do programa
	(*scene.unload)();
	unload();
	al_destroy_timer(game.timer);
	al_destroy_display(game.display);
	al_destroy_event_queue(game.eventQueue);
	return 0;
}
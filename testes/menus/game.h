#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#define LOADSPRITE(NAME,PATH) \
	al_set_path_filename(path,"data/sprites/" #PATH); \
	NAME = al_load_bitmap(al_path_cstr(path,'/')); \
	if (!NAME) { \
		fprintf(stderr,"erro: não foi possível carregar o bitmap em data/sprites/" #PATH "\n"); \
		return false; \
	}

#define UNLOADSPRITE(NAME) \
	if (NAME) al_destroy_bitmap(NAME)

#define LOADFONT(NAME,SIZE,PATH) \
	al_set_path_filename(path,"data/fonts/" #PATH); \
	NAME = al_load_ttf_font(al_path_cstr(path,'/'),SIZE,0); \
	if (!NAME) { \
		fprintf(stderr,"erro: não foi possível carregar a fonte em data/fonts/" #PATH "\n"); \
		return false; \
	}

#define FPS 60
#define DELTA 0.0166666666666667
#define SCREEN_W 960
#define SCREEN_H 540

struct Game {

	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *eventQueue;
	ALLEGRO_TIMER *timer;

} game;

struct Data {

	//fontes
	ALLEGRO_FONT *font_regular;
	ALLEGRO_FONT *font_mono;

	//bitmaps
	ALLEGRO_BITMAP *sprite_test;

} data;

typedef enum {

	MENU,
	SETTINGS,
	LEVEL

} SceneID;

struct Scene {

	//controle de transição
	float tempo;
	SceneID nextScene;
	bool exitRequest;

	//funções
	void (*unload)();
	void (*update)();
	void (*draw)();

} scene;

typedef struct key {
	bool press;
	bool hold;
	bool release;
} key;

void sceneLoad(SceneID id);

bool sceneSelect(SceneID id);

bool sceneForceLoad(SceneID id);

void exitGame();

struct Input {

	key *up;
	key *down;
	key *left;
	key *right;
	key *enter;
	key *backspace;
	
	char text[1024];
	bool captureText;
	bool captureFinish;
	bool textUpdate;
	int caretPos;

} input;

#endif
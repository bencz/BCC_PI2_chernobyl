#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#define LOADBITMAP(NAME,PATH) \
	al_set_path_filename(game.path,"data/bitmaps/" #PATH); \
	NAME = al_load_bitmap(al_path_cstr(game.path,'/')); \
	if (!NAME) { \
		fprintf(stderr,"erro: não foi possível carregar o bitmap em data/bitmaps/" #PATH "\n"); \
		return false; \
	}

#define UNLOADBITMAP(NAME) \
	if (NAME) al_destroy_bitmap(NAME)

#define LOADFONT(NAME,SIZE,PATH) \
	al_set_path_filename(game.path,"data/fonts/" #PATH); \
	NAME = al_load_ttf_font(al_path_cstr(game.path,'/'),SIZE,0); \
	if (!NAME) { \
		fprintf(stderr,"erro: não foi possível carregar a fonte em data/fonts/" #PATH "\n"); \
		return false; \
	}

#define UNLOADFONT(NAME) \
	if (NAME) al_destroy_font(NAME)

struct Game {

	ALLEGRO_DISPLAY *display; //janela
	ALLEGRO_EVENT_QUEUE *eventQueue; //fila de eventos
	ALLEGRO_TIMER *timer; //timer
	ALLEGRO_PATH *path; //caminho para o .exe
	
	int fwidth,fheight; //resolução da janela
	float idealProp; //proporção ideal da janela
	int width,height; //resolução com letterboxes
	int offsetx,offsety; //tamanho das letterboxes
	float fps,delta; //velocidade do timer

} game;

struct Data {
	
	//fontes
	ALLEGRO_FONT *font_Regular52;
	ALLEGRO_FONT *font_Regular37;
	ALLEGRO_FONT *font_Bold67;
	
	//bitmaps
	ALLEGRO_BITMAP *bitmap_test;
	ALLEGRO_BITMAP *bitmap_playerIdle;
	ALLEGRO_BITMAP *bitmap_tileset;
	
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
	bool showLetterbox;

	//funções
	void (*unload)();
	void (*update)();
	void (*draw)();

} scene;

void sceneLoad(SceneID id);
bool sceneSelect(SceneID id);
bool sceneForceLoad(SceneID id);
void exitGame();

#endif
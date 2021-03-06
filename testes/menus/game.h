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

#define BLENDDEFAULT() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA)

#define BLENDALPHA() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA)

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

int px(double x); //converte a porcentagem pro espaço da tela em pixels
int py(double y);

double ix(int x); //converte de pixels pra uma porcentagem de acordo com o espaço da tela
double iy(int y);

struct Data {
	
	//fontes
	ALLEGRO_FONT *font_UbuntuR;
	ALLEGRO_FONT *font_UbuntuB;
	
	//bitmaps
	ALLEGRO_BITMAP *bitmap_test;
	
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
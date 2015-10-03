#include <stdbool.h>

#include <math.h>

#include "input.h"
#include "game.h"
#include "menu.h"
#include "settings.h"
#include "level.h"

int px(double x) {
	return round(x*game.width+game.offsetx);
}

int py(double y) {
	return round(y*game.height+game.offsety);
}

float dx(double x) {
	return (float)(x*game.width+game.offsetx);
}

float dy(double y) {
	return (float)(y*game.height+game.offsety);
}

double ix(int x) {
	return (double)(x-game.offsetx)/game.width;
}

double iy(int y) {
	return (double)(y-game.offsety)/game.height;
}

void sceneLoad(SceneID id) {
	if (scene.tempo != 0) return;
	//prepara a transição para a cena escolhida
	scene.nextScene = id;
	scene.tempo = 1;
}

bool sceneSelect(SceneID id) {
	//carrega a cena escolhida.
	//se retornar false, deu algum problema ao carregar algo
	switch (id) {
		case MENU: return menu_start();
		case SETTINGS: return settings_start();
		case LEVEL: return level_start();
		default: return false;
	}
}

bool sceneForceLoad(SceneID id) {
	//descarrega a cena atual e coloca a cena nova, sem mexer na transição
	(*scene.unload)();
	return sceneSelect(id);
}

void exitGame() {
	if (scene.tempo != 0) return;
	//cria um efeito de transição mas para fechar a janela
	scene.exitRequest = true;
	scene.tempo = 1;
}
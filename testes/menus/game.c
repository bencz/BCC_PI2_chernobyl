#include <stdbool.h>

#include "game.h"
#include "menu.h"
#include "settings.h"
#include "level.h"

const float FPS = 60; //frame rate
const float DELTA = 1.0/60; //inversa
const int SCREEN_W = 960; //resolução em x
const int SCREEN_H = 540; //resolução em y

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
	}
}

bool sceneForceLoad(SceneID id) {
	//descarrega a cena atual e coloca a cena nova, sem mexer na transição
	(*scene.unload)();
	return sceneSelect(id);
}
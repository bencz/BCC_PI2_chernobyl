#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "game.h"
#include "ParserExpressao.h"

#define CACHE_MAX 1024

double functionCache[CACHE_MAX]; //cache
int cacheCount; //tamanho do cache
float xStart,xEnd,xGap; //"domínio" da função e distância entre cada x calculado
float xSize,ySize; //tamanho de cada unidade em pixels
float xOffset,yOffset; //deslocamento do ponto (0,0) do gráfico na tela
char functionString[1024]; //função digitada pelo usuário
int functionIndex; //temp: index da função escolhida, no caso só pra testar o plotador

//função para calcular os pontos

void calculatePoints() {
	cacheCount = 0;
	
	//temp: escolher a função
	switch (functionIndex) {
		case 0: strcpy(functionString,"x"); break;
		case 1: strcpy(functionString,"x*x"); break;
		case 2: strcpy(functionString,"1/x"); break;
	}
	
	for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
		setaValorDaVariavel("x",p);
		functionCache[cacheCount] = calcula(functionString);
	}
}

bool level_load();
void level_unload();
void level_update();
void level_draw();

bool level_start() {
	if (!level_load()) {
		return false;
	}
	scene.unload = &level_unload;
	scene.update = &level_update;
	scene.draw = &level_draw;

	//temp: valores quaisquer
	xGap = 1.0/32.0;
	xStart = -7.5;
	xEnd = 7.5;
	xSize = ySize = 64;
	xOffset = SCREEN_W/2;
	yOffset = SCREEN_H/2;
	functionIndex = 0;
	calculatePoints();

	return true;
}

bool level_load() {
	//carregar assets específicos da fase
	return true;
}

void level_unload() {
	//descarregar esses assets
}

void level_update() {
	if (scene.tempo <= 0) {
		if (input.space->press) {
			sceneLoad(MENU);
		}

		//temp: alternar entre as funções de teste
		if (input.up->press) {
			functionIndex--;
			if (functionIndex < 0) functionIndex = 2;
			calculatePoints();
		}
		if (input.down->press) {
			functionIndex++;
			if (functionIndex > 2) functionIndex = 0;
			calculatePoints();
		}
	}
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	ALLEGRO_COLOR colorButton = al_map_rgb(0,0,0);
	ALLEGRO_COLOR colorButton2 = al_map_rgb(255,0,51);

	//desenha os eixos
	al_draw_line(xOffset,0,xOffset,SCREEN_H,al_map_rgb(204,204,204),3);
	al_draw_line(0,yOffset,SCREEN_W,yOffset,al_map_rgb(204,204,204),3);

	//plota a função
	float xSizeN = xSize*xGap;
	float xOffsetN = xOffset+xStart*xSize;
	for (int a = 0; a < cacheCount-1; a++) {
		al_draw_line(
			xSizeN*a+xOffsetN,-ySize*functionCache[a]+yOffset,
			xSizeN*(a+1)+xOffsetN,-ySize*functionCache[a+1]+yOffset,
			al_color_hsv(360.0*a/(cacheCount-2),1,1),3
		);
	}

	//textos
	al_draw_textf(data.font_regular,colorButton,SCREEN_W/2,30,ALLEGRO_ALIGN_CENTRE,"fase (placeholder)");
	al_draw_textf(data.font_regular,colorButton2,SCREEN_W/2,SCREEN_H-60,ALLEGRO_ALIGN_CENTRE,"voltar");
}
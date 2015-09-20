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
int functionIndex; //temp: index da função escolhida, no caso só pra testar o plotador

//temp: funções de teste

double tempFunctionA(double x) {
	return x;
}

double tempFunctionB(double x) {
	return x*x;
}

double tempFunctionC(double x) {
	return 1.0/x;
}

//função para calcular os pontos

void calculatePoints() {
	cacheCount = 0;
	
	//temp: escolher a função
	float (*func)(float);
	switch (functionIndex) {
		case 0:
		{
			adicionaVariavelDoUsuario("p", 0);
			for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
				setaValorDaVariavel("p", p);
				functionCache[cacheCount] = calcula("p");
			}
		}		
		break;
		case 1:
		{
			adicionaVariavelDoUsuario("t", 0);
			for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
				setaValorDaVariavel("t", p);
				functionCache[cacheCount] = calcula("t*t");
			}
		}
		break;
		case 2:
		{
			adicionaVariavelDoUsuario("n", 1);
			for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
				setaValorDaVariavel("n", p);
				functionCache[cacheCount] = calcula("1.0/(n)");
			}
		}
		case 3:
		{
			adicionaVariavelDoUsuario("teste", 1);
			registraFuncaoDoUsuario("tempFunctionC", tempFunctionC);
			for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
				setaValorDaVariavel("teste", p);
				functionCache[cacheCount] = calcula("tempFunctionC(teste)");
			}
		}
		break;
	}
	
	/*for (float p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
		//functionCache[cacheCount] = (*func)(p);
	}*/
}

bool level_load();
void level_unload();
bool level_update();
void level_draw();

bool level_start() {
	if (!level_load()) {
		return false;
	}
	scene.unload = &level_unload;
	scene.update = &level_update;
	scene.draw = &level_draw;
	
	//temp: valores quaisquer
	xGap = 1.0/8.0;
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

bool level_update() {
	//temp: alternar entre as cenas
	if (input.left->press) {
		sceneLoad(SETTINGS);
	}
	
	//temp: alternar entre as funções de teste
	if (input.up->press) {
		functionIndex--;
		if (functionIndex < 0) functionIndex = 3;
		calculatePoints();
	}
	if (input.down->press) {
		functionIndex++;
		if (functionIndex > 3) functionIndex = 0;
		calculatePoints();
	}
	
	return true;
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,0,ALLEGRO_ALIGN_LEFT,"fase");
	al_draw_textf(data.font_regular,al_map_rgb(0,0,0),0,30,ALLEGRO_ALIGN_LEFT,"esquerda: configurações");
	
	//desenha os eixos
	al_draw_line(xOffset,0,xOffset,SCREEN_H,al_map_rgb(204,204,204),3);
	al_draw_line(0,yOffset,SCREEN_W,yOffset,al_map_rgb(204,204,204),3);
	
	//plota a função
	float xSizeN = xSize*xGap;
	float xOffsetN = xOffset+xStart*xSize;
	for (int a = 0; a < cacheCount-1; a++) {
		//daria pra fazer esse cálculo do posicionamento (size*n+offset) durante o cache,
		//mas estou assumindo que a lógica do jogo também use esse mesmo cache, e que ela
		//não vai rodar usando pixels como unidade, e sim algo como blocos.
		al_draw_line(
			xSizeN*a+xOffsetN,-ySize*functionCache[a]+yOffset,
			xSizeN*(a+1)+xOffsetN,-ySize*functionCache[a+1]+yOffset,
			al_map_rgb(255,0,0),3
		);
	}
}

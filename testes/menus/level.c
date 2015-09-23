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

//função para calcular os pontos

void calculatePoints() {
	cacheCount = 0;
	for (double p = xStart; cacheCount < CACHE_MAX && p <= xEnd; cacheCount++,p += xGap) {
		setaValorDaVariavel("x",p);
		functionCache[cacheCount] = calcula(input.text);
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
	
	input.text[0] = '\0';
	input.captureText = true;
	input.captureFinish = false;
	input.caretPos = 0;

	//temp: valores quaisquer
	xGap = 1.0/8.0;
	xStart = -7.5;
	xEnd = 7.5;
	xSize = ySize = 64;
	xOffset = SCREEN_W/2;
	yOffset = SCREEN_H/2-50;
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
		if (input.backspace->press) {
			sceneLoad(MENU);
		}
		if (input.enter->press && !input.captureText && !input.captureFinish) {
			input.captureText = true;
		}
		if (input.textUpdate) {
			//printf("ae\n");
			calculatePoints();
		}
		//calculatePoints();
	}
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));

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
	
	//textbox
	al_draw_filled_rectangle(0,SCREEN_H-100,SCREEN_W,SCREEN_H,al_map_rgb(51,51,51));
	int textboxOffsetX = 40;
	int textboxOffsetY = SCREEN_H-90;
	int textboxFontSize = 36;
	al_draw_textf(data.font_mono,al_map_rgb(255,255,255),textboxOffsetX,textboxOffsetY,ALLEGRO_ALIGN_LEFT,input.text);
	if (input.captureText) {
		//seria legal fazer a caret ficar piscando enquanto estiver parada
		al_draw_line(
			input.caretPos*textboxFontSize/2+textboxOffsetX+1,
			textboxOffsetY+4,
			input.caretPos*textboxFontSize/2+textboxOffsetX+1,
			textboxOffsetY+textboxFontSize-3,
			al_map_rgb(255,255,255),1
		);
		al_draw_textf(data.font_mono,al_map_rgb(204,204,204),textboxOffsetX-textboxFontSize/2-8,textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_textf(data.font_regular,al_map_rgb(255,255,255),SCREEN_W/2,SCREEN_H-40,ALLEGRO_ALIGN_CENTRE,"enter: fechar textbox");
	} else {
		al_draw_textf(data.font_mono,al_map_rgb(102,102,102),textboxOffsetX-textboxFontSize/2-8,textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_textf(data.font_regular,al_map_rgb(255,255,255),SCREEN_W/2,SCREEN_H-40,ALLEGRO_ALIGN_CENTRE,"enter: abrir textbox - backspace: voltar");
	}
}
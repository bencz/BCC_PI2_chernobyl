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
#include "ParserExpressao.h"

#define CACHE_MAX 1024

double functionCache[CACHE_MAX]; //cache
int cacheCount; //tamanho do cache
float xStart,xEnd,xGap; //"domínio" da função e distância entre cada x calculado
float xSize,ySize; //tamanho de cada unidade em pixels
float xOffset,yOffset; //deslocamento do ponto (0,0) do gráfico na tela

char textboxChar[2] = {'\0','\0'}; //usado para desenhar cada glifo do input

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
	scene.showLetterbox = true;
	
	input.text[0] = '\0';
	input.captureText = true;
	input.captureFinish = false;
	input.caretPos = 0;
	input.selectionStart = -1;

	//temp: valores quaisquer
	xGap = 1.0/8.0;
	xStart = -9;
	xEnd = 9;
	ySize = .1;
	xSize = ySize/game.idealProp;
	xOffset = .5;
	yOffset = .41;
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
			//aqui ainda precisa de algo q veja se a função é válida antes de replotá-la.
			//tb precisa de um analisador de domínio... q provavelmente vai ser complicado.
			//se removermos algumas funções (log, trigonométricas), fica mais fácil.
			//ah!! e precisa passar os erros do console pra janela do allegro depois.
			calculatePoints();
		}
	}
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	int weight = (int)round(game.height/180.0);
	if (weight < 1) weight = 1;
	
	//desenha os eixos
	al_draw_line(px(xOffset),py(0),px(xOffset),py(1),al_map_rgb(204,204,204),weight);
	al_draw_line(px(0),py(yOffset),px(1),py(yOffset),al_map_rgb(204,204,204),weight);

	//plota a função
	float xSizeN = xSize*xGap;
	float xOffsetN = xOffset+xStart*xSize;
	for (int a = 0; a < cacheCount-1; a++) {
		al_draw_line(
			px(xSizeN*a+xOffsetN),py(-ySize*functionCache[a]+yOffset),
			px(xSizeN*(a+1)+xOffsetN),py(-ySize*functionCache[a+1]+yOffset),
			al_color_hsv(360.0*a/(cacheCount-2),1,1),weight
		);
	}
	
	//textbox
	al_draw_filled_rectangle(px(0),py(.82),px(1),py(1),al_map_rgb(51,51,51));
	int textboxOffsetX = px(.04);
	int textboxOffsetY = py(.83);
	int selOffset;
	for (int a = 0; 1; a++) {
		if (input.captureText) {
			if (input.captureText && a == input.caretPos && input.caretBlink < .5f) {
				al_draw_line(
					textboxOffsetX,
					textboxOffsetY,
					textboxOffsetX,
					textboxOffsetY+al_get_font_line_height(data.font_UbuntuB),
					al_map_rgb(255,255,255),1
				);
			}
			if (input.selectionStart != -1 && input.selectionEnd == a) {
				al_draw_filled_rectangle(
					selOffset,
					textboxOffsetY,
					textboxOffsetX,
					textboxOffsetY+al_get_font_line_height(data.font_UbuntuB),
					al_map_rgba(51,51,51,51)
				);
			}
		}
		if (input.text[a] == '\0') break;
		if (input.selectionStart == a) {
			selOffset = textboxOffsetX;
		}
		textboxChar[0] = input.text[a];
		al_draw_textf(data.font_UbuntuB,al_map_rgb(255,255,255),textboxOffsetX,textboxOffsetY,ALLEGRO_ALIGN_LEFT,textboxChar);
		textboxOffsetX += al_get_text_width(data.font_UbuntuB,textboxChar);
	}
	if (input.captureText) {
		al_draw_textf(data.font_UbuntuB,al_map_rgb(204,204,204),px(.01),textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_textf(data.font_UbuntuR,al_map_rgb(255,255,255),px(.5),py(.925),ALLEGRO_ALIGN_CENTRE,"enter: fechar textbox");
	} else {
		al_draw_textf(data.font_UbuntuB,al_map_rgb(102,102,102),px(.01),textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_textf(data.font_UbuntuR,al_map_rgb(255,255,255),px(.5),py(.925),ALLEGRO_ALIGN_CENTRE,"enter: abrir textbox - backspace: voltar");
	}
}
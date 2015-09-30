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

const int mapWidth = 32;
const int mapHeight = 18;

int tilemap[] = {
//	1                    8                      16                      24                      32
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //1
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //9
	1, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //18
};

int baseX,baseY; //posição da base atual do a.i.-sama

double functionCache[CACHE_MAX]; //cache
int cacheCount; //tamanho do cache
double functionStart,functionEnd; //pontos onde a função é calculada no cache
double functionGap; //precisão do cálculo da função

double zeroHeight; //valor de f(0), usado para deslocar o plano cartesiano
double zeroHeightPrev; //valor anterior de zeroHeight
float zeroHeightTempo; //tempo da animação

int textboxPos;
bool errorMsgShow; //mostrar uma mensagem de erro ou não
int errorMsg; //índice da mensgem de erro

char textboxChar[2] = {'\0','\0'}; //usado para desenhar cada glifo do input

//função para calcular os pontos

void calculatePoints() {
	setaValorDaVariavel("x",0);
	double testNum = calcula(input.text);
	if (TemErro() && PegaCodigoErro() != ok) {
		if (PegaCodigoErro() == expressao_vazia) {
			errorMsgShow = false;
		} else {
			errorMsgShow = true;
			errorMsg = PegaCodigoErro();
		}
	} else {
		errorMsgShow = false;
		zeroHeightPrev = lerp(zeroHeight,zeroHeightPrev,zeroHeightTempo*zeroHeightTempo);
		zeroHeight = testNum;
		zeroHeightTempo = 1;
		cacheCount = 0;
		for (double p = functionStart; cacheCount < CACHE_MAX && p <= functionEnd; cacheCount++,p += functionGap) {
			setaValorDaVariavel("x",p);
			functionCache[cacheCount] = calcula(input.text);
		}
	}
}

void setBase(int x,int y) {
	baseX = x;
	baseY = y;
	if (x < 0) {
		functionStart = -x;
	} else {
		functionStart = 0;
	}
	functionEnd = mapWidth-x;
}

void showTextbox() {
	input.captureText = true;
	if (textboxPos) {
		if (baseY >= mapHeight-5) {
			textboxPos = 0;
		}
	} else {
		if (baseY <= 4) {
			textboxPos = 1;
		}
	}
}

void hideTextbox() {
	input.captureText = false;
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
	
	functionGap = 1.0/8.0; //menor o valor, maior a precisão
	
	cacheCount = 0;
	zeroHeight = zeroHeightPrev = 0;
	zeroHeightTempo = 0;
	
	setBase(3,8); //temp
	
	textboxPos = 1;
	showTextbox();
	input.text[0] = '\0';
	input.captureFinish = false;
	input.caretPos = 0;
	input.selectionStart = -1;

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
			showTextbox();
		}
		if (input.captureFinish) {
			hideTextbox();
		}
		if (input.textUpdate) {
			calculatePoints();
		}
		if (zeroHeightTempo > 0) {
			zeroHeightTempo -= game.delta*3;
			if (zeroHeightTempo < 0) {
				zeroHeightTempo = 0;
			}
		}
	}
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	int weight;
	if (game.height <= 180) {
		weight = 1;
	} else {
		weight = (int)round(game.height/180.0);
	}
	
	//inversa do tamanho do mapa, pra usar como porcentagem
	double scaleX = 1.0/mapWidth;
	double scaleY = 1.0/mapHeight;
	
	int t;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			t = tilemap[y*mapWidth+x];
			if (t == 0) {
				al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgb(51,51,51));
			} else if (t == 1) {
				al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgb(204,51,51));
			} else if (t == 2) {
				al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgb(51,204,51));
			}
		}
	}
	
	//posição do ponto 0 do gráfico
	double offsetX = scaleX*(baseX+1);
	double offsetY = scaleY*(baseY+1+lerp(zeroHeight,zeroHeightPrev,zeroHeightTempo*zeroHeightTempo));
	
	//desenha os eixos
	BLENDALPHA();
	ALLEGRO_COLOR axisColor = al_map_rgba(255,255,255,51);
	al_draw_line(px(offsetX),py(0),px(offsetX),py(1),axisColor,weight);
	al_draw_line(px(0),py(offsetY),px(1),py(offsetY),axisColor,weight);
	double gridPos;
	int gridOffset;
	gridOffset = floor(-offsetX/scaleX);
	while (1) {
		if (gridOffset == 0) {
			gridOffset++;
			continue;
		}
		gridPos = gridOffset*scaleX+offsetX;
		if (gridPos > 1) break;
		gridPos = px(gridPos);
		al_draw_line(gridPos,py(offsetY-scaleY*.125),gridPos,py(offsetY+scaleY*.125),axisColor,weight);
		gridOffset++;
	}
	gridOffset = floor(-offsetY/scaleY);
	while (1) {
		if (gridOffset == 0) {
			gridOffset++;
			continue;
		}
		gridPos = gridOffset*scaleY+offsetY;
		if (gridPos > 1) break;
		gridPos = py(gridPos);
		al_draw_line(px(offsetX-scaleX*.125),gridPos,px(offsetX+scaleX*.125),gridPos,axisColor,weight);
		gridOffset++;
	}
	BLENDDEFAULT();
	
	//plota a função
	double xSizeN = scaleX*functionGap;
	double xOffsetN = offsetX+functionStart*scaleX;
	for (int a = 0; a < cacheCount-1; a++) {
		al_draw_line(
			px(xSizeN*a+xOffsetN),py(-scaleY*functionCache[a]+offsetY),
			px(xSizeN*(a+1)+xOffsetN),py(-scaleY*functionCache[a+1]+offsetY),
			al_map_rgb(255,255,255),weight
		);
	}
	
	//textbox
	float textboxHeight = textboxPos?.8:0;
	BLENDALPHA();
	if (input.captureText) {
		al_draw_filled_rectangle(px(0),py(textboxHeight),px(1),py(.2+textboxHeight),al_map_rgba(255,255,255,204));
	} else {
		al_draw_filled_rectangle(px(0),py(textboxHeight),px(1),py(.2+textboxHeight),al_map_rgba(255,255,255,153));
	}
	BLENDDEFAULT();
	int textboxOffsetX = px(.04);
	int textboxOffsetY = py(.03+textboxHeight);
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
				BLENDALPHA();
				al_draw_filled_rectangle(
					selOffset,
					textboxOffsetY,
					textboxOffsetX,
					textboxOffsetY+al_get_font_line_height(data.font_UbuntuB),
					al_map_rgba(51,51,51,51)
				);
				BLENDDEFAULT();
			}
		}
		if (input.text[a] == '\0') break;
		if (input.selectionStart == a) {
			selOffset = textboxOffsetX;
		}
		textboxChar[0] = input.text[a];
		al_draw_text(data.font_UbuntuB,al_map_rgb(51,51,51),textboxOffsetX,textboxOffsetY,ALLEGRO_ALIGN_LEFT,textboxChar);
		textboxOffsetX += al_get_text_width(data.font_UbuntuB,textboxChar);
	}
	if (errorMsgShow) {
		al_draw_filled_triangle(
			px(.0135),py(.1797+textboxHeight),
			px(.0415),py(.1797+textboxHeight),
			px(.0275),py(.125+textboxHeight),
			al_map_rgb(255,204,15)
		);
		al_draw_text(data.font_UbuntuR,al_map_rgb(51,51,51),px(.0275),py(.127+textboxHeight),ALLEGRO_ALIGN_CENTRE,"!");
		al_draw_text(data.font_UbuntuR,al_map_rgb(51,51,51),px(.05),py(.125+textboxHeight),ALLEGRO_ALIGN_LEFT,MensagemDoErro[errorMsg]);
	}
	if (input.captureText) {
		al_draw_text(data.font_UbuntuB,al_map_rgb(51,51,51),px(.01),textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_text(data.font_UbuntuR,al_map_rgb(51,51,51),px(.99),py(.125+textboxHeight),ALLEGRO_ALIGN_RIGHT,"enter: fechar textbox");
	} else {
		al_draw_text(data.font_UbuntuB,al_map_rgb(102,102,102),px(.01),textboxOffsetY,ALLEGRO_ALIGN_LEFT,">");
		al_draw_text(data.font_UbuntuR,al_map_rgb(51,51,51),px(.99),py(.125+textboxHeight),ALLEGRO_ALIGN_RIGHT,"enter: abrir textbox - backspace: voltar");
	}
}
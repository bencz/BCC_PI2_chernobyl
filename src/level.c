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
#include "draw.h"
#include "parserexpressao.h"

#define CACHE_MAX 2200

const int mapWidth = 32;
const int mapHeight = 18;
const double playerRadius = 2.0/3.0;
const bool debugCollision = true;

int tilemap[] = {
//	1                    8                      16                      24                      32
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //1
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //9
	1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //18
};

int baseX,baseY; //posição da base atual do a.i.-sama

double functionCache[CACHE_MAX]; //cache
double functionCachePrev[CACHE_MAX]; //cache anterior, para usar na animação
int cacheCount; //tamanho do cache
int functionDir; //direção da função, 1 se x cresce, -1 se x decresce
double functionStart,functionEnd; //pontos onde a função é calculada no cache
double functionGap; //precisão do cálculo da função
bool functionPlot; //diz se a função será plotada ou não

float plotTempo; //tempo da animação do gráfico de um cache para o outro
float dottedTempo; //tempo da animação do tracejado do gráfico
float dottedAcc; //aceleração da animação do tracejado do gráfico
float weightTempo; //tempo da animação da grossura do gráfico
double zeroHeight; //valor de f(0), usado para deslocar o plano cartesiano
double zeroHeightPrev; //valor anterior de zeroHeight
float zeroHeightTempo; //tempo da animação da altura

int textboxPos; //posição da caixa de texto (0 = cima, 1 = baixo)
float textboxPosTempo; //posição da caixa de texto, animada
float textboxSizeTempo; //tamanho da caixa de texto
bool errorMsgShow; //mostrar uma mensagem de erro ou não
int errorMsg; //índice da mensagem de erro

double playerX,playerY,playerPrevY; //posição do jogador
double playerSpriteX,playerSpriteY; //posição do sprite do jogador
int playerSequence; //índice da animação do jogador
float playerFrame; //índice do frame da animação
bool moving; //tá movendo
bool dead; //tá morrendo
float respawnTempo; //tempo de respawn, pra animação dele piscando
float baseTempo; //tempo pra animação do lerp do player indo pra base

char textboxChar[2] = {'\0','\0'}; //usado para desenhar cada glifo do input

int getTile(int x,int y) {
	return tilemap[x+y*mapWidth];
}

int getTileSafe(int x,int y) {
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return -1;
	return tilemap[x+y*mapWidth];
}

void calculatePoints(bool reset) {
	double p = 0;
	double resultado = 0;
	int flag = 0,errorCode = 10;
	setavariavel("x",&p);
	errorCode = calcula(input.text,&resultado,&flag);
	if (errorCode != E_OK) {
		functionPlot = false;
		errorMsgShow = 1;
		errorMsg = errorCode-1;
	} else {
		if (!reset && weightTempo > 0) {
			plotTempo = easeIn(plotTempo);
			for (int a = 0; a < cacheCount; a++) {
				functionCachePrev[a] = dlerp(functionCache[a],functionCachePrev[a],plotTempo);
			}
			plotTempo = 1;
		} else {
			plotTempo = 0;
		}
		functionPlot = true;
		errorMsgShow = false;
		zeroHeightPrev = lerp(zeroHeight,zeroHeightPrev,zeroHeightTempo*zeroHeightTempo);
		zeroHeight = resultado;
		zeroHeightTempo = 1;
		cacheCount = 0;
		for (p = functionStart; cacheCount < CACHE_MAX && p <= functionEnd; cacheCount++,p += functionGap) {
			setavariavel("x",&p);
			errorCode = calcula(input.text,&resultado,&flag);
			functionCache[cacheCount] = resultado;
		}
	}
}

double getValueOnCache(double x) {
	double ind = (x-functionStart)/functionGap;
	int l = floor(ind);
	if (l >= cacheCount-1) {
		return functionCache[cacheCount-1];
	}
	int h = ceil(ind);
	if (h <= 0) {
		return functionCache[0];
	}
	return dlerp(functionCache[l],functionCache[h],fmod(ind,1));
}

double getValueOnCacheLerp(double x,float t) {
	if (t <= 0) {
		return getValueOnCache(x);
	}
	double ind = (x-functionStart)/functionGap;
	int l = floor(ind);
	if (l >= cacheCount-1) {
		return dlerp(functionCache[cacheCount-1],functionCachePrev[cacheCount-1],t);
	}
	int h = ceil(ind);
	if (h <= 0) {
		return dlerp(functionCache[0],functionCachePrev[0],t);
	}
	ind = fmod(ind,1);
	return dlerp(
		dlerp(functionCache[l],functionCache[h],ind),
		dlerp(functionCachePrev[l],functionCachePrev[h],ind),
		t
	);
}

void setDir(int d) {
	functionDir = (d > 0)?1:-1;
}

void setBase(int x,int y) {
	if (getTileSafe(x-1,y-1) == 2) {
		setBase(x-1,y-1); return;
	}
	if (getTileSafe(x-1,y) == 2) {
		setBase(x-1,y); return;
	}
	if (getTileSafe(x,y-1) == 2) {
		setBase(x,y-1); return;
	}
	baseX = x;
	baseY = y;
	playerX = x;
	playerY = y;
	setDir(functionDir);
	functionStart = -baseX-1.5;
	functionEnd = mapWidth-baseX-.5;
}

bool checkBase(int x,int y) {
	if (getTileSafe(x-1,y-1) == 2) return checkBase(x-1,y-1);
	if (getTileSafe(x-1,y) == 2) return checkBase(x-1,y);
	if (getTileSafe(x,y-1) == 2) return checkBase(x,y-1);
	if (baseX == x && baseY == y) return false;
	return true;
}

void closeTextbox() {
	input.captureText = false;
}

void stopMoving() {
	respawnTempo = 1;
	moving = false;
	dead = false;
}

void startMoving() {
	if (input.captureText) closeTextbox();
	moving = true;
	respawnTempo = 0;
}

void openTextbox() {
	if (moving) stopMoving();
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
	if (textboxSizeTempo == 0) {
		textboxPosTempo = textboxPos;
	}
}

int collideStep(int j,int m,int n,int *x,int *y) {
	int i;
	int t,b = -1;
	for (i = m; i <= n; i++) {
		t = getTile(i,j);
		if (t == 1) {
			*x = i;
			*y = j;
			return 1;
		}
		if (b == -1 && t == 2 && checkBase(i,j)) {
			b = i;
		}
	}
	if (b != -1) {
		*x = b;
		*y = j;
		return 2;
	}
	return 0;
}

int collide(double delta,int *x,int *y,int debug) {
	int i,j;
	int left = floor(playerX-playerRadius)+1;
	int right = ceil(playerX+playerRadius);
	if (left < 0) left = 0;
	if (right >= mapWidth) right = mapWidth-1;
	int top,bot;
	if (delta > 0) {
		top = floor(playerY-playerRadius-delta)+1;
		bot = ceil(playerY+playerRadius);
		if (top < 0) top = 0;
		if (bot >= mapHeight) bot = mapHeight-1;
		if (debug) {
			BLENDALPHA();
			al_draw_filled_rectangle(px(left/32.0),py(top/18.0),px((right+1)/32.0),py((bot+1)/18.0),al_map_rgba(255,0,0,100));
			BLENDDEFAULT();
		} else for (i = top; i <= bot; i++) {
			j = collideStep(i,left,right,x,y);
			if (j) return j;
		}
	} else {
		top = floor(playerY-playerRadius)+1;
		bot = ceil(playerY+playerRadius-delta);
		if (top < 0) top = 0;
		if (bot >= mapHeight) bot = mapHeight-1;
		if (debug) {
			BLENDALPHA();
			al_draw_filled_rectangle(px(left/32.0),py(top/18.0),px((right+1)/32.0),py((bot+1)/18.0),al_map_rgba(255,0,0,100));
			BLENDDEFAULT();
		} else for (i = bot; i >= top; i--) {
			j = collideStep(i,left,right,x,y);
			if (j) return j;
		}
	}
	return 0;
}

//
//
//

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
	
	functionDir = 1;
	functionGap = 1.0/64.0; //menor o valor, maior a precisão
	functionPlot = false;
	
	cacheCount = 0;
	plotTempo = 0;
	dottedTempo = 0;
	dottedAcc = 1;
	weightTempo = 0;
	zeroHeight = zeroHeightPrev = 0;
	zeroHeightTempo = 0;
	
	setBase(3,8); //temp
	
	playerSequence = 0;
	playerFrame = 0;
	
	moving = false;
	dead = false;
	respawnTempo = 1;
	baseTempo = 0;
	
	textboxPos = 1;
	textboxPosTempo = 1;
	textboxSizeTempo = 0;
	
	stopMoving();
	closeTextbox();
	input.text[0] = '\0';
	input.captureFinish = false;
	input.caretPos = 0;
	input.selectionStart = -1;
	
	return true;
}

bool level_load() {
	LOADBITMAP(data.bitmap_playerIdle,playerIdle.png);
	return true;
}

void level_unload() {
	UNLOADBITMAP(data.bitmap_playerIdle);
}

void level_update() {
	//keypresses
	if (scene.tempo <= 0) {
		if (input.escape->press) {
			sceneLoad(MENU);
		}
		if (input.tab->press) {
			setDir(-functionDir);
		}
		if (moving) {
			if (input.enter->press || input.space->press) {
				stopMoving();
				baseTempo = 0;
				playerX = baseX;
				playerY = baseY;
			}
			if (input.enter->press) {
				openTextbox();
			}
		} else {
			if (input.captureFinish) {
				closeTextbox();
			}
			if (input.textUpdate) {
				calculatePoints(false);
			}
			if (input.captureText) {
				if (input.up->press) textboxPos = 0;
				if (input.down->press) textboxPos = 1;
			} else {
				if (input.enter->press) {
					if (!input.captureFinish) openTextbox();
				} else if (input.space->press && functionPlot) {
					startMoving();
				}
			}
		}
	}
	
	//animação do gráfico
	if (zeroHeightTempo > 0) {
		zeroHeightTempo -= game.delta*3;
		if (zeroHeightTempo < 0) {
			zeroHeightTempo = 0;
		}
	}
	if (functionDir > 0) {
		if (dottedAcc < 1) {
			dottedAcc += game.delta*16;
			if (dottedAcc > 1) dottedAcc = 1;
		}
	} else {
		if (dottedAcc > -1) {
			dottedAcc -= game.delta*16;
			if (dottedAcc < -1) dottedAcc = -1;
		}
	}
	dottedTempo += game.delta*3*dottedAcc;
	if (dottedAcc > 0) {
		while (dottedTempo >= 1) {
			dottedTempo -= 1;
		}
	} else {
		while (dottedTempo < 0) {
			dottedTempo += 1;
		}
	}
	if (functionPlot) {
		if (weightTempo < 1) {
			weightTempo += game.delta*8;
			if (weightTempo > 1) weightTempo = 1;
		}
	} else {
		if (weightTempo > 0) {
			weightTempo -= game.delta*2;
			if (weightTempo < 0) weightTempo = 0;
		}
	}
	if (plotTempo > 0) {
		if (weightTempo > 0) {
			plotTempo -= game.delta*8;
			if (plotTempo < 0) plotTempo = 0;
		} else {
			plotTempo = 0;
		}
	}
	
	//animação do jogador
	if (respawnTempo > 0) {
		respawnTempo -= game.delta;
		if (respawnTempo < 0) respawnTempo = 0;
	}
	if (moving) {
		if (baseTempo < 1) {
			baseTempo += game.delta*4;
			if (baseTempo > 1) baseTempo = 1;
		}
	} else {
		if (baseTempo > 0) {
			baseTempo -= game.delta*1.5;
			if (baseTempo < 0) baseTempo = 0;
		}
	}
	playerFrame += game.delta*20;
	while (playerFrame >= 20) {
		playerFrame -= 20;
	}
	
	//movimentação do jogador
	if (moving && !dead) {
		float acc = 1;
		if (input.left->hold) {
			if (!(input.right->hold)) {
				if (functionDir < 0) acc = 1.75f;
				else acc = .5f;
			}
		} else {
			if (input.right->hold) {
				if (functionDir > 0) acc = 1.75f;
				else acc = .5f;
			}
		}
		playerX += game.delta*2.5*baseTempo*dottedAcc*acc;
		playerPrevY = playerY;
		playerY = zeroHeight-getValueOnCache(playerX-baseX)+baseY;
		playerSpriteX = lerp(playerSpriteX,playerX,baseTempo);
		playerSpriteY = lerp(playerSpriteY,playerY,baseTempo);
		int x,y;
		int collision = collide((playerY-playerPrevY),&x,&y,0);
		if (collision == 1) {
			if (fabs(playerSpriteY-y) >= 2) {
				playerSpriteY = y;
			}
			if (playerSpriteY < 0) playerSpriteY = 0;
			if (playerSpriteY > 16) playerSpriteY = 16;
			dead = true;
			respawnTempo = 2.5;
		} else if (collision == 2) {
			setBase(x,y);
			stopMoving();
			calculatePoints(true);
			baseTempo = 1;
			respawnTempo = 0;
		}
	}
	if (dead) {
		if (respawnTempo <= 1) {
			stopMoving();
			playerX = playerSpriteX = baseX;
			playerY = playerSpriteY = baseY;
		} else {
			playerSpriteY += game.delta*7*(2.2-respawnTempo);
		}
	}
	if (!moving && !dead) {
		playerSpriteX = lerp(playerX,playerSpriteX,baseTempo);
		playerSpriteY = lerp(playerY,playerSpriteY,baseTempo);
	}
	
	//textbox
	if (input.captureText) {
		if (textboxSizeTempo < 1) {
			textboxSizeTempo += game.delta*5;
			if (textboxSizeTempo > 1) textboxSizeTempo = 1;
		}
	} else {
		if (textboxSizeTempo > 0) {
			textboxSizeTempo -= game.delta*5;
			if (textboxSizeTempo < 0) textboxSizeTempo = 0;
		}
	}
	if (textboxSizeTempo > 0) {
		if (textboxPos) {
			if (textboxPosTempo < 1) {
				textboxPosTempo += game.delta*2.5;
				if (textboxPosTempo > 1) textboxPosTempo = 1;
			}
		} else {
			if (textboxPosTempo > 0) {
				textboxPosTempo -= game.delta*2.5;
				if (textboxPosTempo < 0) textboxPosTempo = 0;
			}
		}
	} else {
		textboxPosTempo = textboxPos;
	}
}

void level_draw() {
	al_clear_to_color(al_map_rgb(255,255,255));
	float weight,weight2;
	if (game.height <= 180) {
		weight = 1;
	} else {
		weight = round(game.height/180.0);
	}
	if (game.height <= 320) {
		weight2 = 1;
	} else {
		weight2 = round(game.height/320.0);
	}
	
	//inversa do tamanho do mapa, pra usar como porcentagem
	double scaleX = 1.0/mapWidth;
	double scaleY = 1.0/mapHeight;
	
	//desenha o tilemap
	al_draw_filled_rectangle(px(0),py(0),px(1),py(1),al_map_rgb(51,51,51));
	for (int t,y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			t = getTile(x,y);
			if (t == 1) {
				al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgb(204,51,51));
			} else if (t == 2) {
				al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgb(51,204,51));
			}
		}
	}
	
	//posição do ponto 0 do gráfico
	double zeroHeightEase = lerp(zeroHeight,zeroHeightPrev,easeIn(zeroHeightTempo));
	double offsetX = scaleX*(baseX+1);
	double offsetY = scaleY*(baseY+1+zeroHeightEase);
	double offY = fmod(zeroHeightEase,1);
	
	//desenha os eixos
	BLENDALPHA();
	ALLEGRO_COLOR axisColor = al_map_rgba(255,255,255,(int)lerp(25,76,textboxSizeTempo));
	if (textboxSizeTempo > 0) {
		ALLEGRO_COLOR gridColor = al_map_rgba(255,255,255,(int)(textboxSizeTempo*4));
		int r;
		double offsetY = lerp(zeroHeight,zeroHeightPrev,easeIn(zeroHeightTempo));
		for (int x = 0; x <= mapWidth; x++) {
			r = px((double)x/mapWidth);
			al_draw_line(r,py(0),r,py(1),gridColor,weight2);
		}
		for (int y = -1; y <= mapHeight; y++) {
			r = py((double)(y+offY)/mapHeight);
			al_draw_line(px(0),r,px(1),r,gridColor,weight2);
		}
	}
	al_draw_line(px(offsetX),py(0),px(offsetX),py(1),axisColor,weight);
	al_draw_line(px(0),py(offsetY),px(1),py(offsetY),axisColor,weight);
	double gridPos;
	int gridOffset;
	gridOffset = floor(-offsetX/scaleX);
	while (1) {
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
	
	//desenha o guri
	bool blink = respawnTempo > .5 && respawnTempo <= 1.75 && (int)ceilf(respawnTempo*10)&1;
	if (!blink) {
		int cx,cy;
		ALLEGRO_BITMAP *bm;
		switch (playerSequence) {
			case 0: cx = 5; cy = 4; bm = data.bitmap_playerIdle; break;
			default: bm = NULL;
		}
		if (bm != NULL) {
			drawSpriteSheet(bm,(playerSpriteX+1)*scaleX,(playerSpriteY+1)*scaleY,scaleY*2,scaleY*2,cx,cy,(int)playerFrame,0,0,0);
			if (debugCollision) {
				al_draw_rectangle(
					px((playerSpriteX+1-playerRadius)*scaleX),py((playerSpriteY+1-playerRadius)*scaleY),
					px((playerSpriteX+1+playerRadius)*scaleX),py((playerSpriteY+1+playerRadius)*scaleY),
					al_map_rgb(255,51,0),weight
				);
				if (moving && !dead) {
					int x,y;
					collide(playerY-playerPrevY,&x,&y,1);
				}
			}
		}
	}
	
	//plota a função
	if (weightTempo > 0 && cacheCount > 0) {
		float t = easeIn(plotTempo);
		float w = easeOut(weightTempo);
		ALLEGRO_COLOR plotColor = al_map_rgba(255,255,255,(int)lerp(153,255,textboxSizeTempo));
		w = ceil(w*weight);
		BLENDALPHA();
		for (double x = functionStart+dottedTempo*.25; x < functionEnd; x += .25) {
			al_draw_line(
				dx(offsetX+x*scaleX),dy(offsetY-getValueOnCacheLerp(x,t)*scaleY),
				dx(offsetX+(x-.125)*scaleX),dy(offsetY-getValueOnCacheLerp(x-.125,t)*scaleY),
				plotColor,w
			);
		}
		BLENDDEFAULT();
	}

	//textbox
	if (textboxSizeTempo > 0) {
		float textboxHeight = lerp(.08,.82,ease(textboxPosTempo));
		BLENDALPHA();
		float textboxSizeEase = easeIn(1-textboxSizeTempo);
		al_draw_filled_rectangle(px(.046875),py(textboxHeight+.05*textboxSizeEase),px(.953125),py(.1+textboxHeight-.05*textboxSizeEase),al_map_rgba(255,255,255,204));
		BLENDDEFAULT();
		if (textboxSizeTempo >= .5f) {
			int textboxOffsetX = px(.16);
			int textboxOffsetY = py(.01+textboxHeight);
			int selOffset;
			for (int a = 0; 1; a++) {
				if (a == input.caretPos && input.caretBlink < .5f) {
					al_draw_line(
						textboxOffsetX,
						textboxOffsetY,
						textboxOffsetX,
						textboxOffsetY+al_get_font_line_height(data.font_Bold67),
						al_map_rgb(102,102,102),1
					);
				}
				if (input.selectionStart != -1 && input.selectionEnd == a) {
					BLENDALPHA();
					al_draw_filled_rectangle(
						selOffset,
						textboxOffsetY,
						textboxOffsetX,
						textboxOffsetY+al_get_font_line_height(data.font_Bold67),
						al_map_rgba(51,51,51,51)
					);
					BLENDDEFAULT();
				}
				if (input.text[a] == '\0') break;
				if (input.selectionStart == a) {
					selOffset = textboxOffsetX;
				}
				textboxChar[0] = input.text[a];
				al_draw_text(data.font_Bold67,al_map_rgb(51,51,51),textboxOffsetX,textboxOffsetY,ALLEGRO_ALIGN_LEFT,textboxChar);
				textboxOffsetX += al_get_text_width(data.font_Bold67,textboxChar);
			}
			al_draw_text(data.font_Bold67,al_map_rgb(102,102,102),px(.055),textboxOffsetY,ALLEGRO_ALIGN_LEFT,"f(x) = ");
		}
	}
	
	//footer
	float footerHeight = .825f;
	if (errorMsgShow) {
		al_draw_filled_triangle(
			px(.009),py(.165+footerHeight),
			px(.031),py(.165+footerHeight),
			px(.02),py(.125+footerHeight),
			al_map_rgb(255,204,15)
		);
		al_draw_text(data.font_Regular37,al_map_rgb(51,51,51),px(.02),py(.127+footerHeight),ALLEGRO_ALIGN_CENTRE,"!");
		al_draw_text(data.font_Regular37,al_map_rgb(51,51,51),px(.032),py(.125+footerHeight),ALLEGRO_ALIGN_LEFT,mensagensDeErro[errorMsg]);
	}
	if (moving) {
		al_draw_text(
			data.font_Regular37,al_map_rgb(51,51,51),px(.99),py(.125+footerHeight),ALLEGRO_ALIGN_RIGHT,
			"tab: inverter x - espaço/enter: desfazer"
		);
	} else if (input.captureText) {
		al_draw_text(
			data.font_Regular37,al_map_rgb(51,51,51),px(.99),py(.125+footerHeight),ALLEGRO_ALIGN_RIGHT,
			"tab: inverter x - enter: fechar texto"
		);
	} else {
		al_draw_text(
			data.font_Regular37,al_map_rgb(51,51,51),px(.99),py(.125+footerHeight),ALLEGRO_ALIGN_RIGHT,
			"espaço: iniciar - tab: inverter x - enter: abrir texto"
		);
	}
}

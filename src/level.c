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
#include "map.h"
#include "lex.h"

#define CACHE_MAX 2200

const double playerRadius = 2.0/3.0; //metade da largura do jogador
const bool debugCollision = false; //se for true, mostra hitbox e destaca tiles usadas para colisão
const double scaleX = 1.0/mapWidth; //inversa da largura do mapa, para usar como porcentagem
const double scaleY = 1.0/mapHeight; //igual, mas da altura
const float parallaxIntensity = 2.0/3.0; //intensidade do movimento do parallax. 1 ele move normal, 0 ele n move
const int tilemapWidth = 5; //largura do tilemap.png, em tiles
const int tilemapHeight = 1; //altura do tilemap.png, em tiles

TMap *currentMap; //mapa da fase atual
TMap *prevMap; //mapa da fase anterior, para ser mostrado na transição entre duas telas
int mapX,mapY; //índice do mapa atual na grade de mapas
TBase *currentBase; //base na qual o jogador se encontra atualmente

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
bool wire; //tá no circuito
float respawnTempo; //tempo de respawn, pra animação dele piscando
float baseTempo; //tempo pra animação do lerp do player indo pra base

TWire *wireNow; //cabo pelo qual ele tá passando
int wireDir; //direção pela qual o jogador tá indo pelo cabo
float wireTempo; //pra animação dele entrando/saindo do circuito
float wireProgress; //caminho que ele tá fazendo
int wireIndex; //parte do circuito na qual ele se encontra
float wireLength; //comprimento de tal parte

float mapTempo; //animação de transição entre mapas
int mapDirX,mapDirY; //direção da animação

float weightRegular,weightThin,weightThick; //grossuras para desenhar linhas
char textboxChar[2] = {'\0','\0'}; //usado para desenhar cada glifo do input

// mantenho a variavel 'lextemp' alocada diretamente no código ( um array )
unsigned *char lextemp[2048];

int getTile(int *t,int x,int y) {
	return t[x+y*mapWidth];
}

int getTileSafe(int *t,int x,int y) {
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return -1;
	return t[x+y*mapWidth];
}

void calculatePoints(bool reset) {
	double p = 0;
	double resultado = 0;
	int flag = 0,errorCode = 10;
	
	/* teste da nova coisa */
	memset(lextemp, '\0', 2048];
    memcpy(lextemp, expr, strlen(input.text));
    analiselexica(lextemp, 0);
    memset(lextemp, '\0', 2048);
    processaexpressao(lextemp, 0);
	/* fim da nova area de teste! */
	
	setavariavel("x",&p);
	errorCode = calcula(lextemp,&resultado,&flag);
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
			errorCode = calcula(lextemp,&resultado,&flag);
			functionCache[cacheCount] = resultado;
		}
	}
	
	free(tmp);
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

TBase *getBase(int x,int y) {
	if (getTileSafe(currentMap->collision,x+1,y+1) == 2) {
		return getBase(x+1,y+1);
	}
	if (getTileSafe(currentMap->collision,x+1,y) == 2) {
		return getBase(x+1,y);
	}
	if (getTileSafe(currentMap->collision,x,y+1) == 2) {
		return getBase(x,y+1);
	}
	for (int a = 0; a < currentMap->basesN; a++) {
		if (currentMap->bases[a].x == x && currentMap->bases[a].y == y) {
			return &currentMap->bases[a];
		}
	}
	return NULL;
}

void setBase(TBase *base) {
	currentBase = base;
	playerX = base->x;
	playerY = base->y;
	setDir(functionDir);
	functionStart = -(base->x)-.5;
	functionEnd = mapWidth-(base->x)+.5;
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
		if (currentBase->y >= mapHeight-5) {
			textboxPos = 0;
		}
	} else {
		if (currentBase->y <= 4) {
			textboxPos = 1;
		}
	}
	if (textboxSizeTempo == 0) {
		textboxPosTempo = textboxPos;
	}
}

void startCircuit(TWire *w,int d) {
	if (w == NULL) return;
	if (w->base0 < -1 || w->base1 < -1) return;
	wire = true;
	wireProgress = 0;
	wireTempo = 0;
	wireDir = d;
	wireIndex = (d > 0)?0:(w->n-2);
	wireNow = w;
	wireLength = getWireLength(wireNow,wireIndex);
	if (wireDir > 0) {
		playerX = wireNow->nodes[0].x;
		playerY = wireNow->nodes[0].y;
	} else {
		playerX = wireNow->nodes[wireNow->n-1].x;
		playerY = wireNow->nodes[wireNow->n-1].y;
	}
}

void moveMap(int x,int y) {
	TMap *temp = currentMap;
	currentMap = prevMap;
	prevMap = temp;
	mapTempo = 1;
	mapDirX = x;
	mapDirY = y;
	mapX += x;
	mapY += y;
	freeMap(currentMap);
	loadMap(currentMap,mapX,mapY);
	TWire *wire;
	int2 *node;
	for (int a = 0; a < currentMap->wiresN; a++) {
		if (currentMap->wires[a].n <= 1) continue;
		wire = &currentMap->wires[a];
		node = wire->nodes;
		if (fabs(node->x-playerX) < .5 && fabs(node->y-playerY) < .5) {
			startCircuit(wire,1);
			break;
		}
		node = &wire->nodes[wire->n-1];
		if (fabs(node->x-playerX) < .5 && fabs(node->y-playerY) < .5) {
			startCircuit(wire,-1);
			break;
		}
	}
}

int collideStep(int j,int m,int n,int *x,int *y) {
	int i;
	int t,b = -1;
	for (i = m; i < n; i++) {
		t = getTile(currentMap->collision,i,j);
		if (t == 1) {
			*x = i;
			*y = j;
			return 1;
		}
		if (b == -1 && t == 2) {
			TBase *base = getBase(i,j);
			if (base != NULL && base != currentBase) {
				b = i;
			}
		}
	}
	if (b != -1) {
		*x = b;
		*y = j;
		return 2;
	}
	return 0;
}

int collide(double delta,int *x,int *y,bool debug) {
	int i,j;
	int left = floor(playerX-playerRadius);
	int right = ceil(playerX+playerRadius);
	if (left < 0) left = 0;
	if (right > mapWidth) right = mapWidth;
	int top,bot;
	if (delta > 0) {
		top = floor(playerY-playerRadius-delta);
		bot = ceil(playerY+playerRadius);
		if (top < 0) top = 0;
		if (bot > mapHeight) bot = mapHeight;
		//printf("%dx%d -> %dx%d\n",left,top,right,bot);
		if (debug) {
			BLENDALPHA();
			al_draw_filled_rectangle(px(left/32.0),py(top/18.0),px(right/32.0),py(bot/18.0),al_map_rgba(255,0,0,100));
			BLENDDEFAULT();
		} else for (i = top; i < bot; i++) {
			j = collideStep(i,left,right,x,y);
			if (j) return j;
		}
	} else {
		top = floor(playerY-playerRadius);
		bot = ceil(playerY+playerRadius-delta);
		if (top < 0) top = 0;
		if (bot > mapHeight) bot = mapHeight;
		//printf("%dx%d -> %dx%d\n",left,top,right,bot);
		if (debug) {
			BLENDALPHA();
			al_draw_filled_rectangle(px(left/32.0),py(top/18.0),px(right/32.0),py(bot/18.0),al_map_rgba(255,0,0,100));
			BLENDDEFAULT();
		} else for (i = bot-1; i >= top; i--) {
			j = collideStep(i,left,right,x,y);
			if (j) return j;
		}
	}
	if (top == 0 && bot == mapHeight) {
		if (delta < 0) {
			*y = 1;
		} else {
			*y = mapHeight-1;
		}
		return 1;
	}
	if (top == 0) {
		*y = 1;
		return 1;
	}
	if (bot == mapHeight) {
		*y = mapHeight-1;
		return 1;
	}
	if (left == 0 || right == mapWidth) {
		*y = (int)playerY;
		return 1;
	}
	return 0;
}

void drawTileset(int* tileset,float ox,float oy,float mx,float Mx,float my,float My) {
	float nx,ny;
	for (int t,x,y = 0; y < mapHeight; y++) {
		ny = oy+(y+.5f)*scaleY;
		if (ny >= my && ny <= My) {
			for (x = 0; x < mapWidth; x++) {
				t = getTile(tileset,x,y);
				if (t < 0) continue;
				nx = ox+(x+.5f)*scaleX;
				if (nx >= mx && nx <= Mx) {
					drawSpriteSheet(data.bitmap_tileset,nx,ny,scaleY,scaleY,tilemapWidth,tilemapHeight,t,0,0,0);
				}
			}
		}
	}
}

void drawMap(TMap *map,float ox,float oy,bool drawPlayer) {
	al_draw_filled_rectangle(px(ox),py(oy),px(ox+1),py(oy+1),al_map_rgb(0,0,0));
	float mx,Mx,my,My,pmx,pMx,pmy,pMy;
	if (ox > 0) {
		mx = ox;
		pMx = Mx = 1+scaleX;
		pmx = mx+scaleX*.5f;
	} else if (ox < 0) {
		pmx = mx = -scaleX;
		Mx = 1+ox;
		pMx = Mx-scaleX*.5f;
	} else {
		pmx = mx = -scaleX;
		pMx = Mx = 1+scaleX;
	}
	if (oy > 0) {
		my = oy;
		pMy = My = 1+scaleY;
		pmy = my+scaleY*.5f;
	} else if (oy < 0) {
		pmy = my = -scaleY;
		My = 1+oy;
		pMy = My-scaleY*.5f;
	} else {
		pmy = my = -scaleY;
		pMy = My = 1+scaleY;
	}
	
	//desenha o tileset do parallax
	drawTileset(map->parallax,ox*parallaxIntensity,oy*parallaxIntensity,pmx,pMx,pmy,pMy);
	/*
	drawTileset(
		map->parallax,
		ox*parallaxIntensity+(playerSpriteX-mapWidth/2)/(mapWidth*mapWidth),
		oy*parallaxIntensity+(playerSpriteY-mapHeight/2)/(mapHeight*mapHeight),
		mx,Mx,my,My
	);
	*/
	
	//desenha o tilemap de trás
	drawTileset(map->back,ox,oy,mx,Mx,my,My);
	
	//desenha os circuitos
	for (int a = 0; a < map->wiresN; a++) {
		int2 *i = map->wires[a].nodes;
		int2 *j = map->wires[a].nodes+1;
		for (int b = 1; b < map->wires[a].n; b++) {
			al_draw_line(
				px(ox+(i->x)*scaleX),py(oy+(i->y)*scaleY),
				px(ox+(j->x)*scaleX),py(oy+(j->y)*scaleY),
				al_map_rgb(0,153,0),weightThick
			);
			if (b > 1) {
				al_draw_filled_circle(px(ox+(i->x)*scaleX),py(oy+(i->y)*scaleY),weightThick*.5f,al_map_rgb(0,153,0));
			}
			i++;
			j++;
		}
	}
	
	//desenha o guri
	if (!drawPlayer) {
	} else if (wireTempo == 1) {
		al_draw_filled_circle(px(ox+playerSpriteX*scaleX),py(oy+playerSpriteY*scaleY),weightThick*1.5f,al_map_rgb(0,255,0));
	} else if (!(respawnTempo > .5 && respawnTempo <= 1.75 && (int)ceilf(respawnTempo*8)&1)) {
		int cx,cy;
		ALLEGRO_BITMAP *bm;
		switch (playerSequence) {
			case 0: cx = 5; cy = 4; bm = data.bitmap_playerIdle; break;
			default: bm = NULL;
		}
		if (bm != NULL) {
			double sx = scaleY*2;
			double sy = sx;
			if (wireTempo > 0) {
				if (fabs(playerX-playerSpriteX) > fabs(playerY-playerSpriteY)) {
					sy *= (1-easeIn(easeIn(wireTempo))*.9);
				} else {
					sx *= (1-easeIn(easeIn(wireTempo))*.9);
				}
			}
			drawSpriteSheet(bm,playerSpriteX*scaleX,playerSpriteY*scaleY,sx,sy,cx,cy,(int)playerFrame,0,0,(functionDir < 0)?ALLEGRO_FLIP_HORIZONTAL:0);
			if (debugCollision && wireTempo == 0) {
				al_draw_rectangle(
					px(ox+(playerSpriteX-playerRadius)*scaleX),py(oy+(playerSpriteY-playerRadius)*scaleY),
					px(ox+(playerSpriteX+playerRadius)*scaleX),py(oy+(playerSpriteY+playerRadius)*scaleY),
					al_map_rgb(255,51,0),weightRegular
				);
				if (moving && !dead) {
					int x,y;
					collide(playerY-playerPrevY,&x,&y,true);
				}
			}
		}
	}
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
	
	mapX = mapStartX;
	mapY = mapStartY;
	
	currentMap = createMap();
	prevMap = createMap();
	loadMap(currentMap,mapX,mapY);
	
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
	
	TBase *base = &currentMap->bases[mapStartBase];
	if (base == NULL) return false;
	setBase(base);
	calculatePoints(true);
	
	playerSequence = 0;
	playerFrame = 0;
	
	moving = false;
	dead = false;
	wire = false;
	respawnTempo = 1;
	baseTempo = 0;
	
	wireTempo = 0;
	wireNow = NULL;
	wireIndex = 0;
	wireProgress = 0;
	
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
	LOADBITMAP(data.bitmap_tileset,tileset.png);
	return true;
}

void level_unload() {
	UNLOADBITMAP(data.bitmap_playerIdle);
	UNLOADBITMAP(data.bitmap_tileset);
	if (currentMap != NULL) {
		freeMapFull(currentMap);
		currentMap = NULL;
	}
	if (prevMap != NULL) {
		freeMapFull(prevMap);
		prevMap = NULL;
	}
}

void level_update() {
	//keypresses
	if (scene.tempo <= 0) {
		if (input.escape->press) {
			sceneLoad(MENU);
		}
		if (input.tab->press && wireTempo == 0) {
			setDir(-functionDir);
		}
		if (wireTempo > 0) {
		} else if (moving) {
			if (input.enter->press || input.space->press) {
				stopMoving();
				baseTempo = 0;
				playerX = currentBase->x;
				playerY = currentBase->y;
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
				if (!dead && !wire && wireTempo == 0) {
					if (input.up->press) {
						startCircuit(currentBase->wireUp,currentBase->wireUpDir);
					} else if (input.down->press) {
						startCircuit(currentBase->wireDown,currentBase->wireDownDir);
					} else if (input.left->press) {
						startCircuit(currentBase->wireLeft,currentBase->wireLeftDir);
					} else if (input.right->press) {
						startCircuit(currentBase->wireRight,currentBase->wireRightDir);
					}
				}
				if (!wire && wireTempo == 0) {
					if (input.enter->press) {
						if (!input.captureFinish) openTextbox();
					} else if (input.space->press && functionPlot) {
						startMoving();
					}
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
	if (functionPlot && wireTempo == 0) {
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
	
	//movimentação principal (jogador, circuitos, tela)
	if (mapTempo > 0) {
		mapTempo -= game.delta;
		if (mapTempo < 0) mapTempo = 0;
	} else if (wire) {
		if (wireTempo < 1) {
			wireTempo += game.delta*2;
			if (wireTempo > 1) wireTempo = 1;
		}
		if (wireTempo == 1) {
			wireProgress += game.delta*16;
			while (1) {
				if (wireProgress >= wireLength) {
					wireProgress -= wireLength;
					if (wireDir > 0) {
						wireIndex++;
						if (wireIndex >= wireNow->n-1) {
							wireProgress = 0;
							wire = false;
							break;
						}
					} else {
						wireIndex--;
						if (wireIndex < 0) {
							wireProgress = 0;
							wire = false;
							break;
						}
					}
					wireLength = getWireLength(wireNow,wireIndex);
					continue;
				}
				break;
			}
			if (wire) {
				if (wireDir > 0) {
					playerX = lerp(wireNow->nodes[wireIndex].x,wireNow->nodes[wireIndex+1].x,wireProgress/wireLength);
					playerY = lerp(wireNow->nodes[wireIndex].y,wireNow->nodes[wireIndex+1].y,wireProgress/wireLength);
				} else {
					playerX = lerp(wireNow->nodes[wireIndex+1].x,wireNow->nodes[wireIndex].x,wireProgress/wireLength);
					playerY = lerp(wireNow->nodes[wireIndex+1].y,wireNow->nodes[wireIndex].y,wireProgress/wireLength);
				}
			} else {
				int nextBase = (wireDir > 0)?wireNow->base1:wireNow->base0;
				int2 *w = &wireNow->nodes[(wireDir > 0)?(wireNow->n-1):0];
				if (nextBase >= 0) {
					TBase *b = &currentMap->bases[(wireDir > 0)?wireNow->base1:wireNow->base0];
					setBase(b);
					playerX = w->x;
					playerY = w->y;
				} else {
					int x = 0,y = 0;
					if (w->x == mapWidth) {
						x = 1;
						playerX = 0;
					} else if (w->x == 0) {
						x = -1;
						playerX = mapWidth;
					} else if (w->y == mapHeight) {
						y = 1;
						playerY = 0;
					} else if (w->y == 0) {
						y = -1;
						playerY = mapHeight;
					}
					moveMap(x,y);
					wireTempo = 1;
				}
			}
		}
	} else {
		if (wireTempo > 0) {
			wireTempo -= game.delta*2;
			if (wireTempo <= 0) {
				wireTempo = 0;
				calculatePoints(true);
				playerX = currentBase->x;
				playerY = currentBase->y;
			}
		}
	}
	if (wireTempo > 0) {
		if (wireTempo < 1) {
			float e = easeIn(easeIn(wireTempo));
			playerSpriteX = lerp(currentBase->x,playerX,e);
			playerSpriteY = lerp(currentBase->y,playerY,e);
		} else {
			playerSpriteX = playerX;
			playerSpriteY = playerY;
		}
	} else {
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
			playerX += game.delta*3*baseTempo*dottedAcc*acc;
			playerPrevY = playerY;
			playerY = zeroHeight-getValueOnCache(playerX-(currentBase->x))+(currentBase->y);
			playerSpriteX = lerp(playerSpriteX,playerX,baseTempo);
			playerSpriteY = lerp(playerSpriteY,playerY,baseTempo);
			int x,y;
			int collision = collide((playerY-playerPrevY),&x,&y,false);
			if (collision == 1) {
				if (fabs(playerSpriteY-y) >= 2) {
					playerSpriteY = y;
				}
				if (playerSpriteY < 0) playerSpriteY = 0;
				if (playerSpriteY > 16) playerSpriteY = 16;
				dead = true;
				respawnTempo = 2.5;
			} else if (collision == 2) {
				setBase(getBase(x,y));
				stopMoving();
				calculatePoints(true);
				baseTempo = 1;
				respawnTempo = 0;
			}
		}
		if (dead) {
			if (respawnTempo <= 1) {
				stopMoving();
				playerX = playerSpriteX = (currentBase->x);
				playerY = playerSpriteY = (currentBase->y);
			} else {
				playerSpriteY += game.delta*7*(2.2-respawnTempo);
			}
		}
		if (!moving && !dead) {
			playerSpriteX = lerp(playerX,playerSpriteX,baseTempo);
			playerSpriteY = lerp(playerY,playerSpriteY,baseTempo);
		}
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
	if (game.height <= 180) {
		weightRegular = 1;
	} else {
		weightRegular = round(game.height/180.0);
	}
	if (game.height <= 320) {
		weightThin = 1;
	} else {
		weightThin = round(game.height/320.0);
	}
	if (game.height <= 120) {
		weightThick = 1;
	} else {
		weightThick = round(game.height/120.0);
	}
	
	//desenha o mapa
	if (mapTempo > 0) {
		float e = ease(mapTempo);
		drawMap(prevMap,mapDirX*(e-1),mapDirY*(e-1),false);
		drawMap(currentMap,mapDirX*e,mapDirY*e,true);
		//ele tem q desenhar essas coisas do lado de fora do drawMap, senão o jogador pode aparecer cortado no meio
		drawTileset(prevMap->front,mapDirX*(e-1),mapDirY*(e-1),-scaleX,1+scaleX,-scaleY,1+scaleY);
		drawTileset(currentMap->front,mapDirX*e,mapDirY*e,-scaleX,1+scaleX,-scaleY,1+scaleY);
	} else {
		drawMap(currentMap,0,0,true);
		drawTileset(currentMap->front,0,0,0,1,0,1);
	}
	
	if (debugCollision) {
		BLENDALPHA();
		for (int t,x,y = 0; y < mapHeight; y++) {
			for (x = 0; x < mapWidth; x++) {
				t = getTile(currentMap->collision,x,y);
				if (t == 1) {
					al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgba(204,51,51,128));
				} else if (t == 2) {
					al_draw_filled_rectangle(px(x*scaleX),py(y*scaleY),px((x+1)*scaleX),py((y+1)*scaleY),al_map_rgba(51,204,51,128));
				}
			}
		}
		BLENDDEFAULT();
	}
	
	//posição do ponto 0 do gráfico
	double zeroHeightEase = lerp(zeroHeight,zeroHeightPrev,easeIn(zeroHeightTempo));
	double offsetX = scaleX*(currentBase->x);
	double offsetY = scaleY*((currentBase->y)+zeroHeightEase);
	double offY = fmod(zeroHeightEase,1);
	
	//desenha os eixos
	if (wireTempo < 1) {
		BLENDALPHA();
		ALLEGRO_COLOR axisColor = al_map_rgba(255,255,255,(int)lerp(25,76,textboxSizeTempo));
		if (textboxSizeTempo > 0) {
			ALLEGRO_COLOR gridColor = al_map_rgba(255,255,255,(int)(textboxSizeTempo*4));
			int r;
			for (int x = 0; x <= mapWidth; x++) {
				r = px((double)x/mapWidth);
				al_draw_line(r,py(0),r,py(1),gridColor,weightThin);
			}
			for (int y = -1; y <= mapHeight; y++) {
				r = py((double)(y+offY)/mapHeight);
				al_draw_line(px(0),r,px(1),r,gridColor,weightThin);
			}
		}
		float e = ceil(weightRegular*easeOut(1-wireTempo));
		al_draw_line(px(offsetX),py(0),px(offsetX),py(1),axisColor,e);
		al_draw_line(px(0),py(offsetY),px(1),py(offsetY),axisColor,e);
		double gridPos;
		int gridOffset;
		gridOffset = floor(-offsetX/scaleX);
		while (1) {
			gridPos = gridOffset*scaleX+offsetX;
			if (gridPos > 1) break;
			gridPos = px(gridPos);
			al_draw_line(gridPos,py(offsetY-scaleY*.125),gridPos,py(offsetY+scaleY*.125),axisColor,e);
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
			al_draw_line(px(offsetX-scaleX*.125),gridPos,px(offsetX+scaleX*.125),gridPos,axisColor,e);
			gridOffset++;
		}
		BLENDDEFAULT();
	}
	
	//plota a função
	if (weightTempo > 0 && cacheCount > 0) {
		float t = easeIn(plotTempo);
		float w = easeOut(weightTempo);
		ALLEGRO_COLOR plotColor = al_map_rgba(255,255,255,(int)lerp(153,255,textboxSizeTempo));
		w = ceil(w*weightRegular);
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
			int selOffset = -1;
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
	if (wireTempo > 0) {
	} else if (moving) {
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

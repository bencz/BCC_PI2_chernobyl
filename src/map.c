#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "map.h"

void setTile(int t[mapTotal],int x,int y,int v) {
	t[x+y*mapWidth] = v;
}

void setTileSafe(int t[mapTotal],int x,int y,int v) {
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return;
	t[x+y*mapWidth] = v;
}

TMap* createMap() {
	TMap *map = (TMap*)malloc(sizeof(TMap));
	map->collision = (int*)malloc(sizeof(int)*mapTotal);
	map->front = (int*)malloc(sizeof(int)*mapTotal);
	map->back = (int*)malloc(sizeof(int)*mapTotal);
	map->parallax = (int*)malloc(sizeof(int)*mapTotal);
	map->basesN = 0;
	map->bases = NULL;
	map->wiresN = 0;
	map->wires = NULL;
	return map;
}

TBase createBase(int x,int y) {
	TBase base;
	base.x = x;
	base.y = y;
	base.wireUp = NULL;
	base.wireDown = NULL;
	base.wireLeft = NULL;
	base.wireRight = NULL;
	base.wireUpDir = 1;
	base.wireDownDir = 1;
	base.wireLeftDir = 1;
	base.wireRightDir = 1;
	return base;
}

TWire createWire(int n) {
	TWire wire;
	wire.n = n;
	wire.nodes = (int2*)malloc(sizeof(int2)*n);
	wire.base0 = -1;
	wire.base1 = -1;
	return wire;
}

int2 newint2(int x,int y) {
	int2 i;
	i.x = x;
	i.y = y;
	return i;
}

float getWireLength(TWire *w,int i) {
	if (i >= w->n-1 || i < 0) return 0;
	if (w->nodes[i].x == w->nodes[i+1].x) {
		return abs(w->nodes[i].y-w->nodes[i+1].y);
	} else if (w->nodes[i].y == w->nodes[i+1].y) {
		return abs(w->nodes[i].x-w->nodes[i+1].x);
	} else {
		int x = w->nodes[i].x-w->nodes[i+1].x;
		int y = w->nodes[i].y-w->nodes[i+1].y;
		return sqrtf(x*x+y*y);
	}
}

void fixMap(TMap *map) {
	TBase *base;
	TWire *wire;
	int2 *node;
	for (int a = 0; a < map->basesN; a++) {
		base = &map->bases[a];
		setTile(map->collision,base->x-1,base->y-1,2);
		setTile(map->collision,base->x-1,base->y,2);
		setTile(map->collision,base->x,base->y-1,2);
		setTile(map->collision,base->x,base->y,2);
		for (int b = 0; b < map->wiresN; b++) {
			if (map->wires[b].n <= 1) continue;
			wire = &map->wires[b];
			node = wire->nodes;
			if (node->x == base->x) {
				if (node->y == base->y-1) {
					base->wireUp = wire;
					base->wireUpDir = 1;
					wire->base0 = a;
				} else if (node->y == base->y+1) {
					base->wireDown = wire;
					base->wireDownDir = 1;
					wire->base0 = a;
				}
			} else if (node->y == base->y) {
				if (node->x == base->x-1) {
					base->wireLeft = wire;
					base->wireLeftDir = 1;
					wire->base0 = a;
				} else if (node->x == base->x+1) {
					base->wireRight = wire;
					base->wireRightDir = 1;
					wire->base0 = a;
				}
			}
			node = &wire->nodes[wire->n-1];
			if (node->x == base->x) {
				if (node->y == base->y-1) {
					base->wireUp = wire;
					base->wireUpDir = -1;
					wire->base1 = a;
				} else if (node->y == base->y+1) {
					base->wireDown = wire;
					base->wireDownDir = -1;
					wire->base1 = a;
				}
			} else if (node->y == base->y) {
				if (node->x == base->x-1) {
					base->wireLeft = wire;
					base->wireLeftDir = -1;
					wire->base1 = a;
				} else if (node->x == base->x+1) {
					base->wireRight = wire;
					base->wireRightDir = -1;
					wire->base1 = a;
				}
			}
		}
	}
}

void freeMap(TMap *map) {
	for (int a = 0; a < map->wiresN; a++) {
		free(map->wires[a].nodes);
	}
	if (map->bases != NULL) free(map->bases);
}

void freeMapFull(TMap *map) {
	freeMap(map);
	free(map->collision);
	free(map->front);
	free(map->back);
	free(map->parallax);
	free(map);
}

void debugMap(const TMap *map) {
	printf(">> map\n");
	for (int a = 0; a < map->basesN; a++) {
		printf("> base #%d (%dx%d)",a,map->bases[a].x,map->bases[a].y);
		printf("\nup: ");
		if (map->bases[a].wireUp == NULL) printf("N/A");
		else printf("%d",map->bases[a].wireUpDir);
		printf("\ndown: ");
		if (map->bases[a].wireDown == NULL) printf("N/A");
		else printf("%d",map->bases[a].wireDownDir);
		printf("\nleft: ");
		if (map->bases[a].wireLeft == NULL) printf("N/A");
		else printf("%d",map->bases[a].wireLeftDir);
		printf("\nright: ");
		if (map->bases[a].wireRight == NULL) printf("N/A");
		else printf("%d",map->bases[a].wireRightDir);
		printf("\n");
	}
	for (int a = 0; a < map->wiresN; a++) {
		printf("> wire #%d",a);
		printf("\nstart (%dx%d): ",map->wires[a].nodes[0].x,map->wires[a].nodes[0].y);
		if (map->wires[a].base0 < 0) printf("N/A");
		else printf("base #%d (%dx%d)",map->wires[a].base0,map->bases[map->wires[a].base0].x,map->bases[map->wires[a].base0].y);
		printf("\nend (%dx%d): ",map->wires[a].nodes[map->wires[a].n-1].x,map->wires[a].nodes[map->wires[a].n-1].y);
		if (map->wires[a].base1 < 0) printf("N/A");
		else printf("base #%d (%dx%d)",map->wires[a].base1,map->bases[map->wires[a].base1].x,map->bases[map->wires[a].base1].y);
		printf("\n");
	}
}

void loadMap(TMap *map,const char *path) {
	//enqto n tem nada q leia os .tmx
	//aqui um monte de coisa temporária
	
	//preenche o tileset rapidão
	for (int x = 1; x < mapWidth-1; x++) {
		for (int y = 1; y < mapHeight-1; y++) {
			setTile(map->collision,x,y,0);
		}
	}
	for (int a = 0; a < 32; a++) {
		setTile(map->collision,a,0,1);
		setTile(map->collision,a,17,1);
	}
	for (int a = 1; a < 17; a++) {
		setTile(map->collision,0,a,1);
		setTile(map->collision,31,a,1);
	}
	
	//monta as bases presentes
	map->basesN = 2;
	map->bases = (TBase*)malloc(sizeof(TBase)*map->basesN);
	map->bases[0] = createBase(4,9);
	map->bases[1] = createBase(28,9);
	
	//cria os circuitos que ligam uma base à outra
	map->wiresN = 2;
	map->wires = (TWire*)malloc(sizeof(TWire)*map->wiresN);
	map->wires[0] = createWire(4);
	map->wires[0].nodes[0] = newint2(4,8);
	map->wires[0].nodes[1] = newint2(4,5);
	map->wires[0].nodes[2] = newint2(28,5);
	map->wires[0].nodes[3] = newint2(28,8);
	map->wires[1] = createWire(8);
	map->wires[1].nodes[0] = newint2(27,9);
	map->wires[1].nodes[1] = newint2(26,9);
	map->wires[1].nodes[2] = newint2(26,14);
	map->wires[1].nodes[3] = newint2(22,14);
	map->wires[1].nodes[4] = newint2(15,9);
	map->wires[1].nodes[5] = newint2(8,14);
	map->wires[1].nodes[6] = newint2(4,14);
	map->wires[1].nodes[7] = newint2(4,10);
	
	//essa função arruma o tilemap pra colocar as bases
	//e linka os circuitos com as bases
	fixMap(map);
	
	//só pra ver se ta tudo funcionando
	debugMap(map);
	
	/*
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
	*/
}
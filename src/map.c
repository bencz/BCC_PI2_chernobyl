#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "game.h"
#include "map.h"
#include "sxmlc.h"
#include "draw.h"

float dt,dox,doy;
void dpp(float x0,float y0,float x1,float y1,const char *str) {
	//recebe (x,y) de início e (x,y) de fim, em tiles
	drawPopup((x1+x0+1)/(2*mapWidth)+dox,(y1+y0+1)/(2*mapHeight)+doy,(x1-x0+1)/mapWidth,dt*(y1-y0+1)/mapHeight,dt,str);
}

void drawMapPopups(int x,int y,float t,float p,float ox,float oy) {
	dt = t;
	dox = ox;
	doy = sinf(p)*.003+oy;
	int index = mapGrid[y*mapGridWidth+x];
	//index diz o valor numérico do mapa (indicado pelo índice do path no mapList em map.h)
	if (index == 1) {
		dpp(2.5,4,7.5,6,"pressione enter\npara abrir a\ncaixa de texto.");
		dpp(9.5,4,14.5,6,"digite uma\nfunção constante!");
		dpp(16.5,4,21.5,6,"nela, para qualquer\nvalor de x, o\nf(x) é o mesmo.");
		dpp(23.5,4,28.5,6,"use as setas do\nteclado p/ caminhar\npor circuitos.");
	} else if (index == 2) {
		dpp(13.25,8.25,17.75,12.25,"use a variável \"x\"\npara fazer com\nque a f(x) se\naltere ao longo\ndo eixo x\n(horizontal).");
		dpp(9.5,14,21.5,15,"você também pode usar \"-\"\npara negar valores numéricos!");
	} else if (index == 3) {
		dpp(8.5,10.5,18.5,12.5,"o que você digitou no mapa anterior\nfoi uma função do primeiro grau,\nque segue a estrutura: f(x) = ax + b");
		dpp(7.5,14,20.5,16,"\"a\" é o coeficiente angular. ele multiplica\n(ou divide) x e altera o ângulo da reta de\nseu gráfico. tente diferentes valores para \"a\"!");
	} else if (index == 4) {
		dpp(17.5,1.5,29.5,3.5,"também é possível escrever x ao quadrado,\ncom x2 (ou x^2). isso é equivalente a\nmultiplicar x por x (x*x).");
		dpp(17.5,6.5,29.5,8.5,"obviamente, é possível juntar\n\"tipos\" de x diferentes.\ncoisas como 2x - x2 são possíveis!");
		dpp(17.5,10,29.5,11.5,"não esqueça que é possível alterar a\ndireção do jogador apertando [tab]!");
	} else if (index == 8) {
		dpp(8,1.5,26,3.5,"é possível também utilizar funções trigonométricas,\ncomo sen(x), cos(x), tg(x)... estas são funções periódicas\n(funções que se repetem depois de um intervalo constante).");
	} else if (index == 12) {
		dpp(13,2,18,2,"parabéns!");
		dpp(12,4,19,6,"você completou o jogo.\nesse é o fim dele.\nobrigado por jogar!");
	}
}

void setTile(int t[mapTotal],int x,int y,int v) {
	t[x+y*mapWidth] = v;
}

void setTileSafe(int t[mapTotal],int x,int y,int v) {
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return;
	t[x+y*mapWidth] = v;
}

TMap* createMap() {
	TMap *map = (TMap*)malloc(sizeof(TMap));
	map->index = 0;
	map->collision = (int*)calloc(mapTotal,sizeof(int));
	map->front = (int*)calloc(mapTotal,sizeof(int));
	map->back = (int*)calloc(mapTotal,sizeof(int));
	map->parallax = (int*)calloc(mapTotal,sizeof(int));
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
	wire.base0 = -2;
	wire.base1 = -2;
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
	for (int b = 0; b < map->wiresN; b++) {
		if (map->wires[b].n <= 1) continue;
		wire = &map->wires[b];
		node = wire->nodes;
		if (node->x >= mapWidth) {
			node->x = mapWidth;
			wire->base0 = -1;
		} else if (node->x <= 0) {
			node->x = 0;
			wire->base0 = -1;
		} else if (node->y >= mapHeight) {
			node->y = mapHeight;
			wire->base0 = -1;
		} else if (node->y <= 0) {
			node->y = 0;
			wire->base0 = -1;
		}
		for (int a = 0; a < map->basesN; a++) {
			base = &map->bases[a];
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
		}
		node = &wire->nodes[wire->n-1];
		if (node->x >= mapWidth) {
			node->x = mapWidth;
			wire->base1 = -1;
		} else if (node->x <= 0) {
			node->x = 0;
			wire->base1 = -1;
		} else if (node->y >= mapHeight) {
			node->y = mapHeight;
			wire->base1 = -1;
		} else if (node->y <= 0) {
			node->y = 0;
			wire->base1 = -1;
		}
		for (int a = 0; a < map->basesN; a++) {
			base = &map->bases[a];
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

int getWireIndex(const TMap *map,const TWire *wire) {
	//fiz essa função só por causa do debugMap mesmo
	for (int a = 0; a < map->wiresN; a++) {
		if (&map->wires[a] == wire) return a;
	}
	return -1;
}

void debugMap(const TMap *map) {
	for (int a = 0; a < map->basesN; a++) {
		printf("> base #%d (%dx%d)",a,map->bases[a].x,map->bases[a].y);
		if (map->bases[a].wireUp != NULL) printf("\nup: wire #%d (dir.: %d)",getWireIndex(map,map->bases[a].wireUp),map->bases[a].wireUpDir);
		if (map->bases[a].wireDown != NULL) printf("\ndown: wire #%d (dir.: %d)",getWireIndex(map,map->bases[a].wireDown),map->bases[a].wireDownDir);
		if (map->bases[a].wireLeft != NULL) printf("\nleft: wire #%d (dir.: %d)",getWireIndex(map,map->bases[a].wireLeft),map->bases[a].wireLeftDir);
		if (map->bases[a].wireRight != NULL) printf("\nright: wire #%d (dir.: %d)",getWireIndex(map,map->bases[a].wireRight),map->bases[a].wireRightDir);
		printf("\n");
	}
	for (int a = 0; a < map->wiresN; a++) {
		printf("> wire #%d",a);
		if (map->wires[a].base0 == -1) printf("\nstart (%dx%d): new map",map->wires[a].nodes[0].x,map->wires[a].nodes[0].y);
		else if (map->wires[a].base0 >= 0) printf("\nstart (%dx%d): base #%d (%dx%d)",map->wires[a].nodes[0].x,map->wires[a].nodes[0].y,map->wires[a].base0,map->bases[map->wires[a].base0].x,map->bases[map->wires[a].base0].y);
		if (map->wires[a].base1 == -1) printf("\nend (%dx%d): new map",map->wires[a].nodes[map->wires[a].n-1].x,map->wires[a].nodes[map->wires[a].n-1].y);
		else if (map->wires[a].base1 >= 0) printf("\nend (%dx%d): base #%d (%dx%d)",map->wires[a].nodes[map->wires[a].n-1].x,map->wires[a].nodes[map->wires[a].n-1].y,map->wires[a].base1,map->bases[map->wires[a].base1].x,map->bases[map->wires[a].base1].y);
		printf("\n");
	}
	printf("\n");
}

//vars pra leitura do xml
TMap *mapr; //mapa sendo lido
int tc; //índice marcando o início da id de colisão
int tt; //índice marcando o início da id do tilemap normal
int *tm; //ptr pra uma matriz de tiles
int tmm; //indica se é para varrer tiles no próximo texto
int tr; //número de cabos alocado
int tox,toy; //posição do objeto atual

int parseInt(char *s) {
	int r = 0;
	int n = 0;
	while (*s != '\0') {
		if (*s >= '0' && *s <= '9') {
			if (!n) n = 1;
			r = r*10+(*s-'0');
		} else if (!n && *s == '-') n = -1;
		s++;
	}
	return r*n;
}

int loadMapStart(const XMLNode *node,SAX_Data *sd) {
	int a;
	if (!strcmp(node->tag,"tileset")) {
		int i = 0;
		int *p = NULL;
		for (a = 0; a < node->n_attributes; a++) {
			if (!strcmp(node->attributes[a].name,"firstgid")) {
				i = parseInt(node->attributes[a].value);
			} else if (!strcmp(node->attributes[a].name,"name")) {
				if (!strcmp(node->attributes[a].value,"tileset")) {
					p = &tt;
				} else if (!strcmp(node->attributes[a].value,"tilesetCollision")) {
					p = &tc;
				}
			}
		}
		if (p != NULL && i > 0) {
			if (p == &tc) {
				tc = i-1;
			} else {
				*p = i;
			}
		}
	} else if (!strcmp(node->tag,"layer")) {
		for (a = 0; a < node->n_attributes; a++) {
			if (!strcmp(node->attributes[a].name,"name")) {
				if (!strcmp(node->attributes[a].value,"collision")) {
					tm = mapr->collision;
				} else if (!strcmp(node->attributes[a].value,"front")) {
					tm = mapr->front;
				} else if (!strcmp(node->attributes[a].value,"back")) {
					tm = mapr->back;
				} else if (!strcmp(node->attributes[a].value,"parallax")) {
					tm = mapr->parallax;
				} else {
					tm = NULL;
				}
				break;
			}
		}
	} else if (!strcmp(node->tag,"data")) {
		if (tm != NULL) for (a = 0; a < node->n_attributes; a++) {
			if (!strcmp(node->attributes[a].name,"encoding")) {
				if (strcmp(node->attributes[a].value,"csv")) {
					tm = NULL;
				} else {
					tmm = 1;
				}
				break;
			}
		}
	} else if (!strcmp(node->tag,"object")) {
		for (a = 0; a < node->n_attributes; a++) {
			if (!strcmp(node->attributes[a].name,"x")) {
				tox = parseInt(node->attributes[a].value)+16;
			} else if (!strcmp(node->attributes[a].name,"y")) {
				toy = parseInt(node->attributes[a].value)+16;
			}
		}
	} else if (!strcmp(node->tag,"polyline")) {
		if (tr == mapr->wiresN) {
			tr += 4;
			mapr->wires = (TWire*)realloc(mapr->wires,sizeof(TWire)*tr);
			printf("realloc\n");
		}
		for (a = 0; a < node->n_attributes; a++) {
			if (!strcmp(node->attributes[a].name,"points")) {
				char *s = node->attributes[a].value;
				int sp = 1;
				int o,m;
				for (o = 0; s[o] != '\0'; o++) {
					if (s[o] == ' ') sp++;
				}
				if (o == 0) {
					mapr->wiresN--;
					break;
				}
				mapr->wires[mapr->wiresN] = createWire(sp);
				m = 0;
				o = 0;
				int x = 0;
				int y = 0;
				int r = 0;
				int n = 1;
				while (1) {
					if (s[o] == ',') {
						x = (r*n+tox)/32;
						r = 0;
						n = 1;
					} else if (s[o] == '\0' || s[o] == ' ') {
						y = (r*n+toy)/32;
						mapr->wires[mapr->wiresN].nodes[m++] = newint2(x,y);
						if (s[o] == '\0' || m >= sp) break;
						r = 0;
						n = 1;
					} else if (s[o] == '-') {
						n = -1;
					} else if (s[o] >= '0' && s[o] <= '9') {
						r = r*10+s[o]-'0';
					}
					o++;
				}
				break;
			}
		}
		mapr->wiresN++;
	}
	return 1;
}

int loadMapText(SXML_CHAR *text,SAX_Data *sd) {
	if (!tmm) return 1;
	int colCheck = tm == mapr->collision;
	int r = 0;
	int a = 0;
	while (1) {
		if (*text == '\0') {
			if (colCheck) {
				if (r) {
					r -= tc;
				}
			} else {
				r -= tt;
			}
			tm[a] = r;
			break;
		}
		if (*text == ',') {
			if (colCheck) {
				if (r) {
					r -= tc;
				}
			} else {
				r -= tt;
			}
			tm[a++] = r;
			if (a >= mapTotal) break;
			r = 0;
		}
		if (*text >= '0' && *text <= '9') {
			r = r*10+*text-'0';
		}
		text++;
	}
	tmm = 0;
	return 1;
}

int loadMapEnd(const XMLNode *node,SAX_Data *sd) {
	return 1;
}

void loadMap(TMap *map,int x,int y) {
	map->index = mapGrid[y*mapGridWidth+x];
	al_set_path_filename(game.path,mapList[map->index]);
	printf(">> map %dx%d (%s)\n",x,y,al_path_cstr(game.path,'/'));
	
	XMLDoc doc;
	XMLDoc_init(&doc);
	SAX_Callbacks sax;
	SAX_Callbacks_init(&sax);
	sax.start_node = loadMapStart;
	sax.new_text = loadMapText;
	sax.end_node = loadMapEnd;
	
	mapr = map;
	tc = 0;
	tm = NULL;
	tmm = 0;
	tr = 4;
	tox = toy = 0;
	
	map->wiresN = 0;
	map->wires = (TWire*)malloc(sizeof(TWire)*tr);
	
	XMLDoc_parse_file_SAX(al_path_cstr(game.path,'/'),&sax,&sax);
	XMLDoc_free(&doc);
	
	if (tr != mapr->wiresN) {
		map->wires = (TWire*)realloc(map->wires,sizeof(TWire)*mapr->wiresN);
	}
	map->basesN = 0;
	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			if (map->collision[y*mapWidth+x] == 2) {
				if ((x != 0 && map->collision[y*mapWidth+x-1] == 2) &&
					(y != 0 && map->collision[(y-1)*mapWidth+x] == 2)) {
					map->basesN++;
				}
			}
		}
	}
	if (map->basesN) {
		map->bases = (TBase*)malloc(sizeof(TBase)*map->basesN);
		int o = 0;
		for (int x = 0; x < mapWidth; x++) {
			for (int y = 0; y < mapHeight; y++) {
				if (map->collision[y*mapWidth+x] == 2) {
					if ((x != 0 && map->collision[y*mapWidth+x-1] == 2) &&
						(y != 0 && map->collision[(y-1)*mapWidth+x] == 2)) {
						map->bases[o++] = createBase(x,y);
					}
				}
			}
		}
	}
	
	fixMap(map);
	debugMap(map);
}
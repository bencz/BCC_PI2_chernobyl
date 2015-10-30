#ifndef MAP_H
#define MAP_H

#define mapWidth 32
#define mapHeight 18
#define mapTotal mapWidth*mapHeight

typedef struct {
	int x,y;
} int2;

typedef struct {
	int n;
	int2 *nodes;
	int base0,base1;
} TWire;

typedef struct {
	int x,y;
	TWire *wireUp;
	TWire *wireDown;
	TWire *wireLeft;
	TWire *wireRight;
	int wireUpDir;
	int wireDownDir;
	int wireLeftDir;
	int wireRightDir;
} TBase;

typedef struct {
	int *collision;
	int *front;
	int *back;
	int *parallax;
	int basesN;
	TBase *bases;
	int wiresN;
	TWire *wires;
} TMap;

TMap* createMap(); //retorna um mapa alocado bonitinho
float getWireLength(TWire *w,int i); //retorna o comprimento de um pedaço de um circuito
void freeMap(TMap *map); //limpa arrays de um mapa
void freeMapFull(TMap *map); //limpa tudo de um mapa, inclusive as matrizes de tilemap
void debugMap(const TMap *map); //debuga as coisa tudo
void loadMap(TMap *map,const char *path); //teoricamente carrega um arquivo de mapa, mas ainda n tá dando

#endif
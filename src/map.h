#ifndef MAP_H
#define MAP_H

#define mapWidth 32
#define mapHeight 18
#define mapTotal mapWidth*mapHeight

//aqui a lista de mapas a serem carregados
static const char *mapList[] = {
	"data/levels/cenario01.tmx",
	"data/levels/cenario02.tmx",
	"data/levels/cenario02b.tmx",
	"data/levels/cenario03.tmx",
	"data/levels/cenario04.tmx",
};

//e aqui a ordem na qual eles são carregados
//o número indicando o índice dele no array de cima
static const int mapGridWidth = 4;
static const int mapGrid[] = {
	0,0,0,4,
	0,1,2,3,
};

//aqui indica-se o mapa a ser lido de primeira, na grade acima
static const int mapStartX = 0;
static const int mapStartY = 1;

//e aqui indica-se o índice da base na qual o jogador se encontrará
static const int mapStartBase = 0;

static const int tilemapWidth = 32; //largura do tilemap.png, em tiles
static const int tilemapHeight = 8; //altura do tilemap.png, em tiles

typedef struct {
	int x,y;
} int2;

typedef struct {
	int n; //número de nós
	int2 *nodes; //nós
	int base0,base1; //índice das base nas quais o circuito tá ligado. -2 se não está, -1 se ele sai do mapa
} TWire;

typedef struct {
	int x,y; //posição da base
	TWire *wireUp; //cabo pra cima
	TWire *wireDown; //pra baixo
	TWire *wireLeft; //pra esquerda
	TWire *wireRight; //pra direita
	int wireUpDir; //sentido do cabo pra cima
	int wireDownDir; //pra baixo
	int wireLeftDir; //pra esquerda
	int wireRightDir; //pra direita
} TBase;

typedef struct {
	int *collision; //matriz de colisão
	int *front; //matriz dos sprites na frente do personagem
	int *back; //matriz dos sprites atrás do personagem
	int *parallax; //matriz do parallax
	
	int basesN; //número de bases
	TBase *bases; //bases
	int wiresN; //número de cabos
	TWire *wires; //cabos
} TMap;

TMap* createMap(); //retorna um mapa alocado bonitinho
float getWireLength(TWire *w,int i); //retorna o comprimento de um pedaço de um circuito
void freeMap(TMap *map); //limpa arrays de um mapa
void freeMapFull(TMap *map); //limpa tudo de um mapa, inclusive as matrizes de tilemap
void debugMap(const TMap *map); //debuga as coisa tudo
void loadMap(TMap *map,int x,int y); //teoricamente carrega um arquivo de mapa, mas ainda n tá dando

#endif
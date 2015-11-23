#ifndef MAP_H
#define MAP_H

#define mapWidth 32
#define mapHeight 18
#define mapTotal mapWidth*mapHeight

//aqui a lista de mapas a serem carregados
static const char *mapList[] = {
	"data/levels/cenario01.tmx",
	"data/levels/cenario02.tmx",
	"data/levels/cenario03.tmx",
	"data/levels/cenario04.tmx",
};

//e aqui a ordem na qual eles s�o carregados
//o n�mero indicando o �ndice dele no array de cima
static const int mapGridWidth = 3;
static const int mapGrid[] = {
	0,0,3,
	0,1,2,
};

//aqui indica-se o mapa a ser lido de primeira, na grade acima
static const int mapStartX = 0;
static const int mapStartY = 1;

//e aqui indica-se o �ndice da base na qual o jogador se encontrar�
static const int mapStartBase = 0;

static const int tilemapWidth = 32; //largura do tilemap.png, em tiles
static const int tilemapHeight = 8; //altura do tilemap.png, em tiles

typedef struct {
	int x,y;
} int2;

typedef struct {
	int n; //n�mero de n�s
	int2 *nodes; //n�s
	int base0,base1; //�ndice das base nas quais o circuito t� ligado. -2 se n�o est�, -1 se ele sai do mapa
} TWire;

typedef struct {
	int x,y; //posi��o da base
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
	int *collision; //matriz de colis�o
	int *front; //matriz dos sprites na frente do personagem
	int *back; //matriz dos sprites atr�s do personagem
	int *parallax; //matriz do parallax
	
	int basesN; //n�mero de bases
	TBase *bases; //bases
	int wiresN; //n�mero de cabos
	TWire *wires; //cabos
} TMap;

TMap* createMap(); //retorna um mapa alocado bonitinho
float getWireLength(TWire *w,int i); //retorna o comprimento de um peda�o de um circuito
void freeMap(TMap *map); //limpa arrays de um mapa
void freeMapFull(TMap *map); //limpa tudo de um mapa, inclusive as matrizes de tilemap
void debugMap(const TMap *map); //debuga as coisa tudo
void loadMap(TMap *map,int x,int y); //teoricamente carrega um arquivo de mapa, mas ainda n t� dando

#endif
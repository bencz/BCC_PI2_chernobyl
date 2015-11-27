#ifndef DRAW_H
#define DRAW_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#define BLENDDEFAULT() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA)

#define BLENDALPHA() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA)

//converte a porcentagem pro espaço da tela em pixels
int px(double x);
int py(double y);

//tb, mas retorna um float
float dx(double x);
float dy(double y);

//converte de pixels pra uma porcentagem de acordo com o espaço da tela
double ix(int x);
double iy(int y);

//funções de bitmap
/*
= o valor passado em x e y indicam a posição na tela, em porcentagem (de 0 a 1)
= w e h indicam o tamanho da imagem na tela. h é em porcentagem, w vai de 0 à proporção da tela (no caso, 1.7777)
	isso pq, quando uma imagem tem q ser quadrada, é só passar o mesmo valor pra w e h.
= anchor x e y dizem o ponto pra usar como centro. 0 indica que o centro da imagem é o centro,
	-1 indica que a esquerda ou o topo são o centro, 1 indica que a direita ou pra baixo são o centro.
= flags são as mesmas flags do al_draw_bitmap.
= em drawBitmapRegion, sx/sy indicam em porcentagem o início do corte da imagem.
	sw/sh indicam em porcentagem o tamanho do corte. o resto é igual.
= drawSpriteSheet é que nem o Region, mas é só passar o número de sprites na horizontal e na vertical
	e depois o índice do sprite desejado.
= as versões tinted recebem uma cor, que tinge o bitmap.
*/
void drawBitmap(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int anchorX,int anchorY,int flags);
void drawBitmapTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double x,double y,double w,double h,int anchorX,int anchorY,int flags);
void drawBitmapRegion(ALLEGRO_BITMAP *bitmap,double sx,double sy,double sw,double sh,double nx,double ny,double nw,double nh,int anchorX,int anchorY,int flags);
void drawBitmapRegionTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double sx,double sy,double sw,double sh,double nx,double ny,double nw,double nh,int anchorX,int anchorY,int flags);
void drawSpriteSheet(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags);
void drawSpriteSheetTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags);

#endif
#ifndef DRAW_H
#define DRAW_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#define BLENDDEFAULT() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA)

#define BLENDALPHA() \
	al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA)

int px(double x); //converte a porcentagem pro espaço da tela em pixels
int py(double y);

float dx(double x); //tb, mas retorna um float
float dy(double y);

double ix(int x); //converte de pixels pra uma porcentagem de acordo com o espaço da tela
double iy(int y);

void drawBitmap(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int anchorX,int anchorY,int flags);
void drawSpriteSheet(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags);

#endif
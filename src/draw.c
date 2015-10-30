#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "game.h"
#include "draw.h"

int px(double x) {
	return round(x*game.width+game.offsetx);
}

int py(double y) {
	return round(y*game.height+game.offsety);
}

float dx(double x) {
	return (float)(x*game.width+game.offsetx);
}

float dy(double y) {
	return (float)(y*game.height+game.offsety);
}

double ix(int x) {
	return (double)(x-game.offsetx)/game.width;
}

double iy(int y) {
	return (double)(y-game.offsety)/game.height;
}

void drawBitmap(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap);
	int height = al_get_bitmap_height(bitmap);
	anchorX++;
	anchorY++;
	if (anchorX) x -= anchorX*(w/game.idealProp)/2;
	if (anchorY) y -= anchorY*h/2;
	al_draw_scaled_bitmap(bitmap,0,0,width,height,dx(x),dy(y),w*game.height,h*game.height,flags);
}

void drawSpriteSheet(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap)/sx;
	int height = al_get_bitmap_height(bitmap)/sy;
	anchorX++;
	anchorY++;
	if (anchorX) x -= anchorX*(w/game.idealProp)/2;
	if (anchorY) y -= anchorY*h/2;
	al_draw_scaled_bitmap(bitmap,width*(c%sx),height*(c/sx),width,height,dx(x),dy(y),w*game.height,h*game.height,flags);
}
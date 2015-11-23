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

void drawBitmapRegion(ALLEGRO_BITMAP *bitmap,double sx,double sy,double sw,double sh,double nx,double ny,double nw,double nh,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap);
	int height = al_get_bitmap_height(bitmap);
	anchorX++;
	anchorY++;
	if (anchorX) nx -= anchorX*(nw/game.idealProp)/2;
	if (anchorY) ny -= anchorY*nh/2;
	al_draw_scaled_bitmap(bitmap,sx*width,sy*height,sw*width,sh*height,dx(nx),dy(ny),nw*game.height,nh*game.height,flags);
}

void drawSpriteSheet(ALLEGRO_BITMAP *bitmap,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap)/sx;
	int height = al_get_bitmap_height(bitmap)/sy;
	anchorX++;
	anchorY++;
	if (anchorX) x -= anchorX*(w/game.idealProp)/2;
	if (anchorY) y -= anchorY*h/2;
	al_draw_scaled_bitmap(bitmap,width*(c%sx)+1,height*(c/sx)+1,width-2,height-2,dx(x),dy(y),w*game.height,h*game.height,flags);
}
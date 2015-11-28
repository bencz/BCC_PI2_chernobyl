#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
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

void drawBox(double x,double y,double w,double h,ALLEGRO_COLOR border,ALLEGRO_COLOR border2) {
	float b = (game.height <= 340)?1:(round(game.height/340.0));
	w *= .5;
	h *= .5;
	float x0 = dx(x-w);
	float x1 = dx(x+w);
	float y0 = dy(y-h);
	float y1 = dy(y+h);
	BLENDALPHA();
	al_draw_filled_rectangle(x0-b*3,y0-b*3,x1+b*3,y1+b*3,al_map_rgba(0,0,0,51));
	BLENDDEFAULT();
	al_draw_filled_rectangle(x0-b,y0-b,x1+b,y1+b,border);
	al_draw_filled_rectangle(x0,y0,x1,y1,border2);
	al_draw_filled_rectangle(x0+b,y0+b,x1-b,y1-b,al_map_rgb(255,255,255));
}

void drawCorner(int x,int y,double l,float h,ALLEGRO_COLOR border,ALLEGRO_COLOR border2) {
	int xa,xb,ya,yb;
	if (x >= 1) {
		xa = -1;
		xb = 1;
	} else {
		xa = 1;
		xb = 0;
	}
	if (y >= 1) {
		ya = -1;
		yb = 1;
	} else {
		ya = 1;
		yb = 0;
	}
	float b = (game.height <= 333)?1:(round(game.height/333.0));
	float bs2 = b*1.414214*xa;
	float bs21 = b*.414214*xa;
	b *= ya;
	float y0 = dy(-ya*h+yb);
	float y1 = dy(ya*(1.0/18.0-h)+yb);
	float x0 = dx(xb);
	float x1 = dx(xa*l+xb);
	float x2 = dx(xa*(l+.03125)+xb);
	float y1a = y1-b;
	float y1b = y1+b;
	float y1c = y1+b*3;
	float x1a = x1-bs21;
	float x1b = x1+bs21;
	float x1c = x1+bs21*3;
	BLENDALPHA();
	al_draw_filled_rectangle(x0,y0,x1c,y1c,al_map_rgba(0,0,0,51));
	al_draw_filled_triangle(x1c,y0,x1c,y1c,x2+bs2*3,y0,al_map_rgba(0,0,0,51));
	BLENDDEFAULT();
	al_draw_filled_rectangle(x0,y0,x1b,y1b,border);
	al_draw_filled_triangle(x1b,y0,x1b,y1b,x2+bs2,y0,border);
	al_draw_filled_rectangle(x0,y0,x1,y1,border2);
	al_draw_filled_triangle(x1,y0,x1,y1,x2,y0,border2);
	al_draw_filled_rectangle(x0,y0,x1a,y1a,al_map_rgb(255,255,255));
	al_draw_filled_triangle(x1a,y0,x1a,y1a,x2-bs2,y0,al_map_rgb(255,255,255));
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

void drawBitmapTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double x,double y,double w,double h,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap);
	int height = al_get_bitmap_height(bitmap);
	anchorX++;
	anchorY++;
	if (anchorX) x -= anchorX*(w/game.idealProp)/2;
	if (anchorY) y -= anchorY*h/2;
	al_draw_tinted_scaled_bitmap(bitmap,tint,0,0,width,height,dx(x),dy(y),w*game.height,h*game.height,flags);
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

void drawBitmapRegionTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double sx,double sy,double sw,double sh,double nx,double ny,double nw,double nh,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap);
	int height = al_get_bitmap_height(bitmap);
	anchorX++;
	anchorY++;
	if (anchorX) nx -= anchorX*(nw/game.idealProp)/2;
	if (anchorY) ny -= anchorY*nh/2;
	al_draw_tinted_scaled_bitmap(bitmap,tint,sx*width,sy*height,sw*width,sh*height,dx(nx),dy(ny),nw*game.height,nh*game.height,flags);
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

void drawSpriteSheetTinted(ALLEGRO_BITMAP *bitmap,ALLEGRO_COLOR tint,double x,double y,double w,double h,int sx,int sy,int c,int anchorX,int anchorY,int flags) {
	int width = al_get_bitmap_width(bitmap)/sx;
	int height = al_get_bitmap_height(bitmap)/sy;
	anchorX++;
	anchorY++;
	if (anchorX) x -= anchorX*(w/game.idealProp)/2;
	if (anchorY) y -= anchorY*h/2;
	al_draw_tinted_scaled_bitmap(bitmap,tint,width*(c%sx),height*(c/sx),width,height,dx(x),dy(y),w*game.height,h*game.height,flags);
}
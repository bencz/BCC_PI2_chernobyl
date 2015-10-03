#include "utils.h"

float lerp(float a,float b,float t) {
	if (t <= 0) return a;
	if (t >= 1) return b;
	return a+(b-a)*t;
}

double dlerp(double a,double b,double t) {
	if (t <= 0) return a;
	if (t >= 1) return b;
	return a+(b-a)*t;
}

float invLerp(float a,float b,float t) {
	if (b > a) {
		if (t <= a) return 0;
		if (t >= b) return 1;
	} else {
		if (t <= b) return 0;
		if (t >= a) return 1;
	}
	return (t-a)/(b-a);
}

float ease(float t) {
	if (t <= 0) return 0;
	if (t >= 1) return 1;
	return (3-2*t)*t*t;
}

float easeIn(float t) {
	if (t <= 0) return 0;
	if (t >= 1) return 1;
	return t*t;
}

float easeOut(float t) {
	if (t <= 0) return 0;
	if (t >= 1) return 1;
	return (2-t)*t;
}

float clamp(float t,float a,float b) {
	if (t <= a) return a;
	if (t >= b) return b;
	return t;
}

float clamp01(float t) {
	if (t <= 0) return 0;
	if (t >= 1) return 1;
	return t;
}
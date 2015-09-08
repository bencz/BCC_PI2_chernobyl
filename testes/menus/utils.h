#ifndef UTILS_H
#define UTILS_H

float lerp(float a,float b,float t);

float invLerp(float a,float b,float t);

float ease(float t);

float easeIn(float t);

float easeOut(float t);

float clamp(float t,float a,float b);

float clamp01(float t);

#endif
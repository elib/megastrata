//util functions and structs.

#pragma once

//don't define this! ffmpeg needed, not included!
//#define INCLUDE_VIDEO

#include <math.h>

#define EPSILON (1e-5)

//do not link to libNoise
#define DISABLE_LIBNOISE

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

//maximum entities per EntityCollection
//actually sqrt(max entities), because this is one side of the square.
#define MAX_ENTITIES 30

//what is one grid-mark
#define GRID_SIZE 1.0f

// [WODE]
#ifdef __APPLE__
	#include <Glut/glut.h>
    #define glutSolidCylinder(base, height, slices, stacks) gluCylinder(gluNewQuadric(), base, base, height, slices, stacks) 

#else
	#include <freeglut.h>
#endif 

#include <string>
using namespace std;

#include <assert.h>
#include <stdio.h>

#include "AutoMatrixStack.h"

//calculations courtesy of www.WolframAlpha.com :D
#define TWICE_PI	(6.2831853f)
#define PI			(3.1415926f)
#define HALF_PI		(1.5707963f)
#define FOURTH_PI	(0.7853981f)

#define SQRT_2		(1.4142135f)


class Viewer2d;
class LayerVariable;
class GeneratorBase;
class Generator;
class SubGenerator;
class Entity3dCollection;
class Vector3d;


void TessellatedQuad(Vector3d *corners, int tessellation_u, int tessellation_v);


//misc. math functions
#define DEG(angle) (180*((angle)/PI))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define ABS(a) (((a)<0)?(-(a)):(a))

inline int round(float x)
{
   return int(x > 0.0f ? x + 0.5f : x - 0.5f);
}

struct RECTFLOAT
{
	float left, top, right, bottom;
};


#ifdef __APPLE__
typedef int64_t ElapsedTime;
#else
typedef __int64 ElapsedTime;
#endif


bool FileExists( const char* FileName );

//debug helper, just drop this in anywhere.
#ifdef _DEBUG
	#ifdef __APPLE__
	// [WODE] can't seem to make this work on the mac atm
		#define LOG(string, ...)	{ printf(string "\r\n", ##__VA_ARGS__); }
	#else
		#define LOG(string, ...)	{ char ____text[500]; sprintf_s(____text, string "\r\n", __VA_ARGS__); OutputDebugStringA(____text); }
	#endif
#else
	//release mode - compile, don't do anything.
	#define LOG(string, ...) //
#endif

typedef struct _POINT_FLOAT {
    float   x;
    float   y;
} POINT_FLOAT, *PPOINT_FLOAT;

// [WODE]
#ifdef __APPLE__
	#define sprintf_s sprintf
	#define strcpy_s strcpy

	#define Sleep(ms) sleep( (ms) / 1000 )
#endif
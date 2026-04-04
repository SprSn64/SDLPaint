#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL3/SDL.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

typedef struct{
	bool down, pressed, released, pressCheck;
	Uint32 code;
} ButtonMap;

typedef enum blendMode{
	BLENDMODE_NORMAL,
	BLENDMODE_MULT,
	BLENDMODE_ADD,
	BLENDMODE_SUB,
} blendMode;

typedef struct Image Image;

typedef struct Layer{
	Uint32 *pixels;
	Uint32 blend;
	float alpha;

	struct Image* image;
	struct Layer* prev;
	struct Layer* next;
} Layer;

typedef struct Image{
	char* path;
	SDL_Texture* texture;
	Uint32 *pixels;
	Uint32 width, height;

	Layer* headLayer;
} Image;

#endif

#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL3/SDL.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

typedef struct{
	bool down, pressed, released, pressCheck;
	Uint32 code;
} ButtonMap;

typedef struct Layer{
	Uint32 *pixels;
	Uint32 blend;
	float alpha;

	struct Layer* prevLayer;
	struct Layer* nextLayer;
} Layer;

typedef struct{
	char* path;
	SDL_Texture* texture;
	Uint32 *pixels;
	Uint32 width, height;

	Layer* headLayer;
} Image;

#endif

#ifndef RENDER_H
#define RENDER_H

#include <SDL3/SDL.h>
#include <structs.h>

typedef enum toolModes{
	TOOL_BRUSH,
	TOOL_ERASE,
	TOOL_COLOURPICK
} toolModes;


Uint32 colourToInt(SDL_FColor colour);
void setPixel(Image* image, Uint32 posX, Uint32 posY, SDL_FColor colour, bool override);
int drawHamLine(Image* image, SDL_Point pointA, SDL_Point pointB, SDL_FColor colour, bool override);

#endif

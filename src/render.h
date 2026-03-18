#ifndef RENDER_H
#define RENDER_H

#include <SDL3/SDL.h>
#include <structs.h>

Uint32 colourToInt(SDL_FColor colour);
void setPixel(Image* image, Uint32 posX, Uint32 posY, SDL_FColor colour);
int drawHamLine(Image* image, SDL_Point pointA, SDL_Point pointB, SDL_FColor colour);

#endif

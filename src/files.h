#ifndef FILES_H
#define FILES_H

#include <SDL3/SDL.h>
#include <structs.h>

Image* newImageItem(Uint16 width, Uint16 height, Uint32 colour);
Image* loadImageFile(char* path);

SDL_Texture *newTexture(char* path, SDL_ScaleMode scaleMode);

#endif


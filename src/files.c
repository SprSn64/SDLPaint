#include "files.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <structs.h>

extern SDL_Renderer *renderer;

Layer* newLayer(Image* parent, Uint32 colour){
	Layer* newLayer = malloc(sizeof(Layer));
	if(!newLayer) return NULL;

	newLayer->pixels = malloc(sizeof(Uint32) * parent->width * parent->height);
	for(Uint32 i=0; i< parent->width * parent->height; i++){
		newLayer->pixels[i] = colour;
	}

	newLayer->image = parent;
	//if(!parent->headLayer)
		parent->headLayer = newLayer;

	return newLayer;
}

Image* newImageItem(Uint16 width, Uint16 height, Uint32 colour){
	Image* newImg = malloc(sizeof(Image));
	if(!newImg) return NULL;

	newImg->width = width; newImg->height = height;
	newLayer(newImg, colour);

	newImg->pixels = malloc(sizeof(Uint32) * width * height);
	for(Uint32 i=0; i<width * height; i++){
		newImg->pixels[i] = colour;
	}
	newImg->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	SDL_UpdateTexture(newImg->texture, NULL, newImg->pixels, width * sizeof(Uint32));

	return newImg;
}

Image* loadImageFile(char* path){
	SDL_Surface* newSurface = IMG_Load(path); if(!newSurface) return NULL;
	Image* newImg = newImageItem(newSurface->w, newSurface->h, 0xFF000000); if(!newImg) return NULL;

	newImg->path = strdup(path);

	printf("Image size is %d by %d\n", newSurface->w, newSurface->h);
	memcpy(newImg->pixels, newSurface->pixels, newSurface->w*newSurface->h * sizeof(Uint32)); //convert surface format to ABGR8888
	SDL_UpdateTexture(newImg->texture, NULL, newImg->pixels, newImg->width * sizeof(Uint32));

	SDL_DestroySurface(newSurface);
	return newImg;
}

SDL_Texture *newTexture(char* path, SDL_ScaleMode scaleMode){
	SDL_Texture *texture = IMG_LoadTexture(renderer, path);
	if(texture == NULL){
		printf("Issue with loading texture %s!\n", path);
		return NULL;
	}
	SDL_SetTextureScaleMode(texture, scaleMode);
	return texture;
}

bool refreshImage(Image* item){
	printf("meatball ");
	if(!item->headLayer) return 1;

	/*for(Uint32 i=0; i<item->width * item->height; i++){
		item->pixels[i] = 0x00000000;
	}*/

	Layer* currLayer = item->headLayer;
	while(currLayer){
		for(Uint32 i=0; i<item->width * item->height; i++){
			item->pixels[i] = currLayer->pixels[i];
		}
		currLayer = currLayer->next;
	}

	SDL_UpdateTexture(item->texture, NULL, item->pixels, item->width * sizeof(Uint32));

	return 0;
}
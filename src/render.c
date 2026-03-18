#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>

bool between(float input, float min, float max);

float lerp(float a, float b, float t){
	return a + t * (b - a);
}

Uint32 colourToInt(SDL_FColor colour){
	return (int)(colour.r * 255) + ((int)(colour.g * 255) << 8) + ((int)(colour.b * 255) << 16) + ((int)(colour.a * 255) << 24); 
}

SDL_FColor intToColour(Uint32 colour){
	return (SDL_FColor){
		(float)(colour & 0x000000FF) / 255, 
		(float)((colour & 0x0000FF00) >> 8) / 255,
		(float)((colour & 0x00FF0000) >> 16) / 255,
		(float)((colour & 0xFF000000) >> 24) / 255
	};
}

SDL_FColor colourLerp(SDL_FColor colA, SDL_FColor colB, float t){
	return (SDL_FColor){lerp(colA.r, colB.r, t), lerp(colA.g, colB.g, t), lerp(colA.b, colB.b, t), lerp(colA.a, colB.a, t)};
}

SDL_FColor colourMult(SDL_FColor colA, SDL_FColor colB){
	return (SDL_FColor){colA.r * colB.r, colA.g * colB.g, colA.b * colB.b, colA.a * colB.a};
}

void setPixel(Image* image, Uint32 posX, Uint32 posY, SDL_FColor colour){
	if(!image || !between(posX, 0, image->width - 1) || !between(posY, 0, image->height - 1) || colour.a == 0) return;
	SDL_FColor newColour = colour;
	if(colour.a != 1)
		newColour = colourLerp(intToColour(image->pixels[posX + posY * image->width]), colour, colour.a);
	image->pixels[posX + posY * image->width] = colourToInt(newColour);
}

int drawHamLine(Image* image, SDL_Point pointA, SDL_Point pointB, SDL_FColor colour){
	if(abs(pointB.x - pointA.x) > abs(pointB.y - pointA.y)){
		SDL_Point delta = {abs(pointB.x - pointA.x), pointB.y - pointA.y}; 
		Sint8 dirX = 1 - 2 * (pointA.x > pointB.x);
		Sint8 dirY = 1 - 2 * (delta.y < 0);
		delta.y = delta.y * dirY;
		if(delta.x == 0) return 1;
		int decide = 2 * delta.y - delta.x;
		int newY = pointA.y;
		for(int i=0; i <= delta.x; i++){
			setPixel(image, pointA.x + i * dirX, newY, colour);
			if(decide >= 0){
				newY+=dirY;
				decide += -2*delta.x;
			}
			decide += 2*delta.y;
		}
	}else{
		SDL_Point delta = {pointB.x - pointA.x, abs(pointB.y - pointA.y)}; 
		Sint8 dirY = 1 - 2 * (pointA.y > pointB.y);
		Sint8 dirX = 1 - 2 * (delta.x < 0);
		delta.x = delta.x * dirX;
		if(delta.y == 0) return 1;
		int decide = 2 * delta.x - delta.y;
		int newX = pointA.x;
		for(int i=0; i <= delta.y; i++){
			setPixel(image, newX, pointA.y + i * dirY, colour);
			if(decide >= 0){
				newX+=dirX;
				decide += -2*delta.y;
			}
			decide += 2*delta.x;
		}
	}
	return 0;
}

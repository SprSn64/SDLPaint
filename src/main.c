#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>

#include <structs.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_MouseButtonFlags mouseState;
SDL_FPoint mousePos;
ButtonMap mouseButtons[3];

SDL_FPoint lastMousePos;

Image testImage = {NULL, NULL, NULL, 256, 256};

bool between(float input, float min, float max){return(input >= min && input <= max);}

void setPixel(Image* image, Uint32 posX, Uint32 posY, SDL_Color colour);
int drawHamLine(Image* image, SDL_Point pointA, SDL_Point pointB, SDL_Color colour);

SDL_Color drawColour = {0, 0, 0, 1};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	(void)appstate; (void)argc; (void)argv;

	SDL_SetAppMetadata("SDLPaint", "0.0.0", NULL);

	//for(int i=0; i < argc; i++){
	//	(void)argv[i];
	//}

	if(!SDL_CreateWindowAndRenderer("SDLPaint", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)){
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_SetRenderVSync(renderer, 1);

	mouseButtons[0].code = SDL_BUTTON_LMASK; mouseButtons[1].code = SDL_BUTTON_MMASK; mouseButtons[2].code = SDL_BUTTON_RMASK;

	testImage.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, testImage.width, testImage.height);
	SDL_SetTextureScaleMode(testImage.texture, SDL_SCALEMODE_NEAREST);

	testImage.pixels = malloc(sizeof(Uint32) * testImage.width * testImage.height);
	for(Uint32 i=0; i<testImage.width * testImage.height; i++){
		testImage.pixels[i] = 0xFFFFFFFF;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	(void)appstate;
	if(event->type == SDL_EVENT_QUIT){
		return SDL_APP_SUCCESS;
	}
	
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate){
	(void)appstate;
	//SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);

	mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	for(int i=0; i<3; i++){
		mouseButtons[i].down = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) && (mouseState & mouseButtons[i].code);
		if(mouseButtons[i].down){
			if(!mouseButtons[i].pressCheck){
				mouseButtons[i].pressCheck = true;
				mouseButtons[i].pressed = true;
			}else{
				mouseButtons[i].pressed = false;
			}
		}else mouseButtons[i].pressCheck = false;
	}

	SDL_SetRenderDrawColor(renderer, 96, 96, 96, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	if(mouseButtons[0].down && between(mousePos.x, 0, testImage.width) && between(mousePos.y, 0, testImage.height)){
		//testImage.pixels[(int)mousePos.x + (int)mousePos.y * testImage.width] = 0xFF000000;
		if(mouseButtons[0].pressed)
			setPixel(&testImage, (int)mousePos.x, (int)mousePos.y, drawColour);
		else
			drawHamLine(&testImage, (SDL_Point){lastMousePos.x, lastMousePos.y}, (SDL_Point){mousePos.x, mousePos.y}, drawColour);
		lastMousePos = mousePos;
	}

	SDL_UpdateTexture(testImage.texture, NULL, testImage.pixels, testImage.width * sizeof(Uint32));
	SDL_RenderTexture(renderer, testImage.texture, &(SDL_FRect){0, 0, testImage.width, testImage.height}, &(SDL_FRect){0, 0, testImage.width, testImage.height});
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
	(void)appstate; (void)result;
}

Uint32 colourTo32(SDL_Color colour){
	return (int)(colour.r * 255) + ((int)(colour.g * 255) << 8) + ((int)(colour.b * 255) << 16) + ((int)(colour.a * 255) << 24); 
}

void setPixel(Image* image, Uint32 posX, Uint32 posY, SDL_Color colour){
	image->pixels[posX + posY * image->width] = colourTo32(colour);
}

int drawHamLine(Image* image, SDL_Point pointA, SDL_Point pointB, SDL_Color colour){
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
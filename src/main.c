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

Image testImage = {NULL, NULL, NULL, 256, 256};

bool between(float input, float min, float max){return(input >= min && input <= max);}

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

	testImage.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, testImage.width, testImage.height);
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
		mouseButtons[i].down = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS && (mouseState & mouseButtons[i].code));
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

	SDL_UpdateTexture(testImage.texture, NULL, testImage.pixels, testImage.width * sizeof(Uint32));
	SDL_RenderTexture(renderer, testImage.texture, &(SDL_FRect){0, 0, testImage.width, testImage.height}, &(SDL_FRect){0, 0, testImage.width, testImage.height});
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
	(void)appstate; (void)result;
}
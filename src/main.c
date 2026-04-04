#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "render.h"
#include "files.h"
#include "panels.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Point windowSize = {640, 480};

SDL_MouseButtonFlags mouseState;
SDL_FPoint mousePos;
ButtonMap mouseButtons[3];

SDL_Point lastMousePos;

float zoom = 1;
SDL_FPoint cameraPos = {0, 0};

//Image testImage = {NULL, NULL, NULL, 640, 480};
Image* currImage = NULL;
Layer* currLayer = NULL;
bool updateImage = true;

SDL_Texture* checkerTex = false;

bool between(float input, float min, float max){return(input >= min && input <= max);}

SDL_FColor priColour = {0, 0, 0, 1};
SDL_FColor secColour = {1, 1, 1, 1};
SDL_FColor drawColour = {0, 0, 0, 1};

Uint32 toolMode = TOOL_BRUSH;

#define KEYBIND_MAX 24
ButtonMap keyList[KEYBIND_MAX];
void HandleKeyInput();

const char* basePath;

extern Panel toolPanel;
extern Panel colourPanel;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	(void)appstate; (void)argc; (void)argv;

	basePath = SDL_GetBasePath();
	SDL_SetAppMetadata("SDLPaint", "0.0.0", NULL);

	for(int i=0; i < argc; i++){
		printf("%s\n", argv[i]);
	}

	if(!SDL_CreateWindowAndRenderer("SDLPaint", windowSize.x, windowSize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &window, &renderer)){
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if(argc >= 1 && argv[1]){
		currImage = loadImageFile(argv[1]);
	}

	//SDL_SetRenderVSync(renderer, 1);

	char checkerPath[256]; sprintf(checkerPath, "%s%s", basePath, "assets/checkers.png");
	checkerTex = newTexture(checkerPath, SDL_SCALEMODE_NEAREST);

	mouseButtons[0].code = SDL_BUTTON_LMASK; mouseButtons[1].code = SDL_BUTTON_MMASK; mouseButtons[2].code = SDL_BUTTON_RMASK;
	keyList[0].code = SDL_SCANCODE_LCTRL;

	if(!currImage)
		currImage = newImageItem(640, 480, 0xFFFFFFFF);
	currLayer = currImage->headLayer;

	initToolPanel();
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	(void)appstate;
	if(event->type == SDL_EVENT_QUIT){
		return SDL_APP_SUCCESS;
	}

	if(event->type == SDL_EVENT_MOUSE_WHEEL){
		if((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)){
			float zoomSpeed = 0.1;
			float zoomMin = 0.1;
			float zoomMax = 10;
			
			float zoomChange = zoomSpeed * (1 - 2 * (event->wheel.y < 0)) * (event->wheel.y != 0);
			zoom = min(max(zoom + zoomChange, zoomMin), zoomMax);

			cameraPos = (SDL_FPoint){
				cameraPos.x + (1 - 2 * (zoomChange < 0)) * (cameraPos.x - (mousePos.x - windowSize.x/2)) / 10, 
				cameraPos.y + (1 - 2 * (zoomChange < 0)) * (cameraPos.y - (mousePos.y - windowSize.y/2)) / 10
			};

			if(zoom < 1)
				SDL_SetTextureScaleMode(currImage->texture, SDL_SCALEMODE_LINEAR);
			else
				SDL_SetTextureScaleMode(currImage->texture, SDL_SCALEMODE_NEAREST);
		}
	}
	
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate){
	(void)appstate;
	SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);

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

	HandleKeyInput();

	SDL_SetRenderDrawColor(renderer, 96, 96, 96, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	updatePanel(&toolPanel); updatePanel(&colourPanel);

	//mousePos.x = zoom * (cameraPos.x - testImage.width/2 + x) + windowSize.x/2
	SDL_Point canvasLoc = {zoom * (cameraPos.x - currImage->width/2) + windowSize.x/2, zoom * (cameraPos.y - currImage->height/2) + windowSize.y/2};

	SDL_Point adjMousePos = {
		(mousePos.x - canvasLoc.x) / zoom,
		(mousePos.y - canvasLoc.y) / zoom
	}; 

	drawColour = priColour;
	if(mouseButtons[2].down) drawColour = secColour;

	switch(toolMode){
		case TOOL_BRUSH:
			if(!mouseButtons[0].down && !mouseButtons[2].down) break;

			if(mouseButtons[0].pressed || mouseButtons[2].pressed)
				setPixel(currImage, adjMousePos.x, adjMousePos.y, drawColour, false);
			else
				drawHamLine(currImage, lastMousePos, adjMousePos, drawColour, false);
			lastMousePos = adjMousePos; updateImage = true; 
			break;
		case TOOL_ERASE:
			if(!mouseButtons[0].down && !mouseButtons[2].down) break;

			if(mouseButtons[0].pressed || mouseButtons[2].pressed)
				setPixel(currImage, adjMousePos.x, adjMousePos.y, (SDL_FColor){0, 0, 0, 0}, true);
			else
				drawHamLine(currImage, lastMousePos, adjMousePos, (SDL_FColor){0, 0, 0, 0}, true);
			lastMousePos = adjMousePos; updateImage = true;
			break;
		case TOOL_COLOURPICK:
			if(!between(adjMousePos.x, 0, currImage->width) || !between(adjMousePos.y, 0, currImage->height)) break;
			SDL_FColor hoverColour = intToColour(currImage->pixels[(adjMousePos.x % currImage->width) + (adjMousePos.y % currImage->height) * currImage->width]);
			if(mouseButtons[0].down) priColour = hoverColour;
			if(mouseButtons[2].down) secColour = hoverColour;
			break;
	}

	if(updateImage)
		SDL_UpdateTexture(currImage->texture, NULL, currImage->pixels, currImage->width * sizeof(Uint32));
	updateImage = false;

	SDL_FRect imageDest = {canvasLoc.x, canvasLoc.y, currImage->width * zoom, currImage->height * zoom};

	SDL_RenderTextureTiled(renderer, checkerTex, NULL, 1, &imageDest);
	SDL_RenderTexture(
		renderer, 
		currImage->texture, 
		&(SDL_FRect){0, 0, currImage->width, currImage->height}, 
		&imageDest
	);

	drawPanel(&toolPanel); drawPanel(&colourPanel);

	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
	(void)appstate; (void)result;
}

void HandleKeyInput(){
	const bool* keyState = SDL_GetKeyboardState(NULL);
	bool hasFocus = SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS;
	for(int i = 0; i < KEYBIND_MAX; i++){
		keyList[i].down = hasFocus && keyState[keyList[i].code];
		keyList[i].pressed = false;
		if(keyList[i].down){
			if(!keyList[i].pressCheck){
				keyList[i].pressCheck = true;
				keyList[i].pressed = true;
			}
		}else keyList[i].pressCheck = false;
	}
}
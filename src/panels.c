#include "panels.h"
#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <structs.h>
#include "render.h"

bool between(float input, float min, float max);

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern SDL_FPoint mousePos;
extern ButtonMap mouseButtons[3];

Panel testPanel = {(SDL_FRect){4, 4, 38, 256}, NULL, 3, PANEL_VISIBLE};

Uint32 toolList[3] = {TOOL_BRUSH, TOOL_ERASE, TOOL_COLOURPICK};

extern Uint32 toolMode;
void buttonSetTool(Button* item){
	for(int i=0; i<testPanel.buttonCount; i++){
		if(&testPanel.buttonList[i] != item) continue;
		toolMode = toolList[i];
		return;
	}
}

void initTestPanel(){
	Button* newButtons = malloc(3 * sizeof(Button));

	newButtons[0] = (Button){"Br", (SDL_FRect){2, 2, 16, 16}, INPUTTYPE_BUTTON, buttonSetTool, true, false, false, NULL};
	newButtons[1] = (Button){"Er", (SDL_FRect){20, 2, 16, 16}, INPUTTYPE_BUTTON, buttonSetTool, true, false, false, NULL};
	newButtons[2] = (Button){"Co", (SDL_FRect){2, 20, 16, 16}, INPUTTYPE_BUTTON, buttonSetTool, true, false, false, NULL};

	testPanel.buttonList = newButtons;
}

bool updateButton(Button* item, SDL_Point* offset){
	if(!item->enabled || !(item->pressed || item->target)) return 1;

	SDL_Point newOffset = {0, 0};
	if(offset) newOffset = *offset;
	
	item->hover = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) && between(mousePos.x - item->rect.x - newOffset.x, 0, item->rect.w) && between(mousePos.y - item->rect.y - newOffset.y, 0, item->rect.h);
	if(item->hover){
		SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
		if(!mouseButtons[0].down){item->down = false; return 1;}
		if(item->down) return 1;
		item->down = true;

		switch(item->buttonType){
			default: item->pressed(item); break;
		}
	}
	
	return 0;
}

void drawButton(Button* item, SDL_Point* offset){
	SDL_Point newOffset = {0, 0};
	if(offset) newOffset = *offset;

	if(item->buttonType == INPUTTYPE_IMGBUTTON && item->label != NULL){
		TextureItem* texItem = item->label;
		SDL_RenderTexture(renderer, texItem->texture, &texItem->source, &(SDL_FRect){item->rect.x + newOffset.x, item->rect.y + newOffset.y, item->rect.w, item->rect.h});
		return;
	}

	if(!item->enabled){
		SDL_SetRenderDrawColor(renderer, 177, 179, 191, 255);
		goto buttonDrawStart;
	}

	SDL_SetRenderDrawColor(renderer, 205, 208, 226, 255);
	if(item->hover)
		SDL_SetRenderDrawColor(renderer, 231, 234, 249, 255);
	if(item->down)
		SDL_SetRenderDrawColor(renderer, 124, 128, 154, 255);

	buttonDrawStart:
	SDL_RenderFillRect(renderer, &(SDL_FRect){item->rect.x + newOffset.x, item->rect.y + newOffset.y, item->rect.w, item->rect.h});

	if(item->buttonType == INPUTTYPE_BUTTON){
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDebugText(renderer, item->rect.x + 2 + newOffset.x, item->rect.y + 2 + newOffset.y, item->label);
	}
}

void updatePanel(Panel* panel){
	for(int i=0; i<panel->buttonCount; i++){
		updateButton(&panel->buttonList[i], &(SDL_Point){panel->frame.x, panel->frame.y});
	}
}

void drawPanel(Panel* panel){
	if(!(panel->flags & PANEL_VISIBLE)) return;

	SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
	SDL_RenderFillRect(renderer, &panel->frame);

	for(int i=0; i<panel->buttonCount; i++){
		drawButton(&panel->buttonList[i], &(SDL_Point){panel->frame.x, panel->frame.y});
	}
}
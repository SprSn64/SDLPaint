#ifndef PANELS_H
#define PANELS_H

#include <SDL3/SDL.h>
#include <structs.h>

typedef enum buttonInputType{
	INPUTTYPE_BUTTON,
	INPUTTYPE_IMGBUTTON,
	INPUTTYPE_DROPDOWN,
	INPUTTYPE_TEXT,
	INPUTTYPE_NUMBER, //click and drag left/right to change value
	INPUTTYPE_COLOUR
} buttonInputType;

typedef struct TextureItem{
	SDL_Texture* texture;
	SDL_FRect source;
} TextureItem;

typedef struct Button{
	void *label;
	SDL_FRect rect;
	Uint8 buttonType;
	void (*pressed)(struct Button*);
	bool enabled, hover, down;
	void *target;
} Button;

typedef enum panelFlags{
	PANEL_VISIBLE = 	0x00000001,
	PANEL_ANCHOR = 	0x00000006
} panelFlags;

typedef struct Panel{
	SDL_FRect frame;
	Button* buttonList;
	Uint16 buttonCount;
	Uint32 flags;
} Panel;

void initToolPanel();

bool updateButton(Button* item, SDL_Point* offset);
void drawButton(Button* item, SDL_Point* offset);

void updatePanel(Panel* panel);
void drawPanel(Panel* panel);

#endif

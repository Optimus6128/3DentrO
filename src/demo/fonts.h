#ifndef FONTS_H
#define FONTS_H

#include "graphics.h"
#include "sprite_engine.h"

typedef struct TextSpritesList
{
	int numChars;
	Sprite **chars;
}TextSpritesList;


void initFonts(void);

TextSpritesList *generateTextCCBs(char *text);

#endif

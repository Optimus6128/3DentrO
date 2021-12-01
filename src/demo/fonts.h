#ifndef FONTS_H
#define FONTS_H

#include "graphics.h"
#include "sprite_engine.h"

enum { FONTPOS_TYPE_SWIRL };

typedef struct FontPos
{
	int posX, posY;
	int zoom;
	int angle;
}FontPos;

typedef struct TextSpritesList
{
	int numChars;
	Sprite **chars;
	FontPos *startPos;
	FontPos *endPos;
}TextSpritesList;


void initFonts(void);

TextSpritesList *generateTextCCBs(char *text);

void setStartFontPos(int type, TextSpritesList* textSprites, int a, int aa);
void setEndFontPos(int posX, int posY, TextSpritesList* textSprites);

#endif

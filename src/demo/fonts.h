#ifndef FONTS_H
#define FONTS_H

#include "graphics.h"
#include "sprite_engine.h"

enum { FONTPOS_ORIGIN, FONTPOS_LINEAR, FONTPOS_3DO, FONTPOS_SWIRL };

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

void setFontsAnimPos(int type, TextSpritesList* textSprites, int posX, int posY, int angleStart, int angleInc, bool isStart);
void updateFontAnimPos(TextSpritesList* textSprites, int timeF16);
int getAnimIntervalF16(int t0, int t1, int t);

#endif

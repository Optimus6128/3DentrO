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
	int animSpeedF8;
}FontPos;

typedef struct TextSpritesList
{
	int numChars;
	Sprite **chars;
	FontPos *startPos;
	FontPos *endPos;
}TextSpritesList;

extern int pixcFades[9];

void initFonts(void);

TextSpritesList *generateTextCCBs(char *text);

CCB *getFontsCel(void);
void setFontsPalette(TextSpritesList *textSprites, uint16 *pal);
void setFontsAnimPos(int type, TextSpritesList* textSprites, int posX, int posY, int angleStart, int angleInc, bool isStart);
void updateFontAnimPos(TextSpritesList* textSprites, int timeF16, bool mustZoomRotate);
void waveFontAnimPos(TextSpritesList* textSprites, int s0, int s1, int f0, int f1, int a0, int a1, int t, int offX);
int getAnimIntervalF16(int t0, int t1, int t);

void updateSineScroll(char *text, CCB **scrollParts, int t);

#endif

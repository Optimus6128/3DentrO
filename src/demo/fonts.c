#include "types.h"
#include "fonts.h"

#include "core.h"
#include "system_graphics.h"
#include "sprite_engine.h"

#define FONT_WIDTH 16
#define FONT_HEIGHT 16
#define FONT_SIZE (FONT_WIDTH * FONT_HEIGHT)

#define FONTS_PAL_SIZE 16
#define FONTS_MAP_SIZE 256

#define NUM_FONTS 84


static CCB *fonts;

static unsigned char fontMap[256] = {	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
										20,21,22,23,24,0,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,56,0,57,
										0,0,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,0,0,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

TextSpritesList *generateTextCCBs(char *text)
{
	int i;

	TextSpritesList *textSprites = (TextSpritesList*)AllocMem(sizeof(TextSpritesList), MEMTYPE_ANY);
	textSprites->numChars = strlen(text);

	textSprites->chars = (Sprite**)AllocMem(sizeof(Sprite*) * textSprites->numChars, MEMTYPE_ANY);
	textSprites->startPos = (FontPos*)AllocMem(sizeof(FontPos) * textSprites->numChars, MEMTYPE_ANY);
	textSprites->endPos = (FontPos*)AllocMem(sizeof(FontPos) * textSprites->numChars, MEMTYPE_ANY);

	for (i=0; i<textSprites->numChars; ++i) {
		int *dstPtr = (int*)fonts->ccb_SourcePtr;
		int fontOff = fontMap[text[i]];
		if (fontOff!=0) dstPtr += ((fontOff-1) << 1);

		textSprites->chars[i] = newSprite(FONT_WIDTH, FONT_HEIGHT, 4, CREATECEL_CODED, fonts->ccb_PLUTPtr, (ubyte*)fonts->ccb_SourcePtr);
		textSprites->chars[i]->cel->ccb_PRE1 = (textSprites->chars[i]->cel->ccb_PRE1 & ~PRE1_WOFFSET8_MASK) | (((fonts->ccb_Width >> 3) - 2) << PRE1_WOFFSET8_SHIFT);
		textSprites->chars[i]->cel->ccb_SourcePtr = (CelData*)dstPtr;

		textSprites->startPos[i].animSpeedF8 = 256;
		textSprites->endPos[i].animSpeedF8 = 256;

		if (i>0) LinkCel(textSprites->chars[i-1]->cel, textSprites->chars[i]->cel);
		if (fontOff==0) textSprites->chars[i]->cel->ccb_Flags |= CCB_SKIP;
	}

	return textSprites;
}

static void setPositionStyle(int type, int i, int perc, int posX, int posY, int angle, FontPos *fPos)
{
	switch(type) {
		case FONTPOS_ORIGIN:
		{
			fPos->posX = SCREEN_WIDTH/2;
			fPos->posY = SCREEN_HEIGHT/2;
			fPos->zoom = 1 - i * 128;
			fPos->angle = (i+1) * 32768;
		} break;

		case FONTPOS_LINEAR:
		{
			fPos->posX = posX + i * FONT_WIDTH;
			fPos->posY = posY;
			fPos->zoom = 256;
			fPos->angle = 0;
		} break;
		
		case FONTPOS_3DO:
		{
			fPos->zoom = 384;
			fPos->angle = 0;
			if (i < 3) {
				fPos->posX = posX + (i-1) * FONT_WIDTH * 1.75 + 4;
				fPos->posY = posY - 48 - 8 * (i&1);
				fPos->angle = (1-i) * 1024;
			} else if (i < 5) {
				fPos->posX = posX + (i-4) * FONT_WIDTH * 1.75 + 16;
				fPos->posY = posY;
			} else {
				fPos->posX = posX + (i-7) * FONT_WIDTH * 1.75 + 8;
				fPos->posY = posY + 48;
				if (i==5 || i==9) {fPos->posY -=8;}
				if (i==6 || i==8) {fPos->posY -=4;}
				fPos->angle = (i-7) * 1024;
			}
		} break;

		case FONTPOS_SWIRL:
		{
			fPos->posX = SCREEN_WIDTH/2 + ((CosF16(angle<<16) * (320 + perc)) >> 16);
			fPos->posY = SCREEN_HEIGHT/2 + ((SinF16(angle<<16) * (320 + perc)) >> 16);
			fPos->zoom = 2048;
			fPos->angle = 512 + 16 * perc;
		} break;
	}
}

static int interpolateValue(int x0, int x1, int timeF16)
{
	return (x0 * (65536 - timeF16) + x1 * timeF16) >> 16;
}

int getAnimIntervalF16(int t0, int t1, int t)
{
	return ((t-t0) << 16) / (t1-t0);
}

void updateFontAnimPos(TextSpritesList* textSprites, int timeF16)
{
	int i;
	FontPos fp;

	for (i=0; i<textSprites->numChars; ++i) {
		FontPos *fpStart = &textSprites->startPos[i];
		FontPos *fpEnd = &textSprites->endPos[i];
		int newTimeF16 = (timeF16 * textSprites->endPos[i].animSpeedF8) >> 8;
		if (newTimeF16 > 65536) newTimeF16 = 65536;

		fp.posX = interpolateValue(fpStart->posX, fpEnd->posX, newTimeF16);
		fp.posY = interpolateValue(fpStart->posY, fpEnd->posY, newTimeF16);
		fp.zoom = interpolateValue(fpStart->zoom, fpEnd->zoom, newTimeF16);
		fp.angle = interpolateValue(fpStart->angle, fpEnd->angle, newTimeF16);

		setSpritePositionZoomRotate(textSprites->chars[i], fp.posX, fp.posY, fp.zoom, fp.angle);
	}
}

void setFontsAnimPos(int type, TextSpritesList* textSprites, int posX, int posY, int angleStart, int angleInc, bool isStart)
{
	int i;
	for (i=0; i<textSprites->numChars; ++i) {
		const int perc = (32 * i) / (textSprites->numChars-1);

		FontPos *fPos = &textSprites->startPos[i];
		if (!isStart) {
			fPos = &textSprites->endPos[i];
		}
		setPositionStyle(type, i, perc, posX, posY, angleStart, fPos);

		textSprites->endPos[i].animSpeedF8 = 1280 - (perc << 5);

		angleStart += angleInc;
	}
}

void initFonts()
{
	fonts = LoadCel("data/fonts.cel", MEMTYPE_ANY);
}

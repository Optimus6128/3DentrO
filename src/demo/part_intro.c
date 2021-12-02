#include "core.h"

#include "part_intro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"

static CCB *radialCel[8];
static TextSpritesList *myText1;

static uint16 radialPals[248];

static void copyRadialPalsToCels()
{
	int i;
	uint16 *src = radialPals;
	for (i=0; i<8; ++i) {
		uint16 *dst = (uint16*)radialCel[7-i]->ccb_PLUTPtr + 1;
		memcpy(dst, src, 31*2);
	}
}

static void animateRadialPals()
{
	int i;
	for (i=0; i<248; ++i) {
		radialPals[i] = getRand(0, 32767);
	}
}

void partIntroInit()
{
	static char celFilename[17];
	int i;

	for (i=0; i<8; ++i) {
		sprintf(celFilename, "data/radial%d.cel", 7-i);
		radialCel[i] = LoadCel(celFilename, MEMTYPE_ANY);
		if (i>0) LinkCel(radialCel[i-1], radialCel[i]);
	}

	myText1 = generateTextCCBs("3DO IS BACK!");

	setStartFontPos(FONTPOS_TYPE_SWIRL, myText1, 0, 24);
	setEndFontPos(SCREEN_WIDTH/2 - 96, SCREEN_HEIGHT/2 - 8, myText1);

	for (i=0; i<myText1->numChars; ++i) {
		//FontPos *fpos = &myText1->startPos[i];
		FontPos *fpos = &myText1->endPos[i];
		setSpritePositionZoomRotate(myText1->chars[i], fpos->posX, fpos->posY, fpos->zoom, fpos->angle);
	}
}

void partIntroRun()
{
	animateRadialPals();
	copyRadialPalsToCels();

	drawCels(radialCel[0]);

	drawSprite(myText1->chars[0]);
}

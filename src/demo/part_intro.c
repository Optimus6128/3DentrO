#include "core.h"

#include "part_intro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"

static TextSpritesList *myText1;

static CCB *radialCel[8];

static uint16 radialPals[256];
static uint16 palsAnim[1024];

static uint16 fuckPal[16];

static void copyRadialPalsToCels()
{
	int i;
	for (i=0; i<8; ++i) {
		//radialCel[7-i]->ccb_PLUTPtr = &radialPals[i<<5];
		radialCel[i]->ccb_PLUTPtr = &radialPals[i<<5];
	}
}

static void animateRadialPals(int t)
{
	int i,j,n=0,k=0;
	for (j=0; j<4; ++j) {
		for (i=1; i<32; ++i) {
			const int tt1 = k + t;
			const int tt2 = 2*k - t;
			const int tt3 = 3*k + t;
			const int tt4 = 5*k - 2*t;
			int ii = t + (SinF16(tt1<<14) >> 8) + (CosF16(tt2<<15) >> 9) + (SinF16(tt3<<16) >> 10) + (CosF16(tt4<<17) >> 11);
			radialPals[n+i] = palsAnim[ii & 1023];
			++k;
		}
		n+=32;
	}

	for (i=0; i<128; ++i) {
		radialPals[255-i] = radialPals[i+1];
	}
}

void partIntroInit()
{
	static char celFilename[17];
	int i;

	for (i=0; i<8; ++i) {
		//sprintf(celFilename, "data/radial%d.cel", 7-i);
		sprintf(celFilename, "data/radial%d.cel", i);
		radialCel[i] = LoadCel(celFilename, MEMTYPE_ANY);
		if (i>0) LinkCel(radialCel[i-1], radialCel[i]);
	}

	myText1 = generateTextCCBs("3DO IS BACK!");

	setStartFontPos(FONTPOS_TYPE_SWIRL, myText1, 0, 24);
	setEndFontPos(SCREEN_WIDTH/2 - 96, SCREEN_HEIGHT/2 - 8, myText1);

	for (i=0; i<8; ++i) {
		const int c = i << 7;
		int cc = c;

		setPalGradient(c,c+31, 0,0,0, 			(cc*7)>>10,	(cc*3)>>10,	(cc*23)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+32,c+63, 	(cc*2)>>10,(cc*1)>>10,	(cc*3)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+64,c+95, 	(cc*23)>>10,(cc*7)>>10,	(cc*12)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+96,c+127, (cc*1)>>10,	(cc*2)>>10,(cc*3)>>10, palsAnim);
	}

	setPalGradient(0,15, 0,0,0, 31,31,31, fuckPal);
	for (i=0; i<myText1->numChars; ++i) {
		//FontPos *fpos = &myText1->startPos[i];
		FontPos *fpos = &myText1->endPos[i];
		setSpritePositionZoomRotate(myText1->chars[i], fpos->posX, fpos->posY, fpos->zoom, fpos->angle);
		myText1->chars[i]->cel->ccb_PLUTPtr = fuckPal;
	}
}

void partIntroRun()
{
	const int time = getFrameNum();

	animateRadialPals(time);
	copyRadialPalsToCels();

	drawCels(radialCel[0]);

	drawSprite(myText1->chars[0]);
}

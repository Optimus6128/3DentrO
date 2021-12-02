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

static bool isIntroInit = false;


static void copyRadialPalsToCels()
{
	int i;
	for (i=0; i<8; ++i) {
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

	if (isIntroInit) return;

	for (i=0; i<8; ++i) {
		sprintf(celFilename, "data/radial%d.cel", i);
		radialCel[i] = LoadCel(celFilename, MEMTYPE_ANY);
	}

	for (i = 0; i < 8; ++i) {
		const int q = i >> 1;
		const int yp = q >> 1;
		int xp = q;
		if (xp > 1) xp = 3 - xp;

		radialCel[i]->ccb_XPos = (xp * SCREEN_WIDTH / 2) << 16;
		radialCel[i]->ccb_YPos = (yp * SCREEN_HEIGHT / 2) << 16;
	}

	// Unorthodox link order hack in the hope of covering some black buggy pixels from the packer (that I can't fix now because of the deadline)
	for (i=1; i<4; ++i) {
		LinkCel(radialCel[i-1], radialCel[i]);
		LinkCel(radialCel[8-i], radialCel[7-i]);
	}
	LinkCel(radialCel[3], radialCel[7]);

	myText1 = generateTextCCBs("3DOISBACK!");

	setFontsAnimPos(FONTPOS_ORIGIN, myText1, 0, 0, 0, 24, true);
	setFontsAnimPos(FONTPOS_3DO, myText1, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, 0, false);

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
		myText1->chars[i]->cel->ccb_PLUTPtr = fuckPal;
	}

	isIntroInit = true;
}

static void textAnimScript(int t)
{
	if (t > 500 && t < 2000) {
		updateFontAnimPos(myText1, getAnimIntervalF16(500, 2000, t));
	}
	
	if (t > 500) drawSprite(myText1->chars[0]);
}

void partIntroRun(int ticks)
{
	const int time = ticks >> 4;

	animateRadialPals(time);
	copyRadialPalsToCels();

	drawCels(radialCel[0]);

	textAnimScript(ticks);
}

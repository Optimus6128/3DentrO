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

static uint16 introFontsPal[16];

static bool isIntroInit = false;


static void copyRadialPalsToCels()
{
	int i;
	for (i=0; i<8; ++i) {
		radialCel[i]->ccb_PLUTPtr = &radialPals[i<<5];
	}
}

static void animateRadialPals(int t, int fadeShift)
{
	int i,j,n=0,k=0;

	for (j=0; j<4; ++j) {
		for (i=1; i<32; ++i) {
			const int tt1 = k + t;
			const int tt2 = 2*k - t;
			const int tt3 = 3*k + t;
			const int tt4 = 5*k - 2*t;
			//const int ii = t + (SinF16(tt1<<14) >> 8) + (CosF16(tt2<<15) >> 9) + (SinF16(tt3<<16) >> 10) + (CosF16(tt4<<17) >> 11);
			const int ii = t + (sinF16[(tt1>>2) & 255] >> 8) + (cosF16[(tt2>>1) & 255] >> 9) + (sinF16[tt3 & 255] >> 10) + (cosF16[(tt4<<1) & 255] >> 11);
			const uint16 palVal = palsAnim[ii & 1023];
			const int r = ((palVal >> 10) & 31) >> fadeShift;
			const int g = ((palVal >> 5) & 31) >> fadeShift;
			const int b = (palVal & 31) >> fadeShift;
			radialPals[n+i] = (r << 10) | (g << 5) | b;
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
		if (i>0) LinkCel(radialCel[i-1], radialCel[i]);
	}

	for (i = 0; i < 8; ++i) {
		const int q = i >> 1;
		const int yp = q >> 1;
		int xp = q;
		if (xp > 1) xp = 3 - xp;

		radialCel[i]->ccb_XPos = (xp * SCREEN_WIDTH / 2) << 16;
		radialCel[i]->ccb_YPos = (yp * SCREEN_HEIGHT / 2) << 16;
	}

	myText1 = generateTextCCBs("3DOISBACK!");

	setPal(0, 0,0,0, introFontsPal);
	setPalGradient(1,7, 1,3,7, 31,29,21, introFontsPal);
	setFontsPalette(myText1, introFontsPal);

	setFontsAnimPos(FONTPOS_ORIGIN, myText1, 0, 0, 0, 24, true);
	setFontsAnimPos(FONTPOS_3DO, myText1, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, 0, false);

	for (i=0; i<8; ++i) {
		const int c = i << 7;
		int cc = (c * 3)>>1;

		setPalGradient(c,c+31, 0,0,0, 			(cc*7)>>10,	(cc*3)>>10,	(cc*23)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+32,c+63, 	(cc*2)>>10,(cc*1)>>10,	(cc*3)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+64,c+95, 	(cc*23)>>10,(cc*7)>>10,	(cc*12)>>10, palsAnim);
		setPalGradientFromPrevIndex(c+96,c+127, (cc*1)>>10,	(cc*2)>>10,(cc*3)>>10, palsAnim);
	}

	isIntroInit = true;
}

static int fadeOutVal = 1;

static void textAnimScript(int t)
{
	static bool initSecondAnim = false;

	if (t > 3000 && t < 7000) {
		updateFontAnimPos(myText1, getAnimIntervalF16(3000, 7000, t), true);
	}

	if (!initSecondAnim && t > 7000) {
		updateFontAnimPos(myText1, 65536, true);
		setFontsAnimPos(FONTPOS_3DO, myText1, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, 0, true);
		setFontsAnimPos(FONTPOS_SWIRL, myText1, 0, 0, 0, 32, false);
		initSecondAnim = true;
	}

	if (t > 15150 && t < 23000) {
		updateFontAnimPos(myText1, getAnimIntervalF16(15150, 23000, t), true);
	}

	if (t > 3000 && t < 23000) drawSprite(myText1->chars[0]);

	if (t > 23000 & t < 25000) {
		fadeOutVal = (t - 23000) >> 8;
	}
}

void partIntroRun(int ticks, int dt)
{
	animateRadialPals(ticks >> 4, fadeOutVal);
	copyRadialPalsToCels();

	drawCels(radialCel[0]);

	textAnimScript(ticks);
}

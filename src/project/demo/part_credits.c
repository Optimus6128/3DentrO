#include "core.h"

#include "part_credits.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"

#define NUM_STARS 8

static Sprite *radialSpr;
static CCB *skyCel;
static Sprite *star8[NUM_STARS];
static int starSpeeds[NUM_STARS] = {1,2,3,4,2,3,1,2};

static bool isCreditsInit = false;

static uint16 creditsFontsPal[6][16];

static TextSpritesList *creditsText[6];
static char *creditsStr[6] = { "CODE", "OPTIMUS", "GFX", "HAM & OPTIMUS", "MUSIC", "HAM" };


void partCreditsInit()
{
	int i,j=0;
	static int jj[3] = {16, 144, 0};

	if (isCreditsInit) return;

	radialSpr = loadSpriteCel("data/radial.cel");

	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);
	skyCel->ccb_HDX = SCREEN_WIDTH << 20;

	for (i=0; i<NUM_STARS; ++i) {
		if (i==0) {
			star8[i] = loadSpriteCel("data/star8.cel");
		} else {
			star8[i] = newSprite(star8[0]->width, star8[0]->height, 8, CREATECEL_CODED, (uint16*)star8[0]->cel->ccb_PLUTPtr, (ubyte*)star8[0]->cel->ccb_SourcePtr);
			star8[i]->cel->ccb_Flags = star8[0]->cel->ccb_Flags;
			star8[i]->cel->ccb_SourcePtr = star8[0]->cel->ccb_SourcePtr;
			star8[i]->cel->ccb_PLUTPtr = star8[0]->cel->ccb_PLUTPtr;
		}
		star8[i]->posX = getRand(640, 1280);
		star8[i]->posY = getRand(48, 192);
		star8[i]->zoom = 256;
		star8[i]->angle = 0;
	}

	for (i=0; i<NUM_STARS; ++i) {
		if (i>0) LinkCel(star8[i-1]->cel, star8[i]->cel);
	}

	for (i=0; i<6; ++i) {
		creditsText[i] = generateTextCCBs(creditsStr[i]);
		setFontsPalette(creditsText[i], creditsFontsPal[i]);
		setPal(0, 0,0,0, creditsFontsPal[i]);
	}
	setPalGradient(1,7, 11,5,3, 31,23,15, creditsFontsPal[0]);
	setPalGradient(1,7, 7,11,3, 15,27,7, creditsFontsPal[1]);
	
	setPalGradient(1,7, 3,5,11, 7,23,31, creditsFontsPal[2]);
	setPalGradient(1,7, 3,11,7, 7,27,15, creditsFontsPal[3]);
	
	setPalGradient(1,7, 11,3,5, 31,15,23, creditsFontsPal[4]);
	setPalGradient(1,7, 11,7,3, 27,20,11, creditsFontsPal[5]);

	for (i=0; i<6; i+=2) {
		if (i>0) j = 1;

		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i], -SCREEN_WIDTH/4, SCREEN_HEIGHT/8, 0, 0, true);
		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i], SCREEN_WIDTH/10, SCREEN_HEIGHT/8, 0, 0, false);

		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i+1], SCREEN_WIDTH + SCREEN_WIDTH/4, (7*SCREEN_HEIGHT)/8-16, 0, 0, true);
		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i+1], ((5+j)*SCREEN_WIDTH)/8-jj[i>>1], (7*SCREEN_HEIGHT)/8-16, 0, 0, false);
	}

	isCreditsInit = true;
}

static void animSky(int t)
{
	static int skyScale = 16 * 256;
	const int offf = 3500;
	int pixI = 0;

	if (t < 256) {
		pixI = t>>5;
	} else
	if (t < 1800) {
		pixI = 8;
		skyScale = 16 * 256 - ((getAnimIntervalF16(256, 1800, t) * 15 * 256) >> 16);
	}
	if (t > 1800 && t < 29000+offf) skyScale = 256;
	if (t > 29000+offf && t < 30000+offf) {
		skyScale = ((getAnimIntervalF16(29000+offf, 30000+offf, t) * 16 * 256) >> 16);
	}
	if (t > 30000+offf && t < 30500+offf) {
		pixI = 7 - ((t - (30000+offf))>>6);
	}


	if (t < 30500+offf) {
		if (skyScale < 256) skyScale = 256;
		if (skyScale > 16 * 256) skyScale = 16 * 256;

		skyCel->ccb_YPos = ((((SCREEN_HEIGHT/2) << 8) - skyScale * (SCREEN_HEIGHT/2)) << 8);
		skyCel->ccb_VDY = skyScale << 8;

		CLAMP(pixI,0,8)
		skyCel->ccb_PIXC = pixcFades[pixI];

		drawCels(skyCel);
	}
}

static void animRadial(int t)
{
	static int zoom = 0;
	static int radialFade = 0;

	int radialPosX = SCREEN_WIDTH / 2 + (SinF16(t << 10) >> 9);
	int radialPosY = SCREEN_HEIGHT / 2 + (SinF16(t << 11) >> 10);
	//int radialPosX = SCREEN_WIDTH / 2 + (sinF16[(t >> 6) & 255] >> 9);
	//int radialPosY = SCREEN_HEIGHT / 2 + (sinF16[(t >> 5) & 255] >> 10);


	if (t > 2000 && t < 5000) {
		zoom = getAnimIntervalF16(2000, 5000, t) >> 8;
	}
	if (t > 5000) zoom = 256;

	if (t > 31000) {
		zoom = 256 - (getAnimIntervalF16(31000, 33000, t) >> 8);
	}
	
	radialFade = (zoom >> 5) + (SinF16(t << 12) >> 14) - 3;
	//radialFade = (zoom >> 5) + (sinF16[(t >> 4) & 255] >> 14) - 3;
	CLAMP(radialFade, 0, 4)

	radialSpr->cel->ccb_PIXC = pixcFades[radialFade];

	setSpritePositionZoomRotate(radialSpr, radialPosX, radialPosY, zoom, t<<3);
	drawSprite(radialSpr);
}

static void animStars(int t, int dt)
{
	int i;
	const int speedScale = 1 + (int)(dt > 26);

	for (i=0; i<NUM_STARS; ++i) {
		//int zoom = (CosF16(t<<(12 + ((i+3) & 3))) + 65536) >> 14;
		//int fade = (SinF16(t<<(8 + ((i+7) & 3))) + 65536) >> 14;
		int zoom = (cosF16[((t<<(12 + ((i+3) & 3)))>>16) & 255] + 65536) >> 14;
		int fade = (sinF16[((t<<(8 + ((i+7) & 3)))>>16) & 255] + 65536) >> 14;
		CLAMP(fade, 1, 5)

		star8[i]->cel->ccb_PIXC = pixcFades[fade];

		star8[i]->posX -= starSpeeds[i] * speedScale;

		if (t<26000 && star8[i]->posX < -32) star8[i]->posX = SCREEN_WIDTH + getRand(32, 128);

		setSpritePositionZoomRotate(star8[i], star8[i]->posX, star8[i]->posY, 256 - (zoom << 4), t<<(3 + (i & 3)));
	}
	drawSprite(star8[0]);
}

static void animCredits(int t)
{
	const int tRange[6] = {5000,12000, 12500,19500, 20000,27000};

	int i,j;
	for (j=0; j<6; ++j) {
		for (i=0; i<creditsText[j]->numChars; ++i) {
		creditsText[j]->endPos[i].animSpeedF8 = 640 - (i << 5);
		}
	}

	for (i=0; i<6; i+=2) {
		const int t0 = tRange[i];
		const int t1 = tRange[i+1];
		const int tOff = 1500;
		if (t > t0 && t < t1) {
			updateFontAnimPos(creditsText[i], getAnimIntervalF16(t0, t0+tOff, t), false);
			updateFontAnimPos(creditsText[i+1], getAnimIntervalF16(t0+tOff, t0+2*tOff, t), false);
			drawSprite(creditsText[i]->chars[0]);
			drawSprite(creditsText[i+1]->chars[0]);
		}
	}
}

void partCreditsRun(int ticks, int dt)
{
	animSky(ticks);

	if (ticks > 2000) {
		animRadial(ticks);
	}

	if (ticks > 3000) {
		animStars(ticks, dt);
	}
	animCredits(ticks);
}

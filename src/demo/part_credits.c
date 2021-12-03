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
static int starSpeeds[NUM_STARS] = {1,2,3,1,2,3,2,1};

static bool isCreditsInit = false;

static int pixcFades[8] = {0x0380, 0x0780, 0x0B80, 0x0F80, 0x1380, 0x1780, 0x1B80, 0x1F80};

static uint16 fuckPal2[7];

static TextSpritesList *creditsText[6];

void partCreditsInit()
{
	int i,j=0;

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

	creditsText[0] = generateTextCCBs("CODE");
	creditsText[1] = generateTextCCBs("OPTIMUS");
	creditsText[2] = generateTextCCBs("FONTS");
	creditsText[3] = generateTextCCBs("HAM");
	creditsText[4] = generateTextCCBs("MUSIC");
	creditsText[5] = generateTextCCBs("HAM");

	for (i=0; i<6; i+=2) {
		if (i > 0) j = 1;
		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i], -SCREEN_WIDTH/4, SCREEN_HEIGHT/8, 0, 0, true);
		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i], SCREEN_WIDTH/10, SCREEN_HEIGHT/8, 0, 0, false);

		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i+1], SCREEN_WIDTH + SCREEN_WIDTH/4, (7*SCREEN_HEIGHT)/8, 0, 0, true);
		setFontsAnimPos(FONTPOS_LINEAR, creditsText[i+1], ((5+j)*SCREEN_WIDTH)/8, (7*SCREEN_HEIGHT)/8, 0, 0, false);
	}

	setPalGradient(0,7, 0,0,0, 31,31,31, fuckPal2);	// A bug happens and the fonts lose their palette or have wrong pointerafter loading, so we force to get it again
	for (j=0; j<6; ++j) {
		for (i=0; i<creditsText[j]->numChars; ++i) {
			creditsText[j]->chars[i]->cel->ccb_PLUTPtr = fuckPal2;
		}
	}

	isCreditsInit = true;
}

static void animSky(int t)
{
	static int skyScale = 16 * 256;

	if (t > 100 && t < 1800) {
		skyScale = 16 * 256 - ((getAnimIntervalF16(100, 1800, t) * 15 * 256) >> 16);
	}
	if (t > 1800) skyScale = 256;
	if (skyScale < 256) skyScale = 256;

	skyCel->ccb_YPos = ((((SCREEN_HEIGHT/2) << 8) - skyScale * (SCREEN_HEIGHT/2)) << 8);
	skyCel->ccb_VDY = skyScale << 8;

	drawCels(skyCel);
}

static void animRadial(int t)
{
	static int zoom = 0;
	static int skyFade = 0;

	int radialPosX = SCREEN_WIDTH / 2 + (SinF16(t << 10) >> 9);
	int radialPosY = SCREEN_HEIGHT / 2 + (SinF16(t << 11) >> 10);


	if (t > 2000 && t < 5000) {
		zoom = getAnimIntervalF16(2000, 5000, t) >> 8;
	}
	if (t > 5000) zoom = 256;
	
	skyFade = (zoom >> 5) + (SinF16(t << 12) >> 14) - 3;
	CLAMP(skyFade, 0, 4)

	radialSpr->cel->ccb_PIXC = pixcFades[skyFade];

	setSpritePositionZoomRotate(radialSpr, radialPosX, radialPosY, zoom, t<<3);
	drawSprite(radialSpr);
}

static void animStars(int t)
{
	int i;

	for (i=0; i<NUM_STARS; ++i) {
		int zoom = (CosF16(t<<(12 + ((i+3) & 3))) + 65536) >> 14;
		int fade = (SinF16(t<<(8 + ((i+7) & 3))) + 65536) >> 14;
		CLAMP(fade, 1, 5)

		star8[i]->cel->ccb_PIXC = pixcFades[fade];
		
		star8[i]->posX -= starSpeeds[i];
		if (star8[i]->posX < -32) star8[i]->posX = SCREEN_WIDTH + getRand(32, 128);
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
			updateFontAnimPos(creditsText[i], getAnimIntervalF16(t0, t0+tOff, t));
			updateFontAnimPos(creditsText[i+1], getAnimIntervalF16(t0+tOff, t0+2*tOff, t));
			drawSprite(creditsText[i]->chars[0]);
			drawSprite(creditsText[i+1]->chars[0]);
		}
	}
}

static void creditsAnimScript(int t)
{
	animSky(t);

	if (t > 2000) {
		animRadial(t);
	}

	if (t > 3000) {
		animStars(t);
	}
	animCredits(t);
}

void partCreditsRun(int ticks)
{
	creditsAnimScript(ticks);
}

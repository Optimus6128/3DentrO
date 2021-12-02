#include "core.h"

#include "part_credits.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"


static Sprite *radialSpr;
static CCB *skyCel;
static Sprite *star8;

static bool isCreditsInit = false;

static int pixcFades[8] = {0x0380, 0x0780, 0x0B80, 0x0F80, 0x1380, 0x1780, 0x1B80, 0x1F80};

void partCreditsInit()
{
	if (isCreditsInit) return;

	radialSpr = loadSpriteCel("data/radial.cel");
	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);
	star8 = loadSpriteCel("data/star8.cel");

	skyCel->ccb_HDX = SCREEN_WIDTH << 20;
	//radialSpr->cel->ccb_PIXC = 0x1780;
	radialSpr->cel->ccb_PIXC = 0x0F80;
	
	isCreditsInit = true;
}

static void animSky(int t)
{
	drawCels(skyCel);
}

static void animRadial(int t)
{
	setSpritePositionZoomRotate(radialSpr, SCREEN_WIDTH/8, SCREEN_HEIGHT/8, 256, t<<3);
	drawSprite(radialSpr);
}

static void animStars(int t)
{
	int zoom = (CosF16(t<<14) + 65536) >> 14;
	int fade = (SinF16(t<<14) + 65536) >> 14;
	CLAMP(fade, 3, 7)

	star8->cel->ccb_PIXC = pixcFades[fade];
	setSpritePositionZoomRotate(star8, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 256 - (zoom << 3), t<<4);
	drawSprite(star8);
}

static void creditsAnimScript(int t)
{
	animSky(t);

	animRadial(t);

	animStars(t);
}

void partCreditsRun(int ticks)
{
	creditsAnimScript(ticks);
}

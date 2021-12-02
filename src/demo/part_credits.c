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


void partCreditsInit()
{
	if (isCreditsInit) return;

	radialSpr = loadSpriteCel("data/radial.cel");
	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);
	star8 = loadSpriteCel("data/star8.cel");

	skyCel->ccb_HDX = SCREEN_WIDTH << 20;
	radialSpr->cel->ccb_PIXC = 0x1780;
	//radialSpr->cel->ccb_PIXC = 0x0F80;
	
	star8->cel->ccb_PIXC = 0x1F80;

	isCreditsInit = true;
}

void partCreditsRun(int ticks)
{
	drawCels(skyCel);

	setSpritePositionZoomRotate(radialSpr, SCREEN_WIDTH/8, SCREEN_HEIGHT/8, 256, ticks<<3);
	drawSprite(radialSpr);

	setSpritePositionZoomRotate(star8, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 256, ticks<<4);
	drawSprite(star8);
}

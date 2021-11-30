#include "core.h"

#include "part_intro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"


Sprite *radialSpr;
CCB *skyCel;

void partIntroInit()
{
	radialSpr = loadSpriteCel("data/radial.cel");
	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);

	skyCel->ccb_HDX = SCREEN_WIDTH << 20;
	radialSpr->cel->ccb_PIXC = 0x1780;
	//radialSpr->cel->ccb_PIXC = 0x0F80;
}

void partIntroRun()
{
	const int time = getFrameNum();

	drawCels(skyCel);

	setSpritePositionZoomRotate(radialSpr, SCREEN_WIDTH/8, SCREEN_HEIGHT/8, 256, time<<7);
	drawSprite(radialSpr);
}

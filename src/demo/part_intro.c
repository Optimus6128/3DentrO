#include "core.h"

#include "part_intro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"


Sprite *radialSpr;
CCB *skyCel;
TextSpritesList *myText;

void partIntroInit()
{
	int i;

	radialSpr = loadSpriteCel("data/radial.cel");
	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);

	skyCel->ccb_HDX = SCREEN_WIDTH << 20;
	radialSpr->cel->ccb_PIXC = 0x1780;
	//radialSpr->cel->ccb_PIXC = 0x0F80;

	myText = generateTextCCBs("3DO IS BACK!");

	for (i=0; i<myText->numChars; ++i) {
		setSpritePosition(myText->chars[i], i<<4, 100);
	}
}

void partIntroRun()
{
	const int time = getFrameNum();

	drawCels(skyCel);

	setSpritePositionZoomRotate(radialSpr, SCREEN_WIDTH/8, SCREEN_HEIGHT/8, 256, time<<7);
	drawSprite(radialSpr);

	drawSprite(myText->chars[0]);
}

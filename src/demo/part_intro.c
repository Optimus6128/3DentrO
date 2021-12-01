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
TextSpritesList *myText1;

void partIntroInit()
{
	int i;

	radialSpr = loadSpriteCel("data/radial.cel");
	skyCel = LoadCel("data/sky1.cel", MEMTYPE_CEL);

	skyCel->ccb_HDX = SCREEN_WIDTH << 20;
	radialSpr->cel->ccb_PIXC = 0x1780;
	//radialSpr->cel->ccb_PIXC = 0x0F80;

	myText1 = generateTextCCBs("3DO IS BACK!");

	setStartFontPos(FONTPOS_TYPE_SWIRL, myText1, 0, 24);
	setEndFontPos(SCREEN_WIDTH/2 - 96, SCREEN_HEIGHT/2 - 8, myText1);

	for (i=0; i<myText1->numChars; ++i) {
		FontPos *fpos = &myText1->startPos[i];
		//FontPos *fpos = &myText1->endPos[i];
		setSpritePositionZoomRotate(myText1->chars[i], fpos->posX, fpos->posY, fpos->zoom, fpos->angle);
	}
}

void partIntroRun()
{
	const int time = getFrameNum();

	drawCels(skyCel);

	setSpritePositionZoomRotate(radialSpr, SCREEN_WIDTH/8, SCREEN_HEIGHT/8, 256, time<<7);
	drawSprite(radialSpr);

	drawSprite(myText1->chars[0]);
}

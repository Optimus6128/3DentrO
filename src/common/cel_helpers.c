#include "types.h"
#include "cel_helpers.h"

#include "core.h"
#include "system_graphics.h"

#include "sprite_engine.h"
#include "tools.h"


static Sprite *feedbackSpr1;
static Sprite *feedbackSpr2;

static int mosaikBufferWidth;
static int mosaikBufferHeight;

int getCelBpp(CCB *cel)
{
	static int celBppValueToBits[8] = { 0,1,2,4,6,8,16,0 };

	return celBppValueToBits[cel->ccb_PRE0 & 7];
}

int getCelDataSizeInBytes(CCB *cel)
{
	int bpp = getCelBpp(cel);
	if (bpp == 6) bpp = 8;

	// doesn't account yet for packed cels or tiny cels (less than 8 bytes row width) with padding
	return (cel->ccb_Width * cel->ccb_Height * bpp) >> 3;
}

/*
// Will finish later. Ness more, bpp, skipping pixels depending on bpp, etc..
// It would be also nice to give the original CCB, get the max width/height from the extra structure that is not loaded to the hardware, get the bpp and other info from the bits
void setupWindowNormalCel(int posX, int posY, int width, int height, int totalWidth, int totalHeight, void *bitmap, CCB *cel)
{
	int woffset;
	int vcnt;

	cel->ccb_PRE1 &= ~PRE1_LRFORM;
	cel->ccb_SourcePtr = (CelData*)bitmap;
	woffset = (width >> 1) - 2;
	vcnt = height - 1;

	// Should spare the magic numbers at some point
	cel->ccb_PRE0 = (cel->ccb_PRE0 & ~(((1<<10) - 1)<<6)) | (vcnt << 6);
	cel->ccb_PRE1 = (cel->ccb_PRE1 & (65536 - 1024)) | (woffset << 16) | (width-1);
}*/

void setupWindowFeedbackCel(int posX, int posY, int width, int height, int bufferIndex, CCB *cel)
{
	int woffset;
	int vcnt;

	cel->ccb_Flags &= ~(CCB_ACSC | CCB_ALSC);	// Super Clipping will lock an LRFORM feedback texture. Disable it!
	cel->ccb_Flags |= CCB_BGND;
	cel->ccb_PRE1 |= PRE1_LRFORM;
	cel->ccb_SourcePtr = (CelData*)(getBackBufferByIndex(bufferIndex) + (posY & ~1) * SCREEN_WIDTH + 2*posX);
	woffset = SCREEN_WIDTH - 2;
	vcnt = (height >> 1) - 1;

	// Should spare the magic numbers at some point
	cel->ccb_PRE0 = (cel->ccb_PRE0 & ~(((1<<10) - 1)<<6)) | (vcnt << 6);
	cel->ccb_PRE1 = (cel->ccb_PRE1 & (65536 - 1024)) | (woffset << 16) | (width-1);
}

void initMosaikEffect(int bufferWidth, int bufferHeight)
{
	mosaikBufferWidth = bufferWidth;
	mosaikBufferHeight = bufferHeight;

	feedbackSpr1 = newFeedbackSprite(0, 0, mosaikBufferWidth, mosaikBufferHeight, 0);
	feedbackSpr2 = newFeedbackSprite(0, 0, mosaikBufferWidth, mosaikBufferHeight, 1);
}

void prepareForMosaikEffect(int mosaikZoom)
{
	CLAMP(mosaikZoom,1,256)

	if (mosaikZoom < 256) {
		setRenderBuffer(0);
		switchRenderToBuffer(true);
	}
}

void renderMosaikEffect(int mosaikZoom)
{
	CLAMP(mosaikZoom,1,256)

	if (mosaikZoom < 256) {
		Quad q;
		int shrinkX = ((mosaikBufferWidth*mosaikZoom) >> 8) & ~1;
		int shrinkY = ((mosaikBufferHeight*mosaikZoom) >> 8) & ~1;

		if (shrinkX < 2) shrinkX = 2;
		if (shrinkY < 2) shrinkY = 2;

		q.ulX = 0;
		q.ulY = 0;
		q.lrX = shrinkX;
		q.lrY = shrinkY;

		setRenderBuffer(1);
		switchRenderToBuffer(true);
		mapZoomSpriteToQuad(feedbackSpr1, &q);
		drawSprite(feedbackSpr1);

		switchRenderToBuffer(false);
		mapFeedbackSpriteToNewFramebufferArea(0,0, shrinkX, shrinkY, 1, feedbackSpr2);

		q.lrX = SCREEN_WIDTH;
		q.lrY = SCREEN_HEIGHT;

		mapZoomSpriteToQuad(feedbackSpr2, &q);
		drawSprite(feedbackSpr2);
	}
}

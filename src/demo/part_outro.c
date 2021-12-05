#include "core.h"

#include "part_outro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"

static CCB *tunnelCel;
static CCB *tunnelWindowCel;
static CCB *tunnelBlob;

static bool isOutroInit = false;

static Sprite *feedbackSpr1;
static Sprite *feedbackSpr2;

static uint16 texPatternDouble[6*5*4];

static int mosaikZoom = 256;

static int getWordOffset10(CCB *cel)
{
	return ((cel->ccb_PRE1 & PRE1_WOFFSET10_MASK) >> PRE1_WOFFSET10_SHIFT) + 2;
}

static void setTunnelWindow(int posX, int posY)
{
	int *dstPtr = (int*)tunnelCel->ccb_SourcePtr;
	dstPtr += posY * getWordOffset10(tunnelCel) + (posX >> 2);
	tunnelWindowCel->ccb_SourcePtr = (CelData*)dstPtr;

	tunnelWindowCel->ccb_PRE0 = (tunnelWindowCel->ccb_PRE0 & ~(PRE0_VCNT_MASK | PRE0_SKIPX_MASK)) | (SCREEN_WIDTH << PRE0_VCNT_SHIFT) | ((posX & 3) << PRE0_SKIPX_SHIFT);
	tunnelWindowCel->ccb_PRE1 = (tunnelWindowCel->ccb_PRE1 & ~(PRE1_TLHPCNT_MASK)) | (SCREEN_WIDTH - 1);
}

static void updateTunnelTexture(int offX, int offY)
{
	int x,y;

	uint16 *src = &texPatternDouble[(offY%5)*12 + (offX%6)];
	uint16 *dst = (uint16*)tunnelWindowCel->ccb_PLUTPtr;
	++dst;	// start from 1, finish at 30
	for (y=0; y<5; ++y) {
		for (x=0; x<6; ++x) {
			*dst++ = *(src+x);
		}
		src+=12;
	}
}

static void saveTunnelPattern()
{
	int x,y;

	uint16 *src = (uint16*)tunnelWindowCel->ccb_PLUTPtr;
	uint16 *dst = texPatternDouble;

	++src;	// start from 1, finish at 30
	for (y=0; y<5; ++y) {
		for (x=0; x<6; ++x) {
			uint16 c = *src++;
			*(dst+y*12+x) = c;
			*(dst+y*12+6+x) = c;
			*(dst+60+y*12+x) = c;
			*(dst+60+y*12+6+x) = c;
		}
	}
}

static void animateTunnel(int t)
{
	const int offX = SCREEN_WIDTH/2 + ((sinF16[(t>>5) & 255] * (SCREEN_WIDTH/2)) >> 16);
	const int offY = SCREEN_HEIGHT/2 + ((sinF16[(t>>6) & 255] * (SCREEN_HEIGHT/2)) >> 16);

	setTunnelWindow(offX, offY);

	updateTunnelTexture(/*t>>5*/0, t>>5);	// x 4 is good, y 4 or 5

	drawCels(tunnelWindowCel);

	tunnelBlob->ccb_XPos = (SCREEN_WIDTH - (tunnelBlob->ccb_Width >> 1) - offX) << 16;
	tunnelBlob->ccb_YPos = (SCREEN_HEIGHT - (tunnelBlob->ccb_Height >> 1) - offY) << 16;
	tunnelBlob->ccb_PIXC = pixcFades[7];

	drawCels(tunnelBlob);
}

void partOutroInit()
{
	if (isOutroInit) return;

	tunnelCel = LoadCel("data/tunnel.cel", MEMTYPE_CEL);
	tunnelBlob = LoadCel("data/tunnel_blob.cel", MEMTYPE_CEL);

	tunnelWindowCel = CloneCel(tunnelCel, CLONECEL_CCB_ONLY);

	saveTunnelPattern();

	feedbackSpr1 = newFeedbackSprite(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	feedbackSpr2 = newFeedbackSprite(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

	isOutroInit = true;
}

static void mosaikZoomScript(int t)
{
	if (t<1024) {
		mosaikZoom = t>>2;
	} else if (t < 42000) {
		mosaikZoom = 256;
	} else if (t < 54000) {
		mosaikZoom = (54000 - t) >> 5;
	} else {
		// quit
		mosaikZoom = 4;
		sendQuit();
	}

	CLAMP(mosaikZoom,4,256)
}

void partOutroRun(int ticks, int dt)
{
	mosaikZoomScript(ticks);

	if (mosaikZoom < 256) {
		setRenderBuffer(0);
		switchRenderToBuffer(true);
	}

	animateTunnel(ticks);

	if (mosaikZoom < 256) {
		const int shrinkX = ((SCREEN_WIDTH*mosaikZoom) >> 8) & ~1;
		const int shrinkY = ((SCREEN_HEIGHT*mosaikZoom) >> 8) & ~1;

		Quad q;

		q.ulX = 0;
		q.ulY = 0;
		q.lrX = shrinkX;
		q.lrY = shrinkY;

		setRenderBuffer(1);
		switchRenderToBuffer(true);
		mapZoomSpriteToQuad(feedbackSpr1, &q);
		drawSprite(feedbackSpr1);


		q.lrX = SCREEN_WIDTH;
		q.lrY = SCREEN_HEIGHT;

		switchRenderToBuffer(false);
		mapFeedbackSpriteToNewFramebufferArea(0,0, shrinkX, shrinkY, 1, feedbackSpr2);
		mapZoomSpriteToQuad(feedbackSpr2, &q);
		drawSprite(feedbackSpr2);
	}
	//drawNumber(16,224, ticks);
}

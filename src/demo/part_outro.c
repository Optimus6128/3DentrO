#include "core.h"

#include "part_outro.h"

#include "system_graphics.h"
#include "tools.h"

#include "mathutil.h"
#include "celutils.h"

#include "sprite_engine.h"
#include "fonts.h"

#include "cel_helpers.h"

static CCB *tunnelCel;
static CCB *tunnelWindowCel;
static CCB *tunnelBlob;

static bool isOutroInit = false;

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

	int scaleBufferX = 1 << 20;
	int scaleBufferY = 1 << 16;

	setTunnelWindow(offX, offY);

	updateTunnelTexture(/*t>>5*/0, t>>5);	// x 4 is good, y 4 or 5

	tunnelBlob->ccb_XPos = (SCREEN_WIDTH - (tunnelBlob->ccb_Width >> 1) - offX) << 16;
	tunnelBlob->ccb_YPos = (SCREEN_HEIGHT - (tunnelBlob->ccb_Height >> 1) - offY) << 16;
	tunnelBlob->ccb_PIXC = pixcFades[7];

	// We render the effect if the mosaik is active in half the screen width and height to possibly be at 60fps even when the mosaik is active
	if (mosaikZoom < 256) {
		scaleBufferX >>= 1;
		scaleBufferY >>= 1;
		tunnelBlob->ccb_XPos >>= 1;
		tunnelBlob->ccb_YPos >>= 1;
	}
	tunnelWindowCel->ccb_HDX = scaleBufferX;
	tunnelWindowCel->ccb_VDY = scaleBufferY;
	tunnelBlob->ccb_HDX = scaleBufferX;
	tunnelBlob->ccb_VDY = scaleBufferY;

	drawCels(tunnelWindowCel);

	drawCels(tunnelBlob);
}

void partOutroInit()
{
	if (isOutroInit) return;

	tunnelCel = LoadCel("data/tunnel.cel", MEMTYPE_CEL);
	tunnelBlob = LoadCel("data/tunnel_blob.cel", MEMTYPE_CEL);

	tunnelWindowCel = CloneCel(tunnelCel, CLONECEL_CCB_ONLY);

	saveTunnelPattern();

	initMosaikEffect(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

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
}

void partOutroRun(int ticks, int dt)
{
	mosaikZoomScript(ticks>>2);

	prepareForMosaikEffect(mosaikZoom);

	animateTunnel(ticks);

	renderMosaikEffect(mosaikZoom);
}

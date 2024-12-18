#include "sprite_engine.h"
#include "system_graphics.h"
#include "tools.h"
#include "cel_helpers.h"


Sprite *newSprite(int width, int height, int bpp, int type, uint16 *pal, ubyte *bmp)
{
	uint16 *aman = pal;
	Sprite *spr = (Sprite*)AllocMem(sizeof(Sprite), MEMTYPE_ANY);

	spr->width = width;
	spr->height = height;

	spr->cel = CreateCel(width, height, bpp, type, bmp);
	spr->cel->ccb_SourcePtr = (void*)bmp;	// CreateCel is supposed to do that for you but fails for some pointer addresses, either a bug or something I miss

	spr->cel->ccb_Flags |= (CCB_ACSC | CCB_ALSC);
	if (type == CREATECEL_CODED) spr->cel->ccb_PLUTPtr = aman;

	spr->posX = spr->posY = 0;
	spr->angle = 0;
	spr->zoom = 256;

	return spr;
}

Sprite *newFeedbackSprite(int posX, int posY, int width, int height, int bufferIndex)
{
	Sprite *spr = newSprite(width, height, 16, CREATECEL_UNCODED, NULL, (ubyte*)getBackBuffer());

	setupWindowFeedbackCel(posX, posY, width, height, bufferIndex, spr->cel);

	return spr;
}

Sprite *loadSpriteCel(char *path)
{
	Sprite *spr = (Sprite*)AllocMem(sizeof(Sprite), MEMTYPE_ANY);

	spr->cel = LoadCel(path, MEMTYPE_ANY);
	spr->cel->ccb_Flags |= (CCB_ACSC | CCB_ALSC);
	
	spr->width = spr->cel->ccb_Width;
	spr->height = spr->cel->ccb_Height;

	spr->posX = spr->posY = 0;
	spr->angle = 0;
	spr->zoom = 256;

	return spr;
}

void setPalette(Sprite *spr, uint16* pal)
{
	spr->cel->ccb_PLUTPtr = pal;
}

void setSpriteAlpha(Sprite *spr, bool enable)
{
	if (enable)
		spr->cel->ccb_PIXC = 0x1F801F80;
	else
		spr->cel->ccb_PIXC = SOLID_CEL;
}

void setSpriteDottedDisplay(Sprite *spr, bool enable)
{
	if (enable)
		spr->cel->ccb_Flags |= CCB_MARIA;
	else
		spr->cel->ccb_Flags &= ~CCB_MARIA;
}


static void mapSprite(Sprite *spr)
{
	spr->cel->ccb_XPos = spr->posX << 16;
	spr->cel->ccb_YPos = spr->posY << 16;
}

static void hideSpriteIfZoomMinus(Sprite *spr)
{
	if (spr->zoom <= 0){
		spr->cel->ccb_Flags |= CCB_SKIP;
	} else {
		spr->cel->ccb_Flags &= ~CCB_SKIP;
	}
}

static void mapZoomSprite(Sprite *spr)
{
	if (spr->zoom < 256)
		hideSpriteIfZoomMinus(spr);

	spr->cel->ccb_XPos = (spr->posX - (((spr->width >> 1) * spr->zoom) >> 8)) << 16;
	spr->cel->ccb_YPos = (spr->posY - (((spr->height >> 1) * spr->zoom) >> 8)) << 16;

	spr->cel->ccb_HDX = spr->zoom<<12;
	spr->cel->ccb_VDY = spr->zoom<<8;
}

static void mapZoomRotateSprite(Sprite *spr)
{
	int hdx, hdy, vdx, vdy;

	if (spr->zoom < 256)
		hideSpriteIfZoomMinus(spr);

	//hdx = (spr->zoom * CosF16(spr->angle<<8)) >> 16;
	//hdy = (spr->zoom * -SinF16(spr->angle<<8)) >> 16;
	hdx = (spr->zoom * cosF16[(spr->angle>>8) & 255]) >> 16;
	hdy = (spr->zoom * -sinF16[(spr->angle>>8) & 255]) >> 16;
	vdx = -hdy;
	vdy = hdx;

	spr->cel->ccb_HDX = hdx << 12;
	spr->cel->ccb_HDY = hdy << 12;
	spr->cel->ccb_VDX = vdx << 8;
	spr->cel->ccb_VDY = vdy << 8;

	// sub to move to center of the sprite
	hdx *= (spr->width >> 1);
	hdy *= (spr->width >> 1);
	vdx *= (spr->height >> 1);
	vdy *= (spr->height >> 1);
	spr->cel->ccb_XPos = (spr->posX << 16) + ((- hdx - vdx) << 8);
	spr->cel->ccb_YPos = (spr->posY << 16) + ((- hdy - vdy) << 8);
}

void mapZoomSpriteToQuad(Sprite *spr, Quad *q)
{
	spr->cel->ccb_XPos = q->ulX << 16;
	spr->cel->ccb_YPos = q->ulY << 16;

	// Could be faster if they were power of two, but now it's a quick hack
	spr->cel->ccb_HDX = ((q->lrX - q->ulX + 1) << 20) / spr->width;
	spr->cel->ccb_HDY = 0;
	spr->cel->ccb_VDX = 0;
	spr->cel->ccb_VDY = ((q->lrY - q->ulY + 1) << 16) / spr->height;
}

void mapFeedbackSpriteToNewFramebufferArea(int ulX, int ulY, int lrX, int lrY, int bufferIndex, Sprite *spr)
{
	const int newWidth = (lrX-ulX+1) & ~1;
	const int newHeight = (lrY-ulY+1) & ~1;

	spr->width = newWidth;
	spr->height = newHeight;

	setupWindowFeedbackCel(ulX, ulY, newWidth, newHeight, bufferIndex, spr->cel);
}

void setSpritePosition(Sprite *spr, int px, int py)
{
	spr->posX = px;
	spr->posY = py;

	mapSprite(spr);
}

void setSpritePositionZoom(Sprite *spr, int px, int py, int zoom)
{
	spr->posX = px;
	spr->posY = py;
	spr->zoom = zoom;

	mapZoomSprite(spr);
}

void setSpritePositionZoomRotate(Sprite *spr, int px, int py, int zoom, int angle)
{
	spr->posX = px;
	spr->posY = py;
	spr->zoom = zoom;
	spr->angle = angle;

	mapZoomRotateSprite(spr);
}

void *getSpriteBitmapData(Sprite *spr)
{
	return spr->cel->ccb_SourcePtr;
}

void drawSprite(Sprite *spr)
{
	drawCels(spr->cel);
}

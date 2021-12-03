#include "core.h"

#include "part_slimecube.h"

#include "system_graphics.h"
#include "tools.h"
#include "input.h"

#include "mathutil.h"

#include "engine_main.h"
#include "engine_mesh.h"
#include "engine_texture.h"

#include "procgen_mesh.h"

#include "sprite_engine.h"
#include "fonts.h"


#define FRAME_SUB_X 4
#define FRAME_SUB_Y 3

typedef struct BufferRegionInfo
{
	int index;
	int posX, posY;
	int width, height;
}BufferRegionInfo;


static Mesh *draculMesh;
static Texture *draculTex;
static Sprite **feedbackLineSpr;

CCB *eraseCel;
CCB *skyCel2;

const int spriteLines = (SCREEN_HEIGHT / FRAME_SUB_Y) / 2;
const int screenRegionsNum = FRAME_SUB_X * FRAME_SUB_Y;

static int regIter = 0;
static int totalRegions;

static TextSpritesList *dentroText[16];


static void scaleLineSprites(int offX, int offY, int zoom)
{
	int i;
	const int sprWidth = SCREEN_WIDTH / FRAME_SUB_X;
	const int sprHeight = 2;
	const int lineOffY = (sprHeight * zoom) >> 8;
	const int totalHeight = sprHeight * spriteLines;
	const int ulX = ((SCREEN_WIDTH - ((zoom * sprWidth) >> 8)) >> 1) + offX;
	int ulY = ((SCREEN_HEIGHT - ((zoom * totalHeight) >> 8)) >> 1) + offY;

	for (i=0; i<spriteLines; ++i) {
		mapZoomSpriteToQuad(feedbackLineSpr[i], ulX, ulY, ulX + ((zoom * sprWidth)>>8), ulY + lineOffY);
		ulY += lineOffY;
	}
}

static void initFeedbackLineSprites()
{
	int i;
	feedbackLineSpr = (Sprite**)AllocMem(sizeof(Sprite*) * spriteLines, MEMTYPE_ANY);

	for (i=0; i<spriteLines; ++i) {
		feedbackLineSpr[i] = newFeedbackSprite(0, 0, SCREEN_WIDTH / FRAME_SUB_X, 2, 0);
		feedbackLineSpr[i]->cel->ccb_Flags &= ~CCB_BGND;
		feedbackLineSpr[i]->cel->ccb_Flags |= CCB_NOBLK;
		if (i>0) LinkCel(feedbackLineSpr[i-1]->cel, feedbackLineSpr[i]->cel);
	}

	//scaleLineSprites(0,0,256);
}

void partSlimecubeInit()
{
	int i,j;

	initFeedbackLineSprites();

	totalRegions = screenRegionsNum * getNumOffscreenBuffers();

	draculTex = loadTexture("data/draculin64.cel");
	draculMesh = initGenMesh(256, draculTex, MESH_OPTIONS_DEFAULT, MESH_CUBE, NULL);

	eraseCel = CreateBackdropCel(SCREEN_WIDTH / FRAME_SUB_X, SCREEN_HEIGHT / FRAME_SUB_Y, 0, 100);
	eraseCel->ccb_Flags |= CCB_BGND;
	eraseCel->ccb_Flags |= CCB_NOBLK;

	skyCel2 = LoadCel("data/sky2.cel", MEMTYPE_CEL);
	skyCel2->ccb_HDX = SCREEN_WIDTH << 20;

	dentroText[0] = generateTextCCBs("  THIS IS 3DENTRO");
	dentroText[1] = generateTextCCBs("   LITTLE TEXTRO");
	dentroText[2] = generateTextCCBs("    TO ANNOUNCE");
	dentroText[3] = generateTextCCBs("  3DO IS ALIVE!!!");

	dentroText[4] = generateTextCCBs("   3DO COMMUNITY");
	dentroText[5] = generateTextCCBs("    IS GROWING");
	dentroText[6] = generateTextCCBs("   VISIT DISCORD");
	dentroText[7] = generateTextCCBs("   3DO M1(OPERA)");

	dentroText[8] = generateTextCCBs("  COME VISIT US!");
	dentroText[9] = generateTextCCBs("   FOR DEV TIPS");
	dentroText[10] = generateTextCCBs("      CHECK");
	dentroText[11] = generateTextCCBs("    3DODEV.COM");

	dentroText[12] = generateTextCCBs(" WE NEED MORE 3DO");
	dentroText[13] = generateTextCCBs("   HOMEBREW AND");
	dentroText[14] = generateTextCCBs("     DEMOSCENE ");
	dentroText[15] = generateTextCCBs("AND REAL 3DO MEMES");

	for (i=0; i<16; ++i) {
		int py = i & 3;
		setFontsAnimPos(FONTPOS_LINEAR, dentroText[i], 16, 24 + py * 64, 0, 0, true);
		setFontsAnimPos(FONTPOS_LINEAR, dentroText[i], 16, 24 + py * 64, 0, 0, false);
	}
}

static void renderDraculCube(int t)
{
	setMeshPosition(draculMesh, 0, 0, 1408);	//4x3
	//setMeshPosition(draculMesh, 0, 0, 960);	// 2x2
	setMeshRotation(draculMesh, t, t<<1, t>>1);
	renderMesh(draculMesh);
}

static BufferRegionInfo *getBufferRegionInfoFromNum(int num)
{
	static BufferRegionInfo regionInfo;

	const int regX = num % FRAME_SUB_X;
	const int regY = (num / FRAME_SUB_X) % FRAME_SUB_Y;

	regionInfo.index = num / screenRegionsNum;
	regionInfo.width = SCREEN_WIDTH / FRAME_SUB_X;
	regionInfo.height = SCREEN_HEIGHT / FRAME_SUB_Y;
	regionInfo.posX = regX * regionInfo.width;
	regionInfo.posY = regY * regionInfo.height;

	return &regionInfo;
}

static int getBackInTimeIter(int presentIter, int line, int t)
{
	const int waveAmp = totalRegions >> 2;
	const int wave = (((SinF16((3*line+2*t) << 16) + 65536) * waveAmp) >> 16) + (((SinF16((2*line-t) << 16) + 65536) * waveAmp) >> 16);

	int pastIter = presentIter - wave;
	if (pastIter < 0) pastIter += totalRegions;

	return pastIter;
}

static void drawPage(int page, int t, int pageOffX)
{
	int i;
	int textIndex = (page << 2);

	for (i=0; i<4; ++i) {
		const int aman = textIndex + i;
/*		for (j=0; j<dentroText[aman]->numChars; ++j) {
			FontPos *fp = &dentroText[aman]->endPos[j];
			dentroText[aman]->endPos[j].posX = dentroText[aman]->startPos[j].posX;// + (SinF16((8*i+16*j+t) << 12) >> 14);
			dentroText[aman]->endPos[j].posY = dentroText[aman]->startPos[j].posY;// + (SinF16((12*i+24*j+t) << 12) >> 14);
			setSpritePositionZoomRotate(dentroText[aman]->chars[i], fp->posX, fp->posY, fp->zoom, fp->angle);
		}*/
		
		//updateFontAnimPos(dentroText[aman], 0);
		waveFontAnimPos(dentroText[aman], 8192, 4096, 512, 640, 15, 14, 64*i + 24*t, pageOffX);
		
		drawSprite(dentroText[aman]->chars[0]);
	}
}

static void slimecubeAnimScript(int t)
{
	static int posX = 512;
	static int posY = 0;
	static int zoom = 256;

	if (t < 6000) {
		posX -= 2;
		if (posX < 0) posX = 0;
	} else {
		posX = SinF16((t-6000)<<13) >> 11;
		zoom += 4;
		if (zoom > 512) zoom = 512;
	}
	posY = SinF16(t<<14) >> 12;

	scaleLineSprites(posX, posY, zoom);
}

static void dentroTextAnimScript(int t)
{
	int i;
	const int tOffRange = 2000;
	const int tRange[8] = {1000,12000, 13000,25000, 26000,38000, 39000,151000};

	for (i=0; i<8; i+=2) {
		const int t0 = tRange[i];
		const int t1 = tRange[i+1];

		int pageOffX = 0;
		if (t>t0 && t<t0+tOffRange) {
			pageOffX = (t0+tOffRange - t) >> 2;
		} else if (t>t1-tOffRange && t<t1) {
			pageOffX = (t1-tOffRange - t) >> 2;
		}
		CLAMP(pageOffX, -512, 512);

		if (t > t0 && t < t1) {
			drawPage(i>>1, t-t0, pageOffX);
		}
	}

}

void partSlimecubeRun(int ticks)
{
	int i;
	const int time = ticks >> 4;

	BufferRegionInfo *regionInfo = getBufferRegionInfoFromNum(regIter);

	slimecubeAnimScript(ticks);

	switchRenderToBuffer(true);
	setRenderBuffer(regionInfo->index);
	setScreenRegion(regionInfo->posX, regionInfo->posY, regionInfo->width, regionInfo->height);

	eraseCel->ccb_XPos = regionInfo->posX << 16;
	eraseCel->ccb_YPos = regionInfo->posY << 16;
	drawCels(eraseCel);

	renderDraculCube(time);

	for (i=0; i<spriteLines; ++i) {
		int posX, posY;
		regionInfo = getBufferRegionInfoFromNum(getBackInTimeIter(regIter, i, time));

		posX = regionInfo->posX;
		posY = regionInfo->posY + (i<<1);
		mapFeedbackSpriteToNewFramebufferArea(posX, posY, posX + regionInfo->width, posY + 2, regionInfo->index, feedbackLineSpr[i]);

		feedbackLineSpr[i]->cel->ccb_Flags &= ~CCB_BGND;
		feedbackLineSpr[i]->cel->ccb_Flags |= CCB_NOBLK;
	}

	switchRenderToBuffer(false);

	drawCels(skyCel2);

	drawSprite(feedbackLineSpr[0]);
	
	dentroTextAnimScript(ticks);

	//drawBorderEdges(regionPosX, regionPosY, regionWidth, regionHeight);

	regIter = (regIter + 1) % totalRegions;
}

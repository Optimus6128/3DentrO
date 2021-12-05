#include "core.h"
#include "tools.h"

#include "part_intro.h"
#include "part_credits.h"
#include "part_slimecube.h"
#include "part_spritesGecko.h"

#include "fonts.h"
#include "sound.h"

#include "tools.h"

enum { PART_INTRO, PART_CREDITS, PART_SLIMECUBE, PART_SPRITES_GECKO, PARTS_NUM };

static void(*partInitFunc[PARTS_NUM])() = { partIntroInit, partCreditsInit, partSlimecubeInit, partSpritesGeckoInit };
static void(*partRunFunc[PARTS_NUM])(int,int) = { partIntroRun, partCreditsRun, partSlimecubeRun, partSpritesGeckoRun };

int partIndex = PART_INTRO;

int startPartTicks = -1;
static void(*currentPartRunFunc)(int,int);

static void switchPart(int newPart) {

	partIndex = newPart;
	partInitFunc[partIndex]();
	currentPartRunFunc = partRunFunc[partIndex];

	startPartTicks = -1;
}

static void initParts()
{
	initFonts();

	partInitFunc[PART_INTRO]();
	partInitFunc[PART_CREDITS]();
	partInitFunc[PART_SLIMECUBE]();
	//partInitFunc[PART_SPRITES_GECKO]();

	switchPart(partIndex);

	startMusic("data/music.aiff");
}

static void runDemo()
{
	static int dt = 0;
	int t,t0,t1;

	if (startPartTicks==-1) {
		startPartTicks = getTicks();
	}
	t = getTicks() - startPartTicks;

	t0 = getTicks();
	currentPartRunFunc(t,dt);
	t1 = getTicks();
	dt = t1-t0;

	if (partIndex==PART_INTRO && t > 25000) {
		switchPart(PART_CREDITS);
	} else if (partIndex==PART_CREDITS && t > 35000) {
		switchPart(PART_SLIMECUBE);
	}
}

int main()
{
	uint32 flags = CORE_VRAM_BUFFERS(2) | CORE_OFFSCREEN_BUFFERS(4);
	flags |= (CORE_SHOW_FPS | /*CORE_SHOW_MEM | */ CORE_DEFAULT_INPUT);

	coreInit(initParts, flags);
	coreRun(runDemo);
}

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
static void(*partRunFunc[PARTS_NUM])(int) = { partIntroRun, partCreditsRun, partSlimecubeRun, partSpritesGeckoRun };

int partIndex = PART_INTRO;

int startPartTicks = -1;
static void(*currentPartRunFunc)(int);

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
	int dt;

	if (startPartTicks==-1) {
		startPartTicks = getTicks();
	}
	dt = getTicks() - startPartTicks;

	currentPartRunFunc(dt);

	if (partIndex==PART_INTRO && dt > 25000) {
		switchPart(PART_CREDITS);
	} else if (partIndex==PART_CREDITS && dt > 35000) {
		switchPart(PART_SLIMECUBE);
	}
}

int main()
{
	uint32 flags = CORE_VRAM_BUFFERS(2) | CORE_OFFSCREEN_BUFFERS(4);
	//flags |= (CORE_SHOW_FPS | /*CORE_SHOW_MEM | */ CORE_DEFAULT_INPUT);

	coreInit(initParts, flags);
	coreRun(runDemo);
}

#include "core.h"
#include "tools.h"

#include "part_spritesGecko.h"
#include "part_intro.h"

#include "sound.h"

enum { PART_INTRO, PART_SPRITES_GECKO, PARTS_NUM };

static void(*partInitFunc[PARTS_NUM])() = { partIntroInit, partSpritesGeckoInit };
static void(*partRunFunc[PARTS_NUM])() = { partIntroRun, partSpritesGeckoRun };

int partIndex = PART_INTRO;

static void initParts()
{
	partInitFunc[partIndex]();

	//startMusic("data/music.aiff");
}

static void runDemo()
{
	partRunFunc[partIndex]();
}

int main()
{
	coreInit(initParts, CORE_DEFAULT);
	coreRun(runDemo);
}

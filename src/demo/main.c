#include "core.h"
#include "tools.h"

#include "part_spritesGecko.h"
#include "part_intro.h"
#include "part_credits.h"

#include "fonts.h"
#include "sound.h"

enum { PART_INTRO, PART_CREDITS, PART_SPRITES_GECKO, PARTS_NUM };

static void(*partInitFunc[PARTS_NUM])() = { partIntroInit, partCreditsInit, partSpritesGeckoInit };
static void(*partRunFunc[PARTS_NUM])() = { partIntroRun, partCreditsRun, partSpritesGeckoRun };

int partIndex = PART_INTRO;

static void initParts()
{
	initFonts();

	partInitFunc[partIndex]();

	//startMusic("data/music.aiff");
}

static void runDemo()
{
	partRunFunc[partIndex]();
}

int main()
{
	uint32 flags = CORE_VRAM_BUFFERS(2) | CORE_OFFSCREEN_BUFFERS(4);
	flags |= (CORE_SHOW_FPS | CORE_DEFAULT_INPUT);

	coreInit(initParts, flags);
	coreRun(runDemo);
}

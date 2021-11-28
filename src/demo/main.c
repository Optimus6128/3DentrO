#include "core.h"
#include "tools.h"

#include "part_spritesGecko.h"

enum { PART_SPRITES_GECKO, PARTS_NUM };

static void(*partInitFunc[PARTS_NUM])() = { partSpritesGeckoInit };
static void(*partRunFunc[PARTS_NUM])() = { partSpritesGeckoRun };

int main()
{
	const int partIndex = PART_SPRITES_GECKO;

	coreInit(partInitFunc[partIndex], CORE_DEFAULT);
	coreRun(partRunFunc[partIndex]);
}

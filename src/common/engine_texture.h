#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#define TEXTURE_TYPE_STATIC			0
#define TEXTURE_TYPE_DYNAMIC		(1 << 0)
#define TEXTURE_TYPE_FEEDBACK		(1 << 1)
#define TEXTURE_TYPE_PALLETIZED		(1 << 2)
#define TEXTURE_TYPE_PACKED			(1 << 3)
#define TEXTURE_TYPE_BLEND			(1 << 4)
#define TEXTURE_TYPE_TWOSIDED		(1 << 5)

typedef struct Texture
{
	int type;
	int width, height;
	int bpp;

	// Texture bitmap and palette pointer (needed for 8bpp CODED or less)
	ubyte *bitmap;
	uint16 *pal;

	// Necessary extension for feedback textures
	int posX, posY;
	int bufferIndex;
}Texture;


void setupTexture(int width, int height, int bpp, int type, ubyte *bmp, uint16 *pal, ubyte numPals, Texture *tex);
Texture *initTextures(int width, int height, int bpp, int type, ubyte *bmp, uint16 *pal, ubyte numPals, ubyte numTextures);
Texture *initTexture(int width, int height, int bpp, int type, ubyte *bmp, uint16 *pal, ubyte numPals);
Texture *loadTexture(char *path);
Texture *initFeedbackTexture(int posX, int posY, int width, int height, int bufferIndex);
void copyTexture(Texture *src, Texture *dst);

#endif

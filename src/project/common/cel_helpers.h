#ifndef CEL_HELPERS_H
#define CEL_HELPERS_H

#include "graphics.h"

void setupWindowFeedbackCel(int posX, int posY, int width, int height, int bufferIndex, CCB *cel);

int getCelBpp(CCB *cel);
int getCelDataSizeInBytes(CCB *cel);

void initMosaikEffect(int bufferWidth, int bufferHeight);
void prepareForMosaikEffect(int mosaikZoom);
void renderMosaikEffect(int mosaikZoom);

#endif

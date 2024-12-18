#ifndef ENGINE_MAIN_H
#define ENGINE_MAIN_H

#include "engine_mesh.h"

#define MAX_VERTICES_NUM 16
#define MAX_NORMALS_NUM 8

#define PROJ_SHR 8
#define REC_FPSHR 20
#define NUM_REC_Z 4096

#define NORMAL_SHIFT 8

void initEngine(void);

void renderMesh(Mesh *ms);
void renderMeshSoft(Mesh *ms);

void createRotationMatrixValues(int rotX, int rotY, int rotZ, int *rotVecs);	// I will hide this again after I fix the volume

void setScreenRegion(int posX, int posY, int width, int height);

#endif

#include "core.h"

#include "tools.h"
#include "system_graphics.h"

#include "engine_main.h"
#include "procgen_mesh.h"
#include "mathutil.h"


Mesh *initGenMesh(int size, Texture *tex, int optionsFlags, int meshgenId, void *params)
{
	int i, x, y;
	int xp, yp;
	int dx, dy;
	
	Mesh *ms;
	
	const int s = size / 2;
	const int n = 1 << NORMAL_SHIFT;

	switch(meshgenId)
	{
		default:
		case MESH_PLANE:
		{
			ms = initMesh(4, 1);

			ms->vrtx[0].x = -size/2; ms->vrtx[0].y = -size/2; ms->vrtx[0].z = 0;
			ms->vrtx[1].x = size/2; ms->vrtx[1].y = -size/2; ms->vrtx[1].z = 0;
			ms->vrtx[2].x = size/2; ms->vrtx[2].y = size/2; ms->vrtx[2].z = 0;
			ms->vrtx[3].x = -size/2; ms->vrtx[3].y = size/2; ms->vrtx[3].z = 0;

			for (i=0; i<4; i++)
				ms->index[i] = i;

			ms->quad[0].textureId = 0;
			ms->quad[0].palId = 0;
		}
		break;

		case MESH_CUBE:
		{
			ms = initMesh(8, 6);

			setVertex(&ms->vrtx[0], -s, -s, -s);
			setVertex(&ms->vrtx[1], s, -s, -s);
			setVertex(&ms->vrtx[2], s, s, -s);
			setVertex(&ms->vrtx[3], -s, s, -s);
			setVertex(&ms->vrtx[4], s, -s, s);
			setVertex(&ms->vrtx[5], -s, -s, s);
			setVertex(&ms->vrtx[6], -s, s, s);
			setVertex(&ms->vrtx[7], s, s, s);

			ms->index[0] = 0; ms->index[1] = 1; ms->index[2] = 2; ms->index[3] = 3;
			ms->index[4] = 1; ms->index[5] = 4; ms->index[6] = 7; ms->index[7] = 2;
			ms->index[8] = 4; ms->index[9] = 5; ms->index[10] = 6; ms->index[11] = 7;
			ms->index[12] = 5; ms->index[13] = 0; ms->index[14] = 3; ms->index[15] = 6;
			ms->index[16] = 3; ms->index[17] = 2; ms->index[18] = 7; ms->index[19] = 6;
			ms->index[20] = 5; ms->index[21] = 4; ms->index[22] = 1; ms->index[23] = 0;

			setVertex(&ms->normal[0], 0,0,-n);
			setVertex(&ms->normal[1], n,0,0);
			setVertex(&ms->normal[2], 0,0,n);
			setVertex(&ms->normal[3], -n,0,0);
			setVertex(&ms->normal[4], 0,n,0);
			setVertex(&ms->normal[5], 0,-n,0);

			for (i=0; i<ms->quadsNum; i++) {
				ms->quad[i].textureId = 0;
				ms->quad[i].palId = 0;
			}
		}
		break;

		case MESH_PYRAMID1:
		{
			ms = initMesh(5, 5);

			ms->vrtx[0].x = -size/2; ms->vrtx[0].y = -size/2; ms->vrtx[0].z = -size/2;
			ms->vrtx[1].x = size/2; ms->vrtx[1].y = -size/2; ms->vrtx[1].z = -size/2;
			ms->vrtx[2].x = size/2; ms->vrtx[2].y = -size/2; ms->vrtx[2].z = size/2;
			ms->vrtx[3].x = -size/2; ms->vrtx[3].y = -size/2; ms->vrtx[3].z = size/2;
			ms->vrtx[4].x = 0; ms->vrtx[4].y = size/2; ms->vrtx[4].z = 0;


			ms->index[0] = 3; ms->index[1] = 2; ms->index[2] = 1; ms->index[3] = 0;
			ms->index[4] = 0; ms->index[5] = 1; ms->index[6] = 4; ms->index[7] = 4;
			ms->index[8] = 1; ms->index[9] = 2; ms->index[10] = 4; ms->index[11] = 4;
			ms->index[12] = 2; ms->index[13] = 3; ms->index[14] = 4; ms->index[15] = 4;
			ms->index[16] = 3; ms->index[17] = 0; ms->index[18] = 4; ms->index[19] = 4;

			for (i=0; i<ms->quadsNum; i++) {
				ms->quad[i].textureId = 0;
				ms->quad[i].palId = 0;
			}
		}
		break;

		case MESH_PYRAMID2:
		{
			ms = initMesh(9, 5);

			ms->vrtx[0].x = -size/2; ms->vrtx[0].y = -size/2; ms->vrtx[0].z = -size/2;
			ms->vrtx[1].x = size/2; ms->vrtx[1].y = -size/2; ms->vrtx[1].z = -size/2;
			ms->vrtx[2].x = size/2; ms->vrtx[2].y = -size/2; ms->vrtx[2].z = size/2;
			ms->vrtx[3].x = -size/2; ms->vrtx[3].y = -size/2; ms->vrtx[3].z = size/2;
			
			ms->vrtx[4].x = size; ms->vrtx[4].y = size/2; ms->vrtx[4].z = 0;
			ms->vrtx[5].x = 0; ms->vrtx[5].y = size/2; ms->vrtx[5].z = size;
			ms->vrtx[6].x = -size; ms->vrtx[6].y = size/2; ms->vrtx[6].z = 0;
			ms->vrtx[7].x = 0; ms->vrtx[7].y = size/2; ms->vrtx[7].z = -size;
			
			ms->vrtx[8].x = 0; ms->vrtx[8].y = size/2; ms->vrtx[8].z = 0;

			ms->index[0] = 3; ms->index[1] = 2; ms->index[2] = 1; ms->index[3] = 0;
			ms->index[4] = 0; ms->index[5] = 1; ms->index[6] = 4; ms->index[7] = 8;
			ms->index[8] = 1; ms->index[9] = 2; ms->index[10] = 5; ms->index[11] = 8;
			ms->index[12] = 2; ms->index[13] = 3; ms->index[14] = 6; ms->index[15] = 8;
			ms->index[16] = 3; ms->index[17] = 0; ms->index[18] = 7; ms->index[19] = 8;

			for (i=0; i<ms->quadsNum; i++) {
				if (i==0) {
					ms->quad[i].textureId = 0;
					ms->quad[i].palId = 0;
				} else {
					ms->quad[i].textureId = 1;
					ms->quad[i].palId = 1;
				}
			}
		}
		break;

		case MESH_PYRAMID3:
		{
			ms = initMesh(5, 5);

			ms->vrtx[0].x = -size/2; ms->vrtx[0].y = -size/2; ms->vrtx[0].z = -size/2;
			ms->vrtx[1].x = size/2; ms->vrtx[1].y = -size/2; ms->vrtx[1].z = -size/2;
			ms->vrtx[2].x = size/2; ms->vrtx[2].y = -size/2; ms->vrtx[2].z = size/2;
			ms->vrtx[3].x = -size/2; ms->vrtx[3].y = -size/2; ms->vrtx[3].z = size/2;
			ms->vrtx[4].x = 0; ms->vrtx[4].y = size/2; ms->vrtx[4].z = 0;


			ms->index[0] = 3; ms->index[1] = 2; ms->index[2] = 1; ms->index[3] = 0;
			ms->index[4] = 0; ms->index[5] = 1; ms->index[6] = 4; ms->index[7] = 4;
			ms->index[8] = 1; ms->index[9] = 2; ms->index[10] = 4; ms->index[11] = 4;
			ms->index[12] = 2; ms->index[13] = 3; ms->index[14] = 4; ms->index[15] = 4;
			ms->index[16] = 3; ms->index[17] = 0; ms->index[18] = 4; ms->index[19] = 4;

			for (i=0; i<ms->quadsNum; i++) {
				if (i==0) {
					ms->quad[i].textureId = 0;
				} else {
					ms->quad[i].textureId = 1;
				}
				ms->quad[i].palId = 0;
			}
		}
		break;

		case MESH_GRID:
		{
			const int divisions = *((int*)params);
			const int vrtxNum = (divisions + 1) * (divisions + 1);
			const int quadsNum = divisions * divisions;

			ms = initMesh(vrtxNum, quadsNum);

			dx = size / divisions;
			dy = size / divisions;

			i = 0;
			yp = -size / 2;
			for (y=0; y<=divisions; y++)
			{
				xp = -size / 2;
				for (x=0; x<=divisions; x++)
				{
					ms->vrtx[i].x = xp; ms->vrtx[i].z = -yp; ms->vrtx[i].y = getRand(0, 255);
					xp += dx;
					i++;
				}
				yp += dy;
			}

			i = 0;
			for (y=0; y<divisions; y++)
			{
				for (x=0; x<divisions; x++)
				{
					ms->index[i+3] = x + y * (divisions + 1);
					ms->index[i+2] = x + 1 + y * (divisions + 1);
					ms->index[i+1] = x + 1 + (y + 1) * (divisions + 1);
					ms->index[i] = x + (y + 1) * (divisions + 1);
					i+=4;
				}
			}

			for (i=0; i<ms->quadsNum; i++) {
				ms->quad[i].textureId = 0;
				ms->quad[i].palId = 0;
			}
		}
		break;
	}

	ms->tex = tex;
	ms->useFastMapCel = (optionsFlags & MESH_OPTION_FAST_MAPCEL);
	ms->useCPUccwTest = (optionsFlags & MESH_OPTION_CPU_CCW_TEST);

	prepareCelList(ms);

	return ms;
}

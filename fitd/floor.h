#ifndef _FLOOR_H_
#define _FLOOR_H_
#include "types.h"
#include "room.h"
extern std::vector<cameraDataStruct> g_currentFloorCameraData;
extern u32 g_currentFloorRoomRawDataSize;
extern u32 g_currentFloorNumCamera;

void loadFloor(int floorNumber);
void loadFloorDataReady(int floorNumber);
#endif


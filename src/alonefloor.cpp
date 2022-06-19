#include "alonefloor.h"

#include <fstream>
#include <sstream>
#include <QGenericMatrix>
#include <QMessageBox>
#include <ctime>
#include "version.h"

extern const s16 cosTable[];

extern u32 g_currentFloorRoomRawDataSize;
extern u32 g_currentFloorCameraRawDataSize;
extern u32 g_currentFloorNumCamera;
extern std::vector<cameraDataStruct> g_currentFloorCameraData;

u32 AloneRoom::computeSize()
{
    u32 sz=0;
    sz+=2+2;//hardcol and scezone relative offset
    sz+=sizeof(worldX)+sizeof(worldY)+sizeof(worldZ)+sizeof(numCameraInRoom);
    sz+=numCameraInRoom*2;//camIdx
    sz+=2;//nb hardcol
    sz+=hardCols.size()*sizeof(hardColStruct);
    sz+=2;//nb scezone
    sz+=sceZones.size()*sizeof(sceZoneStruct);
    return sz;
}

AloneFloor::AloneFloor():roomDataTable(0),mRooms(0),mCams(0)
{

}

AloneFloor::~AloneFloor()
{
    if (roomDataTable) delete roomDataTable;
}

//TODO: update with floor::loadFloor
bool AloneFloor::load(AloneFile *rooms,AloneFile *cams)
{
    mRooms=rooms;
    mCams=cams;

    g_gameId=AITD1; //for now...
    g_currentFloorRoomRawData=rooms->mDecomprData;
    g_currentFloorCameraRawData=cams->mDecomprData;
    printf("DATA: %x %x %x %x\n",g_currentFloorRoomRawData[0],g_currentFloorRoomRawData[1],g_currentFloorRoomRawData[2],g_currentFloorRoomRawData[3]);
    printf("VAL: %u\n",*((u32*)g_currentFloorRoomRawData));

    g_currentFloorRoomRawDataSize=rooms->mInfo.uncompressedSize;
    g_currentFloorCameraRawDataSize=cams->mInfo.uncompressedSize;
    u32 i;

    if(roomDataTable)
    {
      free(roomDataTable);
      roomDataTable = NULL;
    }

    expectedNumberOfRoom = 0;//TODO: del
    expectedNumberOfRoom = getNumberOfRoom();
    /*if(g_gameId >= AITD3)
    {
      printf("ERROR! make something for g_gameId >= AITD3!\n");
      return false;
    }
    u32 numMax = (((READ_LE_U32(g_currentFloorRoomRawData))/4));
    for(i=0;i<numMax;i++)
    {
      if(g_currentFloorRoomRawDataSize >= READ_LE_U32(g_currentFloorRoomRawData + i * 4))
        expectedNumberOfRoom++;
      else
        break;
    }*/

    for(i=0;i<expectedNumberOfRoom;i++)
    {
      u32 j;
      char* roomData;
      char* hardColData;
      char* sceZoneData;
      roomDataStruct* currentRoomDataPtr;

      if(roomDataTable)
      {
        roomDataTable = (roomDataStruct*)realloc(roomDataTable,sizeof(roomDataStruct)*(i+1));
      }
      else
      {
        roomDataTable = (roomDataStruct*)malloc(sizeof(roomDataStruct));
      }

      if(g_gameId >= AITD3)
      {
        roomData = rooms->mDecomprData;
      }
      else
      {
        roomData = (g_currentFloorRoomRawData + READ_LE_U32(g_currentFloorRoomRawData + i * 4));
      }
      currentRoomDataPtr = &roomDataTable[i];

      currentRoomDataPtr->worldX = READ_LE_S16(roomData+4);
      currentRoomDataPtr->worldY = READ_LE_S16(roomData+6);
      currentRoomDataPtr->worldZ = READ_LE_S16(roomData+8);

      currentRoomDataPtr->numCameraInRoom = READ_LE_U16(roomData+0xA);

      currentRoomDataPtr->cameraIdxTable = (u16*)malloc(currentRoomDataPtr->numCameraInRoom*sizeof(s16));

      for(j=0;j<currentRoomDataPtr->numCameraInRoom;j++)
      {
        currentRoomDataPtr->cameraIdxTable[j] = READ_LE_U16(roomData+0xC+2*j);
      }

      // hard col read

      hardColData = roomData + READ_LE_U16(roomData);
      currentRoomDataPtr->numHardCol = READ_LE_U16(hardColData);
      printf("numHardCol: %u\n",currentRoomDataPtr->numHardCol);
      hardColData+=2;

      if(currentRoomDataPtr->numHardCol)
      {
        currentRoomDataPtr->hardColTable = (hardColStruct*)malloc(sizeof(hardColStruct)*currentRoomDataPtr->numHardCol);

        for(j=0;j<currentRoomDataPtr->numHardCol;j++)
        {
          ZVStruct* zvData;

          zvData = &currentRoomDataPtr->hardColTable[j].zv;

          zvData->ZVX1 = READ_LE_S16(hardColData+0x00);
          zvData->ZVX2 = READ_LE_S16(hardColData+0x02);
          zvData->ZVY1 = READ_LE_S16(hardColData+0x04);
          zvData->ZVY2 = READ_LE_S16(hardColData+0x06);
          zvData->ZVZ1 = READ_LE_S16(hardColData+0x08);
          zvData->ZVZ2 = READ_LE_S16(hardColData+0x0A);

          currentRoomDataPtr->hardColTable[j].parameter = READ_LE_U16(hardColData+0x0C);
          currentRoomDataPtr->hardColTable[j].type = READ_LE_U16(hardColData+0x0E);

          hardColData+=0x10;
        }
      }
      else
      {
        currentRoomDataPtr->hardColTable = NULL;
      }

      // sce zone read

      sceZoneData = roomData + READ_LE_U16(roomData+2);
      currentRoomDataPtr->numSceZone = READ_LE_U16(sceZoneData);
      printf("numSceZone: %u\n",currentRoomDataPtr->numSceZone);
      sceZoneData+=2;

      if(currentRoomDataPtr->numSceZone)
      {
        currentRoomDataPtr->sceZoneTable = (sceZoneStruct*)malloc(sizeof(sceZoneStruct)*currentRoomDataPtr->numSceZone);

        for(j=0;j<currentRoomDataPtr->numSceZone;j++)
        {
          ZVStruct* zvData;

          zvData = &currentRoomDataPtr->sceZoneTable[j].zv;

          zvData->ZVX1 = READ_LE_S16(sceZoneData+0x00);
          zvData->ZVX2 = READ_LE_S16(sceZoneData+0x02);
          zvData->ZVY1 = READ_LE_S16(sceZoneData+0x04);
          zvData->ZVY2 = READ_LE_S16(sceZoneData+0x06);
          zvData->ZVZ1 = READ_LE_S16(sceZoneData+0x08);
          zvData->ZVZ2 = READ_LE_S16(sceZoneData+0x0A);

          currentRoomDataPtr->sceZoneTable[j].parameter = READ_LE_U16(sceZoneData+0x0C);
          currentRoomDataPtr->sceZoneTable[j].type = READ_LE_U16(sceZoneData+0x0E);

          sceZoneData+=0x10;
        }
      }
      else
      {
        currentRoomDataPtr->sceZoneTable = NULL;
      }
    }
    ///////////////////////////////////

    /////////////////////////////////////////////////
    // camera stuff

    if(g_gameId >= AITD3)
    {
      /*char buffer[256];

      if(g_gameId == AITD3)
      {
        sprintf(buffer,"CAM%02d",floorNumber);
      }
      else
      {
        sprintf(buffer,"CAMSAL%02d",floorNumber);
      }

      expectedNumberOfCamera = PAK_getNumFiles(buffer);*/
      printf("ERROR! make something for g_gameId >= AITD3!\n");
      return false;
    }
    else
    {
      int maxExpectedNumberOfCamera = ((READ_LE_U32(g_currentFloorCameraRawData))/4);
      expectedNumberOfCamera = 0;
      int minOffset = 0;
      for(int i=0; i<maxExpectedNumberOfCamera; i++)
      {
          int offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
          if(offset > minOffset)
          {
              minOffset = offset;
              expectedNumberOfCamera++;
          }
          else
          {
              break;
          }
      }
    }
    printf("expectedNumberOfCamera: %u\n",expectedNumberOfCamera);

    g_currentFloorCameraData.clear();
    g_currentFloorCameraData.resize(expectedNumberOfCamera);

    for(i=0;i<expectedNumberOfCamera;i++)
    {
      printf("Camera %d\n",i);
      int k;
      unsigned int offset;
      unsigned char* currentCameraData;

      if(g_gameId >= AITD3)
      {
        /*char buffer[256];

        if(g_gameId == AITD3)
        {
          sprintf(buffer,"CAM%02d",floorNumber);
        }
        else
        {
          sprintf(buffer,"CAMSAL%02d",floorNumber);
        }

        offset = 0;
        g_currentFloorCameraRawDataSize = 1;
        currentCameraData = (unsigned char*)loadPakSafe(buffer,i);*/
        printf("ERROR! make something for g_gameId >= AITD3!\n");
      }
      else
      {
        offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
      }

      // load cameras
      if(offset<g_currentFloorCameraRawDataSize)
      {
        unsigned char* backupDataPtr;

        if(g_gameId<AITD3)
        {
          currentCameraData = (unsigned char*)(g_currentFloorCameraRawData + READ_LE_U32(g_currentFloorCameraRawData + i * 4));
        }

        backupDataPtr = currentCameraData;

        g_currentFloorCameraData[i].alpha = READ_LE_U16(currentCameraData+0x00);
        g_currentFloorCameraData[i].beta  = READ_LE_U16(currentCameraData+0x02);
        g_currentFloorCameraData[i].gamma = READ_LE_U16(currentCameraData+0x04);

        g_currentFloorCameraData[i].x = READ_LE_U16(currentCameraData+0x06);
        g_currentFloorCameraData[i].y = READ_LE_U16(currentCameraData+0x08);
        g_currentFloorCameraData[i].z = READ_LE_U16(currentCameraData+0x0A);

        g_currentFloorCameraData[i].focal1 = READ_LE_U16(currentCameraData+0x0C);
        g_currentFloorCameraData[i].focal2 = READ_LE_U16(currentCameraData+0x0E);
        g_currentFloorCameraData[i].focal3 = READ_LE_U16(currentCameraData+0x10);

        //printf("CAM: %hx %hx %hx %hx %hx %hx %hx %hx %hx\n",g_currentFloorCameraData[i].alpha,g_currentFloorCameraData[i].beta,
        //       g_currentFloorCameraData[i].gamma,g_currentFloorCameraData[i].x,g_currentFloorCameraData[i].y,g_currentFloorCameraData[i].z,
        //       g_currentFloorCameraData[i].focal1,g_currentFloorCameraData[i].focal2,g_currentFloorCameraData[i].focal3);

        g_currentFloorCameraData[i].numViewedRooms = READ_LE_U16(currentCameraData+0x12);
        printf("CAM: %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx\n",g_currentFloorCameraData[i].alpha,g_currentFloorCameraData[i].beta,
               g_currentFloorCameraData[i].gamma,g_currentFloorCameraData[i].x,g_currentFloorCameraData[i].y,g_currentFloorCameraData[i].z,
               g_currentFloorCameraData[i].focal1,g_currentFloorCameraData[i].focal2,g_currentFloorCameraData[i].focal3,
               g_currentFloorCameraData[i].numViewedRooms);

        currentCameraData+=0x14;

        g_currentFloorCameraData[i].viewedRoomTable = (cameraViewedRoomStruct*)malloc(sizeof(cameraViewedRoomStruct)*g_currentFloorCameraData[i].numViewedRooms);

        ASSERT(g_currentFloorCameraData[i].viewedRoomTable);
        memset(g_currentFloorCameraData[i].viewedRoomTable, 0, sizeof(cameraViewedRoomStruct)*g_currentFloorCameraData[i].numViewedRooms);

        for(k=0;k<g_currentFloorCameraData[i].numViewedRooms;k++)
        {
          cameraViewedRoomStruct* pCurrentCameraViewedRoom;

          pCurrentCameraViewedRoom = &g_currentFloorCameraData[i].viewedRoomTable[k];

          pCurrentCameraViewedRoom->viewedRoomIdx = READ_LE_U16(currentCameraData+0x00);
          pCurrentCameraViewedRoom->offsetToMask = READ_LE_U16(currentCameraData+0x02);
          pCurrentCameraViewedRoom->offsetToCover = READ_LE_U16(currentCameraData+0x04);

          if(g_gameId == AITD1)
          {
              pCurrentCameraViewedRoom->offsetToHybrids = 0;
              pCurrentCameraViewedRoom->offsetCamOptims = 0;
              pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData+0x06);
              pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData+0x08);
              pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData+0x0A);
          }
          else
          {
              pCurrentCameraViewedRoom->offsetToHybrids = READ_LE_U16(currentCameraData+0x06);
              pCurrentCameraViewedRoom->offsetCamOptims = READ_LE_U16(currentCameraData+0x08);
              pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData+0x0A);
              pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData+0x0C);
              pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData+0x0E);
          }

          // load camera mask
          unsigned char* pMaskData = NULL;
          if(g_gameId >= JACK)
          {
              pMaskData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToMask;

              // for this camera, how many masks zone
              pCurrentCameraViewedRoom->numMask = READ_LE_U16(pMaskData);
              pMaskData+=2;

              pCurrentCameraViewedRoom->masks = (cameraMaskStruct*)malloc(sizeof(cameraMaskStruct)*pCurrentCameraViewedRoom->numMask);
              memset(pCurrentCameraViewedRoom->masks, 0, sizeof(cameraMaskStruct)*pCurrentCameraViewedRoom->numMask);

              for(int k=0; k<pCurrentCameraViewedRoom->numMask; k++)
              {
                  cameraMaskStruct* pCurrentCameraMask = &pCurrentCameraViewedRoom->masks[k];

                  // for this overlay zone, how many
                  pCurrentCameraMask->numTestRect = READ_LE_U16(pMaskData);
                  pMaskData+=2;

                  pCurrentCameraMask->rectTests = (rectTestStruct*)malloc(sizeof(rectTestStruct)*pCurrentCameraMask->numTestRect);
                  memset(pCurrentCameraMask->rectTests, 0, sizeof(rectTestStruct)*pCurrentCameraMask->numTestRect);

                  for(int j=0; j<pCurrentCameraMask->numTestRect; j++)
                  {
                      rectTestStruct* pCurrentRectTest = &pCurrentCameraMask->rectTests[j];

                      pCurrentRectTest->zoneX1 = READ_LE_S16(pMaskData);
                      pCurrentRectTest->zoneZ1 = READ_LE_S16(pMaskData+2);
                      pCurrentRectTest->zoneX2 = READ_LE_S16(pMaskData+4);
                      pCurrentRectTest->zoneZ2 = READ_LE_S16(pMaskData+6);
                      pMaskData+=8;
                  }
              }
          }

          // load camera cover
          {
              unsigned char* pZoneData;
              int numZones;
              int j;

              pZoneData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToCover;
              if(pMaskData)
              {
                  assert(pZoneData == pMaskData);
              }
              //pZoneData = currentCameraData;

              pCurrentCameraViewedRoom->numCoverZones = numZones =READ_LE_U16(pZoneData);
              pZoneData+=2;
              printf("numZones: %d\n", numZones);
              pCurrentCameraViewedRoom->coverZones = (cameraZoneEntryStruct*)malloc(sizeof(cameraZoneEntryStruct)*numZones);

              ASSERT(pCurrentCameraViewedRoom->coverZones);

              for(j=0;j<pCurrentCameraViewedRoom->numCoverZones;j++)
              {
                  int pointIdx;
                  int numPoints;

                  pCurrentCameraViewedRoom->coverZones[j].numPoints = numPoints = READ_LE_U16(pZoneData);
                  pZoneData+=2;

                  pCurrentCameraViewedRoom->coverZones[j].pointTable = (cameraZonePointStruct*)malloc(sizeof(cameraZonePointStruct)*(numPoints+1));

                  for(pointIdx = 0; pointIdx < pCurrentCameraViewedRoom->coverZones[j].numPoints; pointIdx++)
                  {
                      pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].x = READ_LE_U16(pZoneData);
                      pZoneData+=2;
                      pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].y = READ_LE_U16(pZoneData);
                      pZoneData+=2;
                  }

                  pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].x = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].x; // copy first point to last position
                  pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].y = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].y;
              }
          }

          if(g_gameId == AITD1)
              currentCameraData+=0x0C;
          else
              currentCameraData+=0x10;

          if(g_gameId == TIMEGATE)
          {
              currentCameraData+=6;
          }
        }
      }
      else
      {
        break;
      }
    }

    exportCollada();
    return true;
}

std::string AloneFloor::hardCol2collada(hardColStruct& hardcol, int index, int roomNum)
{
    printf("HardCol  type: %d  param: %d\n",hardcol.type,hardcol.parameter);
    ZVStruct* zvData;
    zvData = &hardcol.zv;
    float roomX=roomDataTable[roomNum].worldX;
    float roomY=roomDataTable[roomNum].worldY;
    float roomZ=roomDataTable[roomNum].worldZ;

    std::ostringstream oss;
    float x=(zvData->ZVX1)/1000.0+roomX/100.0;
    float y=(zvData->ZVY1)/1000.0+roomY/100.0;
    float z=(zvData->ZVZ1)/1000.0-roomZ/100.0;
    float sx=(zvData->ZVX2-zvData->ZVX1)/1000.0;
    float sy=(zvData->ZVY2-zvData->ZVY1)/1000.0;
    float sz=(zvData->ZVZ2-zvData->ZVZ1)/1000.0;
    oss<<"      <node id=\"Col"<<index<<"\" name=\"Col"<<index<<"_r"<<roomNum<<"_t"<<hardcol.type<<"_p"<<hardcol.parameter<<"\" type=\"NODE\">\n";
    oss<<"        <matrix sid=\"transform\">"<<sx<<" 0 0 "<<x<<" 0 "<<sy<<" 0 "<<y<<" 0 0 "<<sz<<" "<<z<<" 0 0 0 1</matrix>\n";
    oss<<"        <instance_geometry url=\"#Cube-mesh\"/>\n";
    oss<<"      </node>\n";
    //TODO: add parameter and type
    return oss.str();
}

std::string AloneFloor::sceZone2collada(sceZoneStruct &sceZone, int index, int roomNum)
{
    printf("SceZone  type: %d  param: %d\n",sceZone.type,sceZone.parameter);
    ZVStruct* zvData;
    zvData = &sceZone.zv;
    float roomX=roomDataTable[roomNum].worldX;
    float roomY=roomDataTable[roomNum].worldY;
    float roomZ=roomDataTable[roomNum].worldZ;

    std::ostringstream oss;
    float x=(zvData->ZVX1)/1000.0+roomX/100.0;
    float y=(zvData->ZVY2)/1000.0+roomY/100.0;
    float z=(zvData->ZVZ1)/1000.0-roomZ/100.0;
    float sx=(zvData->ZVX2-zvData->ZVX1)/1000.0;
    float sy=(zvData->ZVY2-zvData->ZVY1)/1000.0;
    float sz=(zvData->ZVZ2-zvData->ZVZ1)/1000.0;
    oss<<"      <node id=\"Sce"<<index<<"\" name=\"Sce"<<index<<"_r"<<roomNum<<"_t"<<sceZone.type<<"_p"<<sceZone.parameter<<"\" type=\"NODE\">\n";
    oss<<"        <matrix sid=\"transform\">"<<sx<<" 0 0 "<<x<<" 0 "<<sy<<" 0 "<<y<<" 0 0 "<<sz<<" "<<z<<" 0 0 0 1</matrix>\n";
    oss<<"        <instance_geometry url=\"#Cube-mesh\"/>\n";
    oss<<"      </node>\n";
    //TODO: add parameter and type
    return oss.str();
}

std::string AloneFloor::cam2collada_lib(cameraDataStruct* cam, int index)
{
    std::ostringstream oss;
    float fovX=2.0*atan(160.0/cam->focal2)*180.0/M_PI;
    float fovY=2.0*atan(160.0/cam->focal3)*180.0/M_PI;
    oss<<"    <camera id=\"Camera-camera"<<index<<"\">\n";
    oss<<"      <optics>\n";
    oss<<"        <technique_common>\n";
    oss<<"          <perspective>\n";
    oss<<"            <xfov sid=\"xfov\">"<<fovX<<"</xfov>\n";
    oss<<"            <yfov sid=\"yfov\">"<<fovY<<"</yfov>\n";
    oss<<"            <znear sid=\"znear\">2</znear>\n";
    oss<<"            <zfar sid=\"zfar\">12</zfar>\n";
    oss<<"          </perspective>\n";
    oss<<"        </technique_common>\n";
    oss<<"      </optics>\n";
    oss<<"    </camera>\n";
    return oss.str();
}

std::string AloneFloor::cam2collada_node(cameraDataStruct* cam, int index, int roomNum)
{
    std::ostringstream oss;
    float roomX=0;//roomDataTable[roomNum].worldX;
    float roomY=0;//roomDataTable[roomNum].worldY;
    float roomZ=0;//roomDataTable[roomNum].worldZ;
/*  for collada:
    a=alpha*pi/512
    b=beta*pi/512
    c=gamma*pi/512
    rx=[-1 0 0;0 cos(a) -sin(a);0 sin(a) cos(a)]
    ry=[cos(b) 0 -sin(b);0 1 0;sin(b) 0 cos(b)]
    rz=[cos(c) -sin(c) 0;sin(c) cos(c) 0; 0 0 1]
    r=(-rz*rx*ry)'

    dcam=R*[0;0;1]*focal1/1000
  */

    float camX=(cam->x-roomX)/100.0;
    float camY=(roomY-cam->y)/100.0;
    float camZ=(roomZ-cam->z)/100.0;

    double rotX_[]={
        -1,0,0,
        0,cosTable[(cam->alpha+0x100)&0x3FF]/32768.0,-cosTable[cam->alpha&0x3FF]/32768.0,
        0,cosTable[cam->alpha&0x3FF]/32768.0,cosTable[(cam->alpha+0x100)&0x3FF]/32768.0
    };
    double rotY_[]={
        cosTable[(cam->beta+0x100)&0x3FF]/32768.0,0,-cosTable[cam->beta&0x3FF]/32768.0,
        0,1,0,
        cosTable[cam->beta&0x3FF]/32768.0,0,cosTable[(cam->beta+0x100)&0x3FF]/32768.0
    };
    double rotZ_[]={
        cosTable[(cam->gamma+0x100)&0x3FF]/32768.0,-cosTable[cam->gamma&0x3FF]/32768.0,0,
        cosTable[cam->gamma&0x3FF]/32768.0,cosTable[(cam->gamma+0x100)&0x3FF]/32768.0,0,
        0,0,1
    };
    QGenericMatrix<3,3,double> rotX(rotX_);
    QGenericMatrix<3,3,double> rotY(rotY_);
    QGenericMatrix<3,3,double> rotZ(rotZ_);

    QGenericMatrix<3,3,double> rot=-(rotZ*rotX*rotY).transposed();

    double dcamX=rot(0,2)*cam->focal1/1000.0;
    double dcamY=rot(1,2)*cam->focal1/1000.0;
    double dcamZ=rot(2,2)*cam->focal1/1000.0;

    oss<<"      <node id=\"Cam"<<index<<"\" name=\"Cam"<<index<<"_r"<<roomNum<<"\" type=\"NODE\">\n";
    oss<<"        <matrix sid=\"transform\">"<<rot(0,0)<<" "<<rot(0,1)<<" "<<rot(0,2)<<" " <<camX+dcamX
       <<" "<<rot(1,0)<<" "<<rot(1,1)<<" "<<rot(1,2)<<" " <<camY+dcamY
       <<" "<<rot(2,0)<<" "<<rot(2,1)<<" "<<rot(2,2)<<" " <<camZ+dcamZ
       <<" 0 0 0 1</matrix>\n";
    oss<<"        <instance_camera url=\"#Camera-camera"<<index<<"\"/>\n";
    oss<<"      </node>\n";

    std::cout<<"Cam "<<index<<": "<<cam->x<<","<<cam->y<<","<<cam->z;
    std::cout<<"  ang: "<<cam->alpha<<","<<cam->beta<<","<<cam->gamma;
    std::cout<<"  foc: "<<cam->focal1<<","<<cam->focal2<<","<<cam->focal3<<"\n";
    return oss.str();
}

void AloneFloor::exportCollada()
{
    std::time_t time = std::time(nullptr);
    printf("exportCollada\n");
    char filename[256];
    strcpy(filename,mRooms->mPAKFilename);
    strcat(filename,".dae");
    std::cout<<"outname: "<<filename<<std::endl;
    std::ostringstream oss;

    oss<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    oss<<"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
    oss<<"  <asset>\n";
    oss<<"    <contributor>\n";
    oss<<"      <author>AITD PakEdit v"<<EDITOR_VERSION<<"</author>\n";
    oss<<"    </contributor>\n";
    oss<<"    <created>"<<std::asctime(std::localtime(&time))<<"</created>\n";
    oss<<"    <unit name=\"meter\" meter=\"1\"/>\n";
    oss<<"    <up_axis>Z_UP</up_axis>\n";
    oss<<"  </asset>\n";
    oss<<"  <library_cameras>\n";

    for (unsigned int i=0;i<expectedNumberOfRoom;i++)
    {
        roomDataStruct* currentRoomDataPtr = &roomDataTable[i];
        for (unsigned int j=0;j<currentRoomDataPtr->numCameraInRoom;j++)
        {
            u16 idx=currentRoomDataPtr->cameraIdxTable[j];
            oss<<cam2collada_lib(&g_currentFloorCameraData[idx],idx);
        }
    }
    printf("exportCollada cam assets\n");

    oss<<"  </library_cameras>\n";
    oss<<"  <library_images/>\n";
    oss<<"  <library_geometries>\n";
    oss<<"    <geometry id=\"Cube-mesh\">\n";
    oss<<"      <mesh>\n";
    oss<<"        <source id=\"Cube-mesh-positions\">\n";
    oss<<"          <float_array id=\"Cube-mesh-positions-array\" count=\"24\">1 1 0 1 0 0 0 0 0 0 1 0 1 1 1 1 0 1 0 0 1 0 1 1</float_array>\n";
    oss<<"          <technique_common>\n";
    oss<<"            <accessor source=\"#Cube-mesh-positions-array\" count=\"8\" stride=\"3\">\n";
    oss<<"              <param name=\"X\" type=\"float\"/>\n";
    oss<<"              <param name=\"Y\" type=\"float\"/>\n";
    oss<<"              <param name=\"Z\" type=\"float\"/>\n";
    oss<<"            </accessor>\n";
    oss<<"          </technique_common>\n";
    oss<<"        </source>\n";
    oss<<"        <vertices id=\"Cube-mesh-vertices\">\n";
    oss<<"          <input semantic=\"POSITION\" source=\"#Cube-mesh-positions\"/>\n";
    oss<<"        </vertices>\n";
    oss<<"        <polylist count=\"6\">\n";
    oss<<"          <input semantic=\"VERTEX\" source=\"#Cube-mesh-vertices\" offset=\"0\"/>\n";
    oss<<"          <vcount>4 4 4 4 4 4 </vcount>\n";
    oss<<"          <p>0 1 2 3 4 7 6 5 0 4 5 1 1 5 6 2 2 6 7 3 4 0 3 7</p>\n";
    oss<<"        </polylist>\n";
    oss<<"      </mesh>\n";
    oss<<"    </geometry>\n";
    oss<<"  </library_geometries>\n";
    oss<<"  <library_controllers/>\n";
    oss<<"  <library_visual_scenes>\n";
    oss<<"    <visual_scene id=\"Scene\" name=\"Scene\">\n";

    /*for(unsigned int i=0;i<expectedNumberOfCamera;i++)
        oss<<cam2collada_node(&g_currentFloorCameraData[i],i,0);//TODO: determine room and give its center
    printf("exportCollada cam nodes\n");*/

    for (unsigned int i=0;i<expectedNumberOfRoom;i++)
    {
        roomDataStruct* currentRoomDataPtr = &roomDataTable[i];
        printf("exportCollada room %d: (%d,%d,%d)\n",i,currentRoomDataPtr->worldX,currentRoomDataPtr->worldY,currentRoomDataPtr->worldZ);
        for (unsigned int j=0;j<currentRoomDataPtr->numCameraInRoom;j++)
        {
            u16 idx=currentRoomDataPtr->cameraIdxTable[j];
            oss<<cam2collada_node(&g_currentFloorCameraData[idx],idx,0);//TODO: room center not used?
        }

        for (unsigned int j=0;j<currentRoomDataPtr->numHardCol;j++)
        {
            oss<<hardCol2collada(currentRoomDataPtr->hardColTable[j],j,i);
        }
        for (unsigned int j=0;j<currentRoomDataPtr->numSceZone;j++)
        {
            oss<<sceZone2collada(currentRoomDataPtr->sceZoneTable[j],j,i);
        }
    }
    oss<<"      <node id=\"Cube0\" name=\"Cube0\" type=\"NODE\">\n";
    oss<<"        <matrix sid=\"transform\">1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1</matrix>\n";
    oss<<"        <instance_geometry url=\"#Cube-mesh\"/>\n";
    oss<<"      </node>\n";

    oss<<"    </visual_scene>\n";
    oss<<"  </library_visual_scenes>\n";
    oss<<"  <scene>\n";
    oss<<"    <instance_visual_scene url=\"#Scene\"/>\n";
    oss<<"  </scene>\n";
    oss<<"</COLLADA>\n";

   FILE *F;
   F=fopen(filename,"w");
   fprintf(F,"%s",oss.str().c_str());
   fclose(F);
   //std::ofstream fileout; //crash on windows??
   //fileout << oss.str()<<std::endl;
   //fileout.close();
}


bool AloneFloor::importCollada(const char *filename,AloneFile *roomsFile,AloneFile *camsFile)
{
    QDomDocument domDocument;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0,"Error","Impossible to open Collada file!");
        return false;
    }
    if (!domDocument.setContent(&file)) {
        QMessageBox::information(0,"Error","Error in XML!");
        return false;
    }

    QDomElement root = domDocument.documentElement();
    std::cout<<root.tagName().toStdString()<<std::endl;
    //COLLADA library_visual_scenes visual_scene node name

    rooms.clear();

    QDomNode n = root.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            //std::cout << qPrintable(e.tagName()) << std::endl; // the node really is an element.
            if (e.tagName()=="library_visual_scenes")
            {
                QDomNode n2 = n.firstChild();
                while(!n2.isNull()) {
                    e = n2.toElement();
                    if(!e.isNull()) {
                        if (e.tagName()=="visual_scene")
                        {
                            QDomNode n3 = n2.firstChild();
                            while(!n3.isNull()) {
                                e = n3.toElement();
                                std::cout << e.attribute("name").toStdString() << std::endl;
                                xml2struct(e);
                                n3 = n3.nextSibling();
                            }
                        }
                    }
                    n2 = n2.nextSibling();
                }
            }
        }
        n = n.nextSibling();
    }

    //TODO: fill data structure
    /*File0=rooms
     * list offsets rooms 32b (1st=nb rooms*4)
     * data room :
     *   hardcol relative offset 16b
     *   scezone relative offset  16b
     *   x,y,z room 16b
     *   nb cam 16b
     *   liste cam idx 16b = cam counter
     *   nb hardcol 16b
     *   liste ZVX1 ZVX2... param type 8*16b
     *   nb scezone 16b
     *   liste ZVX1 ZVX2... param type 8*16b
     *File1=cameras
     * */
    u32 nbHardCols=0;
    u32 nbSceZones=0;
    u32 nbCameras=0;
    u32 roomsDataSz=4;//+4 to look like original file... why??
    for (u16 i=0;i<rooms.size();i++)
    {
        rooms[i].worldX=roomDataTable[i].worldX;
        rooms[i].worldY=roomDataTable[i].worldY;
        rooms[i].worldZ=roomDataTable[i].worldZ;

        rooms[i].numCameraInRoom=roomDataTable[i].numCameraInRoom;//TODO: get it from cameras names
        rooms[i].cameraIdxTable=roomDataTable[i].cameraIdxTable;
        nbCameras+=roomDataTable[i].numCameraInRoom;
        nbHardCols+=rooms[i].hardCols.size();
        nbSceZones+=rooms[i].sceZones.size();

        roomsDataSz+=4;//room offset
        roomsDataSz+=rooms[i].computeSize();
    }
    printf("Found %d cams, %d hardcols, %d scezones.\n",nbCameras,nbHardCols,nbSceZones);

    char *roomsData = (char*)malloc(roomsDataSz);

    char* roomsDataPtr=roomsData;
    u32 currentOffset=rooms.size()*4+4;//+4 to look like original file... why??
    for (u16 i=0;i<rooms.size();i++)
    {
        *(u32*)roomsDataPtr=currentOffset;
        roomsDataPtr+=4;
        currentOffset+=rooms[i].computeSize();
    }
    roomsDataPtr=roomsData+4*rooms.size()+4;//+4 to look like original file... why??
    for (u16 i=0;i<rooms.size();i++)
    {
        //hcoffset, szoffset, x,y,z,nbcam,camsidx
        u16 hardColOffset=2+2+2*3+2+2*rooms[i].numCameraInRoom;
        //hcoffset, szoffset, x,y,z,nbcam,camsidx,nbhc,hcs
        u16 sceZoneOffset=2+2+2*3+2+2*rooms[i].numCameraInRoom+2+rooms[i].hardCols.size()*sizeof(hardColStruct);
        *(u16*)roomsDataPtr=hardColOffset;roomsDataPtr+=2;
        *(u16*)roomsDataPtr=sceZoneOffset;roomsDataPtr+=2;
        *(s16*)roomsDataPtr=rooms[i].worldX;roomsDataPtr+=2;
        *(s16*)roomsDataPtr=rooms[i].worldY;roomsDataPtr+=2;
        *(s16*)roomsDataPtr=rooms[i].worldZ;roomsDataPtr+=2;
        *(u16*)roomsDataPtr=rooms[i].numCameraInRoom;roomsDataPtr+=2;
        for (u16 j=0;j<rooms[i].numCameraInRoom;j++)
        {
            *(u16*)roomsDataPtr=rooms[i].cameraIdxTable[j];roomsDataPtr+=2;
        }
        *(u16*)roomsDataPtr=rooms[i].hardCols.size();roomsDataPtr+=2;
        for (u16 j=0;j<rooms[i].hardCols.size();j++)
        {
            *(hardColStruct*)roomsDataPtr=*rooms[i].hardCols[j];roomsDataPtr+=sizeof(hardColStruct);
        }
        *(u16*)roomsDataPtr=rooms[i].sceZones.size();roomsDataPtr+=2;
        for (u16 j=0;j<rooms[i].sceZones.size();j++)
        {
            *(sceZoneStruct*)roomsDataPtr=*rooms[i].sceZones[j];roomsDataPtr+=sizeof(sceZoneStruct);
            printf("sceZoneStruct %hx %hx %hx %hx %hx %hx %hx %hx\n",
                   rooms[i].sceZones[j]->zv.ZVX1,rooms[i].sceZones[j]->zv.ZVX2,
                   rooms[i].sceZones[j]->zv.ZVY1,rooms[i].sceZones[j]->zv.ZVY2,
                   rooms[i].sceZones[j]->zv.ZVZ1,rooms[i].sceZones[j]->zv.ZVZ2,
                   rooms[i].sceZones[j]->parameter,rooms[i].sceZones[j]->type);
        }
    }

    FILE* fileHandle;
    fileHandle = fopen("tmp.dat","wb");
    fwrite(roomsData,roomsDataSz,1,fileHandle);
    fclose(fileHandle);

    delete roomsFile->mDecomprData;
    roomsFile->mInfo.uncompressedSize=roomsDataSz;
    roomsFile->mDecomprData=roomsData;

    delete roomsFile->mComprData;
    roomsFile->mComprData = (char*)malloc(roomsDataSz);
    memcpy((char*)roomsFile->mComprData,(char*)roomsFile->mDecomprData,roomsDataSz);
    roomsFile->mInfo.discSize=roomsFile->mInfo.uncompressedSize;
    roomsFile->mInfo.compressionFlag=0;

    return true;
}

bool AloneFloor::xml2struct(QDomNode &n)
{
    //Col0_r0_t1_p0   Sce0_r0_t10_p0
    QRegularExpression reCol("Col\\d+_r(\\d+)_t(\\d+)_p(\\d+)");
    QDomElement e = n.toElement();
    QRegularExpressionMatch match = reCol.match(e.attribute("name"));
    if (match.hasMatch()) {
        std::cout<<"match !  r="<<match.captured(1).toStdString()<<" t="<<match.captured(2).toStdString()<<" p="<<match.captured(3).toStdString()<<std::endl;
        unsigned r=match.captured(1).toInt();
        if (r+1>rooms.size())
            rooms.resize(r+1);
        QDomNode n2 = n.firstChild();
        while(!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if(!e2.isNull()) {
                if (e2.tagName()=="matrix")
                {
                    std::cout<<"matrix: "<<e2.text().toStdString()<<std::endl;
                    QStringList matlist = e2.text().split(" ",QString::SkipEmptyParts);
                    float scaleX=matlist[0].toFloat();
                    float scaleY=matlist[5].toFloat();
                    float scaleZ=matlist[10].toFloat();
                    float posX=matlist[3].toFloat();
                    float posY=matlist[7].toFloat();
                    float posZ=matlist[11].toFloat();
                    int roomNum=match.captured(1).toInt();
                    hardColStruct *hardCol=new hardColStruct();
                    hardCol->type=match.captured(2).toInt();
                    hardCol->parameter=match.captured(3).toInt();

                    float roomX=roomDataTable[roomNum].worldX;
                    float roomY=roomDataTable[roomNum].worldY;
                    float roomZ=roomDataTable[roomNum].worldZ;

                    hardCol->zv.ZVX1=1000*posX-10*roomX;
                    hardCol->zv.ZVY1=1000*posY-10*roomY;
                    hardCol->zv.ZVZ1=1000*posZ+10*roomZ;
                    hardCol->zv.ZVX2=1000*scaleX+hardCol->zv.ZVX1;
                    hardCol->zv.ZVY2=1000*scaleY+hardCol->zv.ZVY1;
                    hardCol->zv.ZVZ2=1000*scaleZ+hardCol->zv.ZVZ1;
                    rooms[r].hardCols.push_back(hardCol);
                    return true;
                }
            }
            n2 = n2.nextSibling();
        }
    }

    QRegularExpression reSce("Sce\\d+_r(\\d+)_t(\\d+)_p(\\d+)");
    e = n.toElement();
    match = reSce.match(e.attribute("name"));
    if (match.hasMatch()) {
        std::cout<<"match !  r="<<match.captured(1).toStdString()<<" t="<<match.captured(2).toStdString()<<" p="<<match.captured(3).toStdString()<<std::endl;
        unsigned r=match.captured(1).toInt();
        if (r+1>rooms.size())
            rooms.resize(r+1);
        QDomNode n2 = n.firstChild();
        while(!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if(!e2.isNull()) {
                if (e2.tagName()=="matrix")
                {
                    std::cout<<"matrix: "<<e2.text().toStdString()<<std::endl;
                    QStringList matlist = e2.text().split(" ",QString::SkipEmptyParts);
                    float scaleX=matlist[0].toFloat();
                    float scaleY=matlist[5].toFloat();
                    float scaleZ=matlist[10].toFloat();
                    float posX=matlist[3].toFloat();
                    float posY=matlist[7].toFloat();
                    float posZ=matlist[11].toFloat();
                    int roomNum=match.captured(1).toInt();
                    sceZoneStruct *sceZone=new sceZoneStruct();
                    sceZone->type=match.captured(2).toInt();
                    sceZone->parameter=match.captured(3).toInt();

                    float roomX=roomDataTable[roomNum].worldX;
                    float roomY=roomDataTable[roomNum].worldY;
                    float roomZ=roomDataTable[roomNum].worldZ;

                    /*
                    float x=(zvData->ZVX1)/1000.0+roomX/100.0;
                    float y=(zvData->ZVY2)/1000.0+roomY/100.0;
                    float z=(zvData->ZVZ1)/1000.0-roomZ/100.0;
                    float sx=(zvData->ZVX2-zvData->ZVX1)/1000.0;
                    float sy=1;//(zvData->ZVY2-zvData->ZVY1)/100.0;
                    float sz=(zvData->ZVZ2-zvData->ZVZ1)/1000.0;*/
                    sceZone->zv.ZVX1=1000*posX-10*roomX;
                    sceZone->zv.ZVY2=1000*posY-10*roomY;
                    sceZone->zv.ZVZ1=1000*posZ+10*roomZ;
                    sceZone->zv.ZVX2=1000*scaleX+sceZone->zv.ZVX1;
                    sceZone->zv.ZVY1=-1000*scaleY+sceZone->zv.ZVY2;
                    sceZone->zv.ZVZ2=1000*scaleZ+sceZone->zv.ZVZ1;
                    rooms[r].sceZones.push_back(sceZone);
                    return true;
                }
            }
            n2 = n2.nextSibling();
        }
    }
    return false;
}

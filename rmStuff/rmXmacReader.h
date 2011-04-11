#pragma once

#include "rmFileStream.h"
#include "rmScene.h"

#define USE_BASEVERTS false //$

class rmXmacReader
{
public:
	static void readXmacFileToScene(rmInFileStream * fileStream, rmScene * scene, vector<PosDescriptor> * getInfo = 0);

private:
	static void readMeshSection(void);
	static void readWeightingSection(void);
	static void readInfoSection(void);
	static void readNodesSection(void);
	static void readMatsSection(void);

	static void readVertsSubSection(void);
	static void readNormalsSubSection(void);
	static void readTangentsSubSection(void);
	static void readTVertsSubSection(void);
	static void readBaseVertsSubSection(void);
	static void readFacesSubSection(void);

	static rmInFileStream * fileStream;
	static rmScene * scene;
	static rmMesh * curMesh;
	static rmU32 * baseVertIndices;
	static rmU32 nVertices;

	static vector<PosDescriptor> info;
	static PosDescriptor pDesc;

	static rmOut out;
};

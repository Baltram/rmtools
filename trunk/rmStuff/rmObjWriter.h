#pragma once

#include "rmDataTypes.h"
#include "rmFileStream.h"
#include "rmScene.h"

class rmObjWriter
{
public:
	static void writeSceneToMtlFile(rmOutFileStream * fileStream, rmScene * scene);
	static void writeSceneToObjFile(rmOutFileStream * fileStream, rmScene * scene, rmU32 nodeIndex);

private:
	static void writeMesh(rmMesh & m);
	static void checkMatId(rmU32 newMatId);

	static rmScene * scene;
	static rmOutFileStream * fileStream;

	static rmU32 curMatId;

	static rmOut out;
};

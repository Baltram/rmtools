#pragma once

#include "rmOptimizedMesh.h"
#include "rmNearestNeighbor.h"
#include "rmFileStream.h"
#include "rmScene.h"

class rmXmacWriter
{
public:
	static void overwriteVertices(string xmacFilePath, rmNode * node);
	static void replaceMesh(string xmacFilePath, const rmScene & scene, rmU32 iNodeInScene = 0);

	static rmNode * node;
	static rmInFileStream * inFileStream;
	static rmOutFileStream * outFileStream;

	static rmOut out;
};


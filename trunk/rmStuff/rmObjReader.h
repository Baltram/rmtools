#pragma once

#include "rmDataTypes.h"
#include "rmFileStream.h"
#include "rmScene.h"

class rmObjReader
{
private:
	enum Token
	{
		TOKEN_UNSUPPORTED = -1,
		TOKEN_F,
		TOKEN_MAP_KD,
		TOKEN_MTLLIB,
		TOKEN_NEWMTL,
		TOKEN_USEMTL,
		TOKEN_V,
		TOKEN_VN,
		TOKEN_VT,
		TOKEN_COUNT
	};
	static rmLPCChar const tokenStrings[TOKEN_COUNT];

public:
	static void readObjFileToScene(rmInFileStream * fileStream, rmScene * scene);

private:
	static void readMtlFile(string materialLibraryName);

	static rmInFileStream * fileStream;
	static rmScene * scene;

	static rmOut out;
};

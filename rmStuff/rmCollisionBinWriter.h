#pragma once

#ifdef RM_PHYSX

#include "rmXcomFile.h"
#include "rmStringTable.h"
#include "rmDir.h"

class rmCollisionBinWriter
{
public:
	static void registerFile(string xcomFilePath, string binFilePath);
	static void rebuildBin(string binFilePath);

private:
	static void writeXcomFileAsRecord(void);

private:
	static rmInFileStream binIn;
	static rmOutFileStream bin;
	static rmInFileStream xcom;

	static rmStringTable stringTable;

	static rmOut out;
};

#endif // RM_PHYSX

#pragma once

#include "rmDataTypes.h"
#include "rmObjWriter.h" //
#include "rmOut.h"
#include "rmScene.h" //
#include <algorithm>

class rmNearestNeighbor
{
private: 
	struct LookUpPoint
	{
		rmU64 location;
		rmU32 iCube;
		rmU8 xyzInCube[3];
		rmU8 nLevelsSearched;
		const rmPoint3 * pos;
		LookUpPoint * next;
	};
	
	struct LookUpCube
	{
		rmU64 pointSpread;
		rmU32 nPoints;
		LookUpPoint ** points;
	};

	struct FoundPoint
	{
		rmF32 distance;
		LookUpPoint * point;
		rmI32 offsetToNext;
	};

public:
	static void findNearestTargetVerts(const rmPoint3 * vertsToMatch, const rmU32 nVertsToMatch, const rmPoint3 * targetVerts, const rmU32 nTargetVerts, vector<rmU32> & dest);

private:	
	static void printToObj(const rmU64 & src);
	static void printToObj(rmU64 * src, rmU32 iSystem);
	static string toString(rmU64 & src);

	static void init(void);

	static void init3dLoop(rmU32 xCount, rmU32 yCount, rmU32 zCount);
	static void nextBlock(void);
	static void nextCube(void);

	static void initShift(rmU32 * system, rmU32 direction, rmU32 count);
	static void shift(void);

	static void searchPoints(LookUpPoint * p);

	static rmBool isInRightOrder(const FoundPoint & p1, const FoundPoint & p2);

	static rmU64 * buffer;
	static rmU64 * buffer2;
	static rmU64 * searchLevel[4];

	static rmU64 first[9];

	static rmU32 system3[3];
	static rmU32 system4[3];
	static rmU32 system5[3];

	static rmU32 systemCur[3];
	static rmU32 extentsCur[3];

	static rmU64 tmp;

	static rmU64 save;
	static rmU64 keep;
	static rmU32 rotate;
	static rmU32 backrotate;
	static rmU32 next;

	static rmU64 * curBlock;
	static rmU64 * curLevel;

	static rmU32 x;
	static rmU32 y;
	static rmU32 z;

	static rmU32 maxX;
	static rmU32 maxY;
	static rmU32 maxZ;

	static LookUpPoint * points;
	static LookUpPoint * endPoints;
	static LookUpCube * cubes;
	static LookUpCube * curCube;
	static vector<FoundPoint> foundPoints;
	static rmU32 * foundAt;

	static rmOut out;
};

#define SECURITY_PUFFER_0 0.000001
#define SECURITY_PUFFER_1 1.000001

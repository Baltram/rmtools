#pragma once

#ifdef RM_PHYSX

#include <ctime>
#include "rmPhysxGlobals.h"
#include <NxTriangleMesh.h>
#include <NxTriangleMeshDesc.h>
#include "rmNxInFileStream.h"
#include "rmNxOutFileStream.h"
#include "rmMesh.h"

#define XCOL_F_MAX 3.40282346e38f
#define XCOL_F_MIN -3.40282346e38f

class rmXcomFile
{
public:
    enum ShapeMaterial
    {
	None,
	Wood,
	Metal,
	Water,
	Stone,
	Earth,
	Ice,
	Leather,
	Clay,
	Glass,
	Flesh,
	Snow,
	Debris,
	Foliage,
	Magic,
	Grass,
	SpringAndDamper1,
	SpringAndDamper2,
	SpringAndDamper3,
	Damage,
	Sand,
	Movement,
	Axe,
	NShapeMaterials
    };


    struct TriangleMesh
    {
	rmMesh mesh;
	ShapeMaterial shapeMaterial;
	rmBool ignoredByTraceRay;
	rmBool disableCollision;
	rmBool disableResponse;
    };

public:
    static rmLPCChar const shapeMaterialNames[NShapeMaterials];

public:
    rmXcomFile(void);
    ~rmXcomFile(void);

    void reset(void);

    TriangleMesh * getTriangleMesh(rmU32 index);
    void removeTriangleMesh(rmU32 index);
    TriangleMesh * addTriangleMesh(void);
    rmU32 getNumTriangleMeshes(void);

    bool read(string xcomFilePath);
    bool write(string xcomFilePath);

private:
    void readTriangleMesh(rmNxInFileStream * xcom, rmMesh * dest);
    rmU32 getElementSize(NxTriangleMesh * tm, NxInternalArray type);

    void initTriangleMesh(TriangleMesh * tm);
	static void writeFakeBBox(rmOutFileStream & out);

private:
    vector<TriangleMesh *> triangleMeshes;

    static rmOut out;

	friend class rmCollisionBinWriter;
};

#endif // RM_PHYSX

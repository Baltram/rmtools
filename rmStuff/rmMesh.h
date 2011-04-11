#pragma once

#include "rmDataTypes.h"
#include "rmOut.h"
#include "rmArr2.h"

class rmMesh
{
public:
	rmMesh(void);
	rmMesh(const rmMesh & m);
	~rmMesh(void);

	void reset(void);
	void swap(rmMesh * other);

	const rmPoint3 * getVertsContainer(void) const;
	const rmFace * getFacesContainer(void) const;
	const rmPoint3 * getTVertsContainer(void) const;
	const rmTVFace * getTVFacesContainer(void) const;
	const rmVec3 * getVNormalsContainer(void) const;
	const rmVNFace * getVNFacesContainer(void) const;
	const rmVec4 * getVTangentsContainer(void) const;
	const rmVTFace * getVTFacesContainer(void) const;

	void setNumVerts(rmU32 count);
	void setNumFaces(rmU32 count);
	void setNumTVerts(rmU32 count);
	void setNumVNormals(rmU32 count);
	void setNumVTangents(rmU32 count);

	rmU32 getNumVerts(void) const;
	rmU32 getNumFaces(void) const;
	rmU32 getNumTVerts(void) const;
	rmU32 getNumVNormals(void) const;
	rmU32 getNumVTangents(void) const;

	void setVert(rmU32 num, const rmPoint3 & val);
	void setFace(rmU32 num, const rmFace & val);
	void setTVert(rmU32 num, const rmPoint3 & val);
	void setTVFace(rmU32 num, const rmTVFace & val);
	void setVNormal(rmU32 num, const rmVec3 & val);
	void setVNFace(rmU32 num, const rmVNFace & val);
	void setVTangent(rmU32 num, const rmVec4 & val);
	void setVTFace(rmU32 num, const rmVTFace & val);

	void setVerts(rmU32 count, const rmPoint3 * src);
	void setFaces(rmU32 count, const rmFace * src);
	void setTVerts(rmU32 count, const rmPoint3 * src);
	void setTVFaces(rmU32 count, const rmTVFace * src);
	void setVNormals(rmU32 count, const rmVec3 * src);
	void setVNFaces(rmU32 count, const rmVNFace * src);
	void setVTangents(rmU32 count, const rmVec4 * src);
	void setVTFaces(rmU32 count, const rmVTFace * src);

	const rmPoint3 & getVert(rmU32 num) const;
	const rmFace & getFace(rmU32 num) const;
	const rmPoint3 & getTVert(rmU32 num) const;
	const rmTVFace & getTVFace(rmU32 num) const;
	const rmVec3 & getVNormal(rmU32 num) const;
	const rmVNFace & getVNFace(rmU32 num) const;
	const rmVec4 & getVTangent(rmU32 num) const;
	const rmVTFace & getVTFace(rmU32 num) const;

	rmVec3 getFNormal(rmU32 num) const;
	rmVec4 getFTangent(rmU32 num) const;
	rmVec4 getFTangent(rmU32 num, const rmVec3 & normal) const;

	rmBox calcExtents(void);

	void calcVNormalsBySGs(void);
	void calcVNormalsByAngle(rmF32 maxAngleInDegrees);

	void calcFacesUsingVerts(rmSArr2<rmFVIndex> &dest);

	void copyFacesToTVFaces(void);
	void copyFacesToVNFaces(void);
	void copyFacesToVTFaces(void);

	void swapYZ(void);

	void sortFacesByMaterial(void);

	rmMesh & operator = (const rmMesh & m);

private:
	rmU32 numVerts;
	rmU32 numFaces;
	rmU32 numTVerts;
	rmU32 numVNormals;
	rmU32 numVTangents;

	rmPoint3 * vertices;
	rmFace * faces;
	rmPoint3 * TVerts;
	rmTVFace * TVFaces;
	rmVec3 * VNormals;
	rmVNFace * VNFaces;
	rmVec4 * VTangents;
	rmVTFace * VTFaces;

	rmBox * extents;

	void resetUnInit(void);

	void extentsChanged(void);

	void calcVNormals(bool useSgs, rmF32 minCosOfAngle = 0.999999);
	void calcFNormalsWeightedByAngle(rmWFNormal *dest);
	void buildSimpleVNFaces(void);

	static rmOut out;
};

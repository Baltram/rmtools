#pragma once

#include "rmMesh.h"

class rmOptimizedMesh
{
public:
	 typedef rmBool (rmOptimizedMesh::*OptMeshComparator)(void) const;

	 struct FaceSequenceInfo
	 {
		 rmU32 matId;
		 rmU32 begin;
		 rmU32 count;
		 rmU32 nUsedVerts;
	 };
public:
	rmOptimizedMesh(const rmMesh & mesh, rmBool compareTVertsByRef = false, rmBool compareVNormalsByRef = false);
	~rmOptimizedMesh(void);

	const rmMesh * getRefMesh(void) const;

	const vector<rmFace> & faces;
	const vector<FaceSequenceInfo> & faceSequenceInfos;

	const vector<const rmPoint3 *> & verts;
	const vector<const rmPoint3 *> & tVerts;
	const vector<const rmVec3 *> & vNormals;
	const vector<const rmVec4 *> & vTangents;

	rmBool hasTVerts(void) const;
	rmBool hasVNormals(void) const;
	rmBool hasVTangents(void) const;

	void turnToMesh(rmMesh & target) const;

private:
	rmBool tVertsMatch(void) const;
	rmBool tVertRefsMatch(void) const;
	rmBool vNormalsMatch(void) const;
	rmBool vNormalRefsMatch(void) const;
	rmBool vTangentsMatch(void) const;
	rmBool noMatchingNeeded(void) const;

	vector<rmFace> m_faces;
	vector<FaceSequenceInfo> m_faceSequenceInfos;

	vector<const rmPoint3 *> m_verts;
	vector<const rmPoint3 *> m_tVerts;
	vector<const rmVec3 *> m_vNormals;
	vector<const rmVec4 *> m_vTangents;

	vector<rmVec4> m_averageVTangents;

	rmBool m_hasTVerts;
	rmBool m_hasVNormals;
	rmBool m_hasVTangents;

	rmFVIndex * listCompareIterator;
	rmFVIndex * curListIterator;

	const rmMesh * m_refMesh;

	static rmOut out;
};
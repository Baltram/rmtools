#include "rmOptimizedMesh.h"

rmOut rmOptimizedMesh::out("rmOptimizedMesh.cpp");

rmOptimizedMesh::rmOptimizedMesh(const rmMesh & mesh, rmBool compareTVertsByRef, rmBool compareVNormalsByRef) : verts(m_verts), tVerts(m_tVerts), vNormals(m_vNormals), vTangents(m_vTangents), faces(m_faces), faceSequenceInfos(m_faceSequenceInfos)
{
	m_refMesh = &mesh;

	rmU32 nVerts = mesh.getNumVerts();
	rmU32 nFaces = mesh.getNumFaces();
	rmU32 nIndices = nFaces * 3;

	const rmPoint3 * verts = mesh.getVertsContainer();
	const rmPoint3 * tVerts = mesh.getTVertsContainer();
	const rmVec3 * vNormals = mesh.getVNormalsContainer();
	const rmVec4 * vTangents = mesh.getVTangentsContainer();

	const rmFace * faces = mesh.getFacesContainer();

	m_hasTVerts = (m_refMesh->getNumTVerts() != 0);
	m_hasVNormals = (m_refMesh->getNumVNormals() != 0);
	m_hasVTangents = (m_refMesh->getNumVTangents() != 0);

	OptMeshComparator tVertCompAction = &rmOptimizedMesh::noMatchingNeeded;
	OptMeshComparator vNormalCompAction = &rmOptimizedMesh::noMatchingNeeded;
	OptMeshComparator vTangentCompAction = &rmOptimizedMesh::noMatchingNeeded;

	if (m_hasTVerts) {
		tVertCompAction = (compareTVertsByRef) ? &rmOptimizedMesh::tVertRefsMatch : &rmOptimizedMesh::tVertsMatch;
	}
	if (m_hasVNormals) {
		vNormalCompAction = (compareVNormalsByRef) ? &rmOptimizedMesh::vNormalRefsMatch : &rmOptimizedMesh::vNormalsMatch;
	}
	if (m_hasVTangents) {
		tVertCompAction = &rmOptimizedMesh::vTangentsMatch;
	}

	m_faces.resize(nFaces);

	m_verts.reserve(nIndices);
	m_hasTVerts ? m_tVerts.reserve(nIndices) : static_cast<void>(true);
	m_hasVNormals ? m_vNormals.reserve(nIndices) : static_cast<void>(true);
	m_hasVTangents ? m_vTangents.reserve(nIndices) : static_cast<void>(true);
	m_hasVTangents ? m_averageVTangents.reserve(nIndices) : static_cast<void>(true);

	// Sorting faces by material.
	vector<rmU32> oldFaceNumbers(nFaces);
	vector<rmU32> nFacesPerMat(2000, 0);
	for (rmU32 i = 0; i != nFaces; ++i) {
		++nFacesPerMat[faces[i].matId];
	}

	vector<FaceSequenceInfo *> faceSequenceInfosPerMat(2000);
	vector<rmU32> faceIdInsertPerMat(2000);
	rmU32 nFacesPassed = 0;
	for (vector<rmU32>::iterator ib = nFacesPerMat.begin(), i = ib; nFacesPassed != nFaces; ++i) {
		if (*i == 0) {
			continue;
		}
		m_faceSequenceInfos.push_back(FaceSequenceInfo());
		FaceSequenceInfo & fSI = m_faceSequenceInfos.back();
		fSI.begin = nFacesPassed;
		fSI.count = *i;
		fSI.matId = (i - ib);
		fSI.nUsedVerts = ((*i) * 3);
		faceIdInsertPerMat[fSI.matId] = fSI.begin;
		nFacesPassed += *i;
	}
	freeVector<rmU32>(nFacesPerMat);
	for (rmU32 i = 0, ie = m_faceSequenceInfos.size(); i != ie; ++i) {
		faceSequenceInfosPerMat[m_faceSequenceInfos[i].matId] = &m_faceSequenceInfos[i];
	}

	for (rmU32 i = 0; i != nFaces; ++i) {
		const rmFace & f = faces[i];
		rmU32 newFaceNumber = faceIdInsertPerMat[f.matId]++;
		oldFaceNumbers[newFaceNumber] = i;
		rmFace & newFace = m_faces[newFaceNumber];
		newFace.matId = f.matId;
		newFace.sgs = f.sgs;
	}
	freeVector<rmU32>(faceIdInsertPerMat);

	rmArr2<rmFVIndex> vertOccurancesUnsorted(nIndices, nVerts);

	rmU32 j = 0;
	for (rmU32 i = 0; i != nFaces; ++i) {
		rmFace & f = m_faces[i];
		f.a = j++;
		f.b = j++;
		f.c = j++;
	}

	if (true) {
		rmFVIndex iOccurance = 0;
		for (int i = 0; i != nFaces; ++i) {
			const rmFace & f = mesh.getFace(oldFaceNumbers[i]);
			m_verts.push_back(verts + f.a);
			m_verts.push_back(verts + f.b);
			m_verts.push_back(verts + f.c);
			vertOccurancesUnsorted.append(iOccurance++, f.a); 
			vertOccurancesUnsorted.append(iOccurance++, f.b);
			vertOccurancesUnsorted.append(iOccurance++, f.c);
		}
	}
	if (m_hasTVerts == true) {
		for (int i = 0; i != nFaces; ++i) {
			const rmTVFace & tvf = mesh.getTVFace(oldFaceNumbers[i]);
			m_tVerts.push_back(tVerts + tvf.a);
			m_tVerts.push_back(tVerts + tvf.b);
			m_tVerts.push_back(tVerts + tvf.c);
		}
	}
	if (m_hasVNormals == true) {
		for (int i = 0; i != nFaces; ++i) {
			const rmVNFace & vnf = mesh.getVNFace(oldFaceNumbers[i]);
			m_vNormals.push_back(vNormals + vnf.a);
			m_vNormals.push_back(vNormals + vnf.b);
			m_vNormals.push_back(vNormals + vnf.c);
		}
	}
	if (m_hasVTangents == true) {
		for (int i = 0; i != nFaces; ++i) {
			const rmVTFace & vtf = mesh.getVTFace(oldFaceNumbers[i]);
			m_vTangents.push_back(vTangents + vtf.a);
			m_vTangents.push_back(vTangents + vtf.b);
			m_vTangents.push_back(vTangents + vtf.c);
		}
	}
	freeVector<rmU32>(oldFaceNumbers);

	rmSArr2<rmFVIndex> vertOccurances;
	vertOccurancesUnsorted.sort(vertOccurances);

	rmU32 nVertsNew = nIndices;

	vector<rmBool> skip;

	curListIterator = vertOccurances.getBuffer();
	rmFVIndex * endCurList = curListIterator;
	for (rmU32 i = 0; i != nVerts; ++i) {
		skip.clear();
		skip.resize(1000, false); // Allows up to 999 faces sharing one vert.
		endCurList += vertOccurances[i].getCount();
		
		for (; curListIterator != endCurList; ++curListIterator) {
			if (skip[endCurList - curListIterator]) {
				continue;
			}
			for (listCompareIterator = (endCurList - 1); listCompareIterator != curListIterator; --listCompareIterator) {
				if (faces[*curListIterator / 3].matId != faces[*listCompareIterator / 3].matId) {
					continue;
				}
				if ((*this.*tVertCompAction)() == false) {
					continue;
				}
				if ((*this.*vNormalCompAction)() == false) {
					continue;
				}
				if ((*this.*vTangentCompAction)() == false) {
					continue;
				}
				m_faces[*curListIterator / 3].verts[*curListIterator % 3] = *listCompareIterator;
				skip[endCurList - listCompareIterator] = true;
				if (m_hasVTangents) {
					m_averageVTangents.push_back(*m_vTangents[*curListIterator]);
					rmVec4 * t = &m_averageVTangents.back();
					t->vec3 += m_vTangents[*listCompareIterator]->vec3;
					m_vTangents[*curListIterator] = m_vTangents[*listCompareIterator] = t;
				}
				--faceSequenceInfosPerMat[m_faces[*curListIterator / 3].matId]->nUsedVerts;
				--nVertsNew;
				break;
			}
		}
	}
	freeVector<FaceSequenceInfo *>(faceSequenceInfosPerMat);

	vector<const rmPoint3 *> vertsNew;
	vector<const rmPoint3 *> tVertsNew;
	vector<const rmVec3 *> vNormalsNew;
	vector<const rmVec4 *> vTangentsNew;

	vertsNew.reserve(nVertsNew);
	m_hasTVerts ? tVertsNew.reserve(nVertsNew) : static_cast<void>(true);
	m_hasVNormals ? vNormalsNew.reserve(nVertsNew) : static_cast<void>(true);
	m_hasVTangents ? vTangentsNew.reserve(nVertsNew) : static_cast<void>(true);

	rmU32 * indexOffsetsNew = new rmU32 [nIndices];

	j = 0;
	for (rmU32 i = 0; i != nIndices; ++i) {
		if (m_faces[i / 3].verts[i % 3] != i) {
			continue;
		}
		indexOffsetsNew[i] = j++;
		vertsNew.push_back(m_verts[i]);
		m_hasTVerts ? tVertsNew.push_back(m_tVerts[i]) : static_cast<void>(true);
		m_hasVNormals ? vNormalsNew.push_back(m_vNormals[i]) : static_cast<void>(true);
		m_hasVTangents ? vTangentsNew.push_back(m_vTangents[i]) : static_cast<void>(true);
	}

	m_verts.swap(vertsNew);
	m_tVerts.swap(tVertsNew);
	m_vNormals.swap(vNormalsNew);
	m_vTangents.swap(vTangentsNew);

	j = 0;
	for (rmU32 i = 0; i != nFaces; ++i) {
		rmFace & f = m_faces[i];
		f.a = indexOffsetsNew[f.a];
		f.b = indexOffsetsNew[f.b];
		f.c = indexOffsetsNew[f.c];
		f.matId = faces[i].matId;
		f.sgs = faces[i].sgs;
	}

	if (m_hasVTangents) {
		rmVec4 ** newTangentPointers = new rmVec4 * [nIndices];
		memset(newTangentPointers, 0, nIndices * sizeof (rmU32 *));

		rmVec4 * startArea = &m_averageVTangents.front();
		rmVec4 * endArea = &m_averageVTangents.back();

		rmU32 nNewTangents = nIndices;
		for (rmU32 i = 0; i != nIndices; ++i) {
			const rmVec4 * t = m_vTangents[i];
			if ((t < startArea) || (t > endArea)) {
				--nNewTangents;
				continue;
			}
			else {
				if (newTangentPointers[t - startArea] == 0) {
					newTangentPointers[t - startArea] = startArea;
				}
				else {
					--nNewTangents;
				}
			}
		}
		memset(newTangentPointers, 0, nIndices * sizeof (rmU32 *));

		vector<rmVec4> newVTangents;
		newVTangents.reserve(nNewTangents);

		for (rmU32 i = 0; i != nIndices; ++i) {
			const rmVec4 *& t = m_vTangents[i];
			if ((t < startArea) || (t > endArea)) {
				continue;
			}
			else {
				if (newTangentPointers[t - startArea] == 0) {
					newTangentPointers[t - startArea] = startArea + newVTangents.size();
					newVTangents.push_back(*t);
					newVTangents.back().vec3.normalize();
				}
				t = newTangentPointers[t - startArea];
			}
		}

		m_averageVTangents.swap(newVTangents);
		delete [] newTangentPointers;
	}

	delete [] indexOffsetsNew;
}

rmOptimizedMesh::~rmOptimizedMesh(void) {}

const rmMesh * rmOptimizedMesh::getRefMesh(void) const
{
	return m_refMesh;
}

rmBool rmOptimizedMesh::hasTVerts(void) const
{
	return m_hasTVerts;
}

rmBool rmOptimizedMesh::hasVNormals(void) const
{
	return m_hasVNormals;
}

rmBool rmOptimizedMesh::hasVTangents(void) const
{
	return m_hasVTangents;
}

rmBool rmOptimizedMesh::tVertsMatch(void) const
{
	const rmPoint3 & tVert1 = *m_tVerts[*curListIterator];
	const rmPoint3 & tVert2 = *m_tVerts[*listCompareIterator];
	return ((rmRoughF32(0.0) == zeroAffineFMod((tVert1.x - tVert2.x), 1.0)) && (rmRoughF32(0.0) == zeroAffineFMod((tVert1.y - tVert2.y), 1.0)));
}

rmBool rmOptimizedMesh::tVertRefsMatch(void) const
{
	return (m_tVerts[*curListIterator] == m_tVerts[*listCompareIterator]);
}

rmBool rmOptimizedMesh::vNormalsMatch(void) const
{
	const rmVec3 & vNormal1 = *m_vNormals[*curListIterator];
	const rmVec3 & vNormal2 = *m_vNormals[*listCompareIterator];
	return ((rmRoughF32(vNormal1.x) == vNormal2.x) && (rmRoughF32(vNormal1.y) == vNormal2.y) && (rmRoughF32(vNormal1.z) == vNormal2.z));
}

rmBool rmOptimizedMesh::vNormalRefsMatch(void) const
{
	return (m_vNormals[*curListIterator] == m_vNormals[*listCompareIterator]);
}

rmBool rmOptimizedMesh::vTangentsMatch(void) const
{
	return ((m_vTangents[*curListIterator]->w == m_vTangents[*listCompareIterator]->w) && (rmVec3::cosOfAngle(m_vTangents[*curListIterator]->vec3, false, m_vTangents[*listCompareIterator]->vec3, false) > 0.0));
}

rmBool rmOptimizedMesh::noMatchingNeeded(void) const
{
	return true;
}

void rmOptimizedMesh::turnToMesh(rmMesh & target) const
{
	/*
	const rmPoint3 * verts = m_refMesh->getVertsContainer();
	const rmPoint3 * tVerts = m_refMesh->getTVertsContainer();
	const rmVec3 * vNormals = m_refMesh->getVNormalsContainer();
	const rmVec4 * vTangents = m_refMesh->getVTangentsContainer();
	rmU32 nMax = max(max(m_refMesh->getNumVerts(), m_refMesh->getNumTVerts()), max(m_refMesh->getNumVNormals(), m_refMesh->getNumVTangents()));
	rmU32 * newLocations = new rmU32 [nMax];
	// ToDp: Bulding up mesh
	delete [] newLocations;
	*/

	rmU32 nVerts = m_verts.size();
	rmU32 nFaces = m_faces.size();

	target.setNumVerts(nVerts);
	m_hasTVerts ? target.setNumTVerts(nVerts): static_cast<void>(true);
	m_hasVNormals ? target.setNumVNormals(nVerts) : static_cast<void>(true);
	m_hasVTangents ? target.setNumVTangents(nVerts) : static_cast<void>(true);

	target.setNumFaces(nFaces);

	rmPoint3 * verts = const_cast<rmPoint3 *>(target.getVertsContainer());
	rmPoint3 * tVerts = const_cast<rmPoint3 *>(target.getTVertsContainer());
	rmVec3 * vNormals = const_cast<rmVec3 *>(target.getVNormalsContainer());
	rmVec4 * vTangents = const_cast<rmVec4 *>(target.getVTangentsContainer());

	rmFace * faces = const_cast<rmFace *>(target.getFacesContainer());
	rmTVFace * tVFaces = const_cast<rmTVFace *>(target.getTVFacesContainer());
	rmVNFace * vNFaces = const_cast<rmVNFace *>(target.getVNFacesContainer());
	rmVTFace * vTFaces = const_cast<rmVTFace *>(target.getVTFacesContainer());

	for (rmU32 i = 0; i != nVerts; ++i) {
		verts[i] = *m_verts[i];
		if (m_hasTVerts) {
			tVerts[i] = *m_tVerts[i];
		}
		if (m_hasVNormals) {
			vNormals[i] = *m_vNormals[i];
		}
		if (m_hasVTangents) {
			vTangents[i] = *m_vTangents[i];
		}
	}

	for (rmU32 i = 0; i != nFaces; ++i) {
		faces[i] = m_faces[i];
		if (m_hasTVerts) {
			tVFaces[i] = m_faces[i];
		}
		if (m_hasVNormals) {
			vNFaces[i] = m_faces[i];
		}
		if (m_hasVTangents) {
			vTFaces[i] = m_faces[i];
		}
	}
}

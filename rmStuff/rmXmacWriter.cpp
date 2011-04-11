#include "rmXmacWriter.h"

// ErrorIdRange: 56739XXX
// Used: 56739001, 56739002, 56739003, 56739004

rmOut rmXmacWriter::out("rmXmacWriter.cpp");
rmNode * rmXmacWriter::node;
rmInFileStream * rmXmacWriter::inFileStream;
rmOutFileStream * rmXmacWriter::outFileStream;

void rmXmacWriter::overwriteVertices(string xmacFilePath, rmNode * node)
{
	rmMesh & m = *node->getMesh();
        try {
                inFileStream = new rmInFileStream(xmacFilePath);
        }
        catch (rmException & e) {
		OUT_SERR_E("Could not open \"" << xmacFilePath << "\".", 56739004)
        }
	rmInFileStream & is = *inFileStream;
	rmU8 * buffer = new rmU8 [is.getFileSize()];
	inFileStream->read(buffer, is.getFileSize());
        string tmpXmacFilePath = xmacFilePath + ".rmnew";
        try {
                outFileStream = new rmOutFileStream(tmpXmacFilePath);
        }
        catch (rmException & e) {
                OUT_SERR_E("Could not create \"" << tmpXmacFilePath << "\".", 56739001)
        }
	rmOutFileStream & os = *outFileStream;
	os.write(buffer,is.getFileSize());
	delete [] buffer;

	rmU32 xacFileSize;
	is.seek(136);
	is.read(xacFileSize);
	string s = is.readString(3);
	if (s != "XAC") {
		OUT_SERR(is.getFileName() << " is no valid ._xmac file")
	}
	is.seek(146);
	if (is.readByte() != 0) {
		is.bigEndianMode();
	}
	is.seek(148);

	vector<string> nodeNames;
	rmU32 vertsPosition = -1;
	rmU32 nVerts;
	rmU32 * baseVerts;

	while ((xacFileSize + 140) != is.tell()) {
		rmU32 secID = is.readLong();
		if (secID == 1) { // Mesh.
			rmU32 size = is.readLong();
			is.skip(4);
			if (nodeNames[is.readLong()] == node->getName()) {
				if (is.readLong() != m.getNumVerts()) {
					is.skip(-4);
                                        OUT_SERR_E("Wrong vertex count (Must be " << is.readLong() << " in this case)", 56739002)
				}
				nVerts = is.readLong();
				baseVerts = new rmU32 [nVerts];
				is.skip(16);
                                for (rmU32 i = 0; i != 2; ++i) {
					rmU32 subSecId = is.readLong();
					while ((subSecId != 0) && (subSecId != 5)) {
						is.skip(is.readLong() * nVerts + 4);
						subSecId = is.readLong();
					}
					if (subSecId == 0) {
						vertsPosition = is.tell() + 8;
						is.skip(is.readLong() * nVerts + 4);
					}
					else {
						is.skip(8);
						is.read(baseVerts, 4 * nVerts);
					}
				}
				break;
			}
			else {
				is.skip(size - 4);
			}
		}
		else if (secID == 11) { // Nodes.
			is.skip(8);
			rmU32 nNodes = is.readLong();
			is.skip(4);
			for (rmU32 i = 0; i < nNodes; ++i) {
				is.skip(156);
				nodeNames.push_back(is.readString(is.readLong()));
			}
		}
		else if (secID == 13) { // Mats.
			is.skip(8);
			rmU32 nMaterials = is.readLong();
			is.skip(8);
			for (rmU32 i = 0; i < nMaterials; ++i) {
				is.skip(95);
				rmU8 nMaps = is.readByte();
				is.skip(is.readLong());
				for (rmU32 j = 0; j < nMaps; ++j) {
					is.skip(28);
					is.skip(is.readLong());
				}
			}
		}
		else {
			is.skip(is.readLong() + 4);
		}
	}
	is.close();

        if (vertsPosition == (-1)) {
            OUT_SERR_E("No corresponding mesh found in xmac", 56739003)
        }

	os.seek(vertsPosition);
	for (rmU32 i = 0; i < nVerts; ++i) {
		rmPoint3 v = m.getVert(baseVerts[i]);
		os.writeFloat(v.x);
		os.writeFloat(v.z);
		os.writeFloat(v.y);
	}	
	os.close();

	delete [] baseVerts;
}

void rmXmacWriter::replaceMesh(string xmacFilePath, const rmScene & scene, rmU32 iNodeInScene)
{
	rmNode & node = scene.getNode(iNodeInScene);
	rmMesh & mesh = *node.getMesh();

	rmInFileStream xmac;
	rmOutFileStream newXmac;

	try {
		xmac.open(xmacFilePath);
	}
        catch (rmException & e) {
		OUT_SERR_E("Could not open \"" << xmacFilePath << "\".", 56739004)
	}
	try {
                newXmac.open(xmacFilePath.append(".rmnew"));
	}
        catch (rmException & e) {
                OUT_SERR_E("Could not create \"" << xmacFilePath << "\".", 56739001)
	}

	// Retrieving the needed information from existing XMAC file.
	// The following code is copied and modified from 'rmXmacReader.cpp'.

	rmU32 nNodes = 0;
	rmU32 iNode = 0;
	rmU32 matCountOffset = 0;
	vector<string> materialNames;
	rmU32 endMaterialsSectionOffset = 0;
	rmU32 correspondingMeshSectionOffset = 0;
	rmU32 nOriginalVerts = 0;
	rmU32 nOriginalBaseVerts = 0;
	rmU32 specialDword = 0;
	vector<rmU32> originalBaseVertReferences;
	vector<rmPoint3> originalVerts;
	rmU32 endCorrespondingMeshSectionOffset = 0;
	rmU32 correspondingSkinSectionOffset = 0;
	rmU32 iSkin = -1;
	vector<rmWeighting> weightings;
	vector<rmVertWeightingsInfo> vertWeightingsInfos; 
	rmU32 endCorrespondingSkinSectionOffset = 0;
	rmU32 newXacFileSize;

	rmU32 xacFileSize;
	xmac.seek(136);
	xmac.read(xacFileSize);
	string s = xmac.readString(3);
	if (s != "XAC") {
		OUT_SERR(xmac.getFileName() << " is no valid ._xmac file")
	}
	xmac.seek(146);
	if (xmac.readByte() != 0) {
		xmac.bigEndianMode();
		newXmac.bigEndianMode();
	}
	xmac.seek(148);

	while ((xacFileSize + 140) != xmac.tell()) {
		rmU32 secID = xmac.readLong();
		if (secID == 1) { // Mesh.
			rmU32 size = xmac.readLong();
			rmU32 sectionEnd = (xmac.tell() + size + 4);
			xmac.skip(4);
			rmU32 iCurNode = xmac.readLong();
			if (iCurNode != iNode) {
				xmac.seek(sectionEnd);
				continue;
			}
			correspondingMeshSectionOffset = (xmac.tell() - 16);
			correspondingSkinSectionOffset = endCorrespondingSkinSectionOffset = endCorrespondingMeshSectionOffset = sectionEnd;
			nOriginalBaseVerts = xmac.readLong();
			nOriginalVerts = xmac.readLong();
			originalBaseVertReferences.resize(nOriginalVerts);
			originalVerts.resize(nOriginalVerts);
			xmac.skip(12);
			xmac.read(specialDword);
			rmBool readVertsSection = false;
			rmBool readBaseVertsSection = false;
			while (sectionEnd != xmac.tell()) {
				rmU32 subSecID = xmac.readLong();
				rmU32 subSecBlockSize = xmac.readLong();
				xmac.skip(4);
				if ((subSecID == 0) && (subSecBlockSize == 12)) { // Vertices.
					//xmac.read(&originalVerts[0], (originalVerts.size() * sizeof(rmPoint3)));
					rmPoint3 * vertsContainer = &originalVerts[0];
					rmU32 vCSize = originalVerts.size();
					for (rmU32 i = 0; i != vCSize; ++i) {
						xmac.read(vertsContainer[i].x);
						xmac.read(vertsContainer[i].z);
						xmac.read(vertsContainer[i].y);
					}
					readVertsSection = true;
					if (readBaseVertsSection) {
						xmac.seek(sectionEnd);
					}
				}
				else if ((subSecID == 5) && (subSecBlockSize == 4)) { // Indicates which vertices are printed more than one time.
					//xmac.read(&originalBaseVertReferences[0], (originalBaseVertReferences.size() * sizeof(rmU32)));
					rmU32 * baseVertRefsContainer = &originalBaseVertReferences[0];
					rmU32 bVRCSize = originalBaseVertReferences.size();
					for (rmU32 i = 0; i != bVRCSize; ++i) {
						xmac.read(baseVertRefsContainer[i]);
					}
					readBaseVertsSection = true;
					if (readVertsSection) {
						xmac.seek(sectionEnd);
					}
				}
				else {
					xmac.skip(nOriginalVerts * subSecBlockSize);
				}
			}
		}
		else if (secID == 2) { // Weighting.
			rmU32 size = xmac.readLong();
			xmac.skip(4);
			if (xmac.readLong() != iNode) {
				xmac.skip(size - 4);
				continue;
			}
			correspondingSkinSectionOffset = (xmac.tell() - 16);
			xmac.skip(4);
			vertWeightingsInfos.resize(nOriginalBaseVerts);
			weightings.resize(xmac.readLong());
			iSkin = xmac.readLong();
			//xmac.read(&weightings[0], weightings.size() * sizeof(rmWeighting));
			rmWeighting * weightsContainer = &weightings[0];
			rmU32 wCSize = weightings.size();
			for (rmU32 i = 0; i != wCSize; ++i) {
				xmac.read(weightsContainer[i].weight);
				xmac.read(weightsContainer[i].iBone);
				xmac.skip(2);
			}
			//xmac.read(&vertWeightingsInfos[0], vertWeightingsInfos.size() * sizeof(rmVertWeightingsInfo));
			rmVertWeightingsInfo * vertWeightsInfosContainer = &vertWeightingsInfos[0];
			rmU32 vWICSize = vertWeightingsInfos.size();
			for (rmU32 i = 0; i != vWICSize; ++i) {
				xmac.read(vertWeightsInfosContainer[i].iFirstWeighting);
				xmac.read(vertWeightsInfosContainer[i].nWeightings);
			}
			endCorrespondingSkinSectionOffset = xmac.tell();
		}
		else if (secID == 7) { // General scene information.
			xmac.skip(xmac.readLong() + 4);
		}
		else if (secID == 11) { // Nodes.
			string nodeName = node.getName();
			rmU32 size = xmac.readLong();
			rmU32 sectionEnd = (xmac.tell() + size + 4);
			xmac.skip(4);
			nNodes = xmac.readLong();
			xmac.skip(4);
			for (; iNode != nNodes; ++iNode) {
				xmac.skip(39 * sizeof(rmF32));
				if (nodeName == xmac.readString(xmac.readLong())) {
					xmac.seek(sectionEnd);
					break;
				}
			}
			if (iNode == nNodes) {
                                OUT_SERR_E("No corresponding mesh found in xmac", 56739003)
			}
		}
		else if (secID == 13) { // Mats.
			xmac.skip(8);
			matCountOffset = xmac.tell();
			rmU32 nMaterials = xmac.readLong();
			xmac.skip(8);
			for (rmU32 i = 0; i < nMaterials; ++i) {
				xmac.skip(95);
				rmU32 nMaps = xmac.readByte();
				materialNames.push_back(xmac.readString(xmac.readLong()));
				for (rmU32 j = 0; j < nMaps; ++j) {
					xmac.skip(28);
					xmac.skip(xmac.readLong());
				}
			}
			endMaterialsSectionOffset = xmac.tell();
		}
		else {
			OUT_SERR("Unknown XAC Section identifier (" << secID << ") in " << xmac.getFileName() << " at position " << (xmac.tell() - 4))
		}
	}

	if (correspondingMeshSectionOffset == 0) {
                OUT_SERR_E("No corresponding mesh found in xmac", 56739003) // Corresponding node has no mesh.
	}

	rmU32 nSubSections = (mesh.getNumTVerts() == 0) ? 3 : 5;
	rmBool vNormalsCalculated = false;
	if (mesh.getNumVNormals() == 0) {
		mesh.calcVNormalsByAngle(45.0);
		vNormalsCalculated = true;
	}
	rmOptimizedMesh oMesh(mesh, false, vNormalsCalculated);
	rmU32 nNewVerts = oMesh.verts.size();

	vector<rmPoint3> originalBaseVerts;
	originalBaseVerts.resize(nOriginalBaseVerts);
	for (vector<rmU32>::iterator i = originalBaseVertReferences.begin(), ib = i, ie = originalBaseVertReferences.end(); i != ie; ++i) {
		originalBaseVerts[*i] = originalVerts[i - ib];
	}

	rmU32 newMeshSectionSize = (oMesh.faceSequenceInfos.size() * 16 + oMesh.faces.size() * 12) + ((nSubSections * 12 - 8) * (oMesh.verts.size() + 1)) + 36;
	vector< vector<rmU32> > usedBonesPerMat;
	vector<rmU32> skinReferenceVerts;
	if (iSkin >= 0) {
		rmNearestNeighbor::findNearestTargetVerts(mesh.getVertsContainer(), mesh.getNumVerts(), originalBaseVerts.data(), originalBaseVerts.size(), skinReferenceVerts);

		rmBool * usedBones = new rmBool [nNodes];
		const rmFace * f = &oMesh.faces.front();
		const rmPoint3 * baseVertsContainer = mesh.getVertsContainer();

		usedBonesPerMat.resize(oMesh.faceSequenceInfos.size());

		for (rmU32 i = 0; i != oMesh.faceSequenceInfos.size(); ++i) {
			memset(usedBones, 0, nNodes * sizeof(rmBool));
			const rmOptimizedMesh::FaceSequenceInfo & fSI = oMesh.faceSequenceInfos[i];
			for (const rmFace * fe = f + fSI.count; f != fe; ++f) {
				for (rmU32 j = 0; j != 3; ++j) {
					rmVertWeightingsInfo & vWI = vertWeightingsInfos[skinReferenceVerts[oMesh.verts[f->verts[j]] - baseVertsContainer]];
					for (const rmWeighting * w = &weightings[vWI.iFirstWeighting], * we = (w + vWI.nWeightings); w != we; ++w) {
						usedBones[w->iBone] = true;
					}
				}
			}
			for (rmU32 j = 0; j != nNodes; ++j) {
				if (usedBones[j] == true) {
					usedBonesPerMat[i].push_back(j);
				}
			}
			newMeshSectionSize += (usedBonesPerMat[i].size() * 4);
		}
		delete [] usedBones;
	}

	rmU32 nSceneMats = scene.getNumMaterials();
	vector<string> newMaterialNames;
	vector<string> newDiffuseMaps;
	vector<rmU32> newMatIds;
	for (rmU32 i = 0; i != nSceneMats; ++i) {
		rmMaterial & mat = scene.getMat(i);
		string matName = mat.getName();
		rmU32 newMatId = materialNames.size();
		for (rmU32 j = 0; j != newMatId; ++j) {
			if (materialNames[j] == matName) {
				newMatId = j;
				break;
			}
		}
		if (newMatId == materialNames.size()) {
			newMatId += newMaterialNames.size();
			newMaterialNames.push_back(mat.getName());
			rmMap * map = mat.getDiffuseMap();
			newDiffuseMaps.push_back((map != 0) ? map->getBitmapPath() : "");
		}
		newMatIds.push_back(newMatId);
	}

	rmU32 bufferSize = 0;
	rmU8 * buffer = 0;

	xmac.seek(0);
	bufferSize = matCountOffset;
	buffer = new rmU8 [bufferSize];
	xmac.read(buffer, bufferSize);
	newXmac.write(buffer, bufferSize);
	rmU32 nTotalMatsNew = (xmac.readLong() + newMaterialNames.size());
	xmac.skip(4);
	newXmac.writeLong(nTotalMatsNew);
	newXmac.writeLong(nTotalMatsNew);
	delete [] buffer;

	bufferSize = (endMaterialsSectionOffset - matCountOffset - 8);
	buffer = new rmU8 [bufferSize];
	xmac.read(buffer, bufferSize);
	newXmac.write(buffer, bufferSize);
	delete [] buffer;

	for (rmU32 i = 0; i != newMaterialNames.size(); ++i) {
		string & matName = newMaterialNames[i];
		string & diffuseMapPath = newDiffuseMaps[i];
		newXmac.writeLong(3);
                newXmac.writeLong(88 + matName.size());
		newXmac.writeLong(2);
		newXmac.writeFloat(0); newXmac.writeFloat(0); newXmac.writeFloat(0); newXmac.writeFloat(1); newXmac.writeFloat(0.75); newXmac.writeFloat(0.75); newXmac.writeFloat(0.75);
		newXmac.writeFloat(1); newXmac.writeFloat(1); newXmac.writeFloat(1); newXmac.writeFloat(1); newXmac.writeFloat(1); newXmac.writeFloat(0); newXmac.writeFloat(0);
		newXmac.writeFloat(0); newXmac.writeFloat(1); newXmac.writeFloat(25); newXmac.writeFloat(0); newXmac.writeFloat(1); newXmac.writeFloat(1.5);
		newXmac.writeShort(0);
		newXmac.writeByte(70);
		newXmac.writeByte((diffuseMapPath != "") ? 1 : 0);
		newXmac.writeLong(matName.size());
		newXmac.writeString(matName);
		if (diffuseMapPath != "") {
			newXmac.writeFloat(1); newXmac.writeFloat(0); newXmac.writeFloat(0); newXmac.writeFloat(1); newXmac.writeFloat(1); newXmac.writeFloat(0);
			newXmac.writeShort(1);
			newXmac.writeShort(2); // Map type.
			newXmac.writeLong(diffuseMapPath.size());
			newXmac.writeString(diffuseMapPath);
		}
	}

	bufferSize = (correspondingMeshSectionOffset - endMaterialsSectionOffset);
	buffer = new rmU8 [bufferSize];
	xmac.read(buffer, bufferSize);
	xmac.seek(endCorrespondingMeshSectionOffset);
	newXmac.write(buffer, bufferSize);
	delete [] buffer;

	//
	// Mesh section.
	//

	newXmac.writeLong(1);
	rmU32 meshSectionSizeOffset = newXmac.tell();
	newXmac.writeLong(newMeshSectionSize);
	newXmac.writeLong(1);

	newXmac.writeLong(iNode);
	newXmac.writeLong(mesh.getNumVerts());
	newXmac.writeLong(oMesh.verts.size());
	newXmac.writeLong(oMesh.faces.size() * 3);

	newXmac.writeLong(oMesh.faceSequenceInfos.size());
	newXmac.writeLong(nSubSections);
	newXmac.writeLong(specialDword);

	// Base verts section.
	newXmac.writeLong(5); // Id.
	newXmac.writeLong(4); // Block size.
	newXmac.writeShort(0); // In this sub-section always 0.
	newXmac.writeShort(37913); // Just any number.
	const rmPoint3 * vertsContainer = mesh.getVertsContainer();
	for (rmU32 i = 0; i != nNewVerts; ++i) {
		newXmac.writeLong(oMesh.verts[i] - vertsContainer);
	}

	// Vertices section.
	newXmac.writeLong(0); // Id.
	newXmac.writeLong(12); // Block size.
	newXmac.writeShort(1); // In this sub-section always 1.
	newXmac.writeShort(37913); // Just any number.
	for (rmU32 i = 0; i != nNewVerts; ++i) {
		const rmPoint3 & v = *oMesh.verts[i];
		newXmac.writeFloat(v.x);
		newXmac.writeFloat(v.z);
		newXmac.writeFloat(v.y);
	}

	// Normals section.
	newXmac.writeLong(1); // Id.
	newXmac.writeLong(12); // Block size.
	newXmac.writeShort(1); // In this sub-section always 1.
	newXmac.writeShort(37913); // Just any number.
	for (rmU32 i = 0; i != nNewVerts; ++i) {
		const rmVec3 & vn = *oMesh.vNormals[i];
		newXmac.writeFloat(vn.x);
		newXmac.writeFloat(vn.z);
		newXmac.writeFloat(vn.y);
	}

	// Texture verts section.
	if (nSubSections == 5) {
		newXmac.writeLong(3); // Id.
		newXmac.writeLong(8); // Block size.
		newXmac.writeShort(0); // In this sub-section always 0.
		newXmac.writeShort(37913); // Just any number.
		for (rmU32 i = 0; i != nNewVerts; ++i) {
			const rmPoint3 & tv = *oMesh.tVerts[i];
			newXmac.writeFloat(tv.x);
			newXmac.writeFloat(tv.y * (-1));
		}
	}

	// Tangents section.
	if (nSubSections == 5) {
		newXmac.writeLong(2); // Id.
		newXmac.writeLong(16); // Block size.
		newXmac.writeShort(1); // In this sub-section always 1.
		newXmac.writeShort(37913); // Just any number.
		for (rmU32 i = 0; i != nNewVerts; ++i) {
			//const rmVec4 & vt = *oMesh.vTangents[i];
			newXmac.writeFloat(0.0); // Apparently they are recalculated in the game.
			newXmac.writeFloat(0.0);
			newXmac.writeFloat(1.0);
			newXmac.writeFloat(1.0);
		}
	}

	// Face lists.
	rmU32 nVertsPassed = 0;
	const rmFace * f = oMesh.faces.data();
	for (rmU32 i = 0; i != oMesh.faceSequenceInfos.size(); ++i) {
		const rmOptimizedMesh::FaceSequenceInfo & fSI = oMesh.faceSequenceInfos[i];
		const vector<rmU32> & usedBones = usedBonesPerMat[fSI.matId];
		rmU32 nUsedBones = usedBones.size();
		newXmac.writeLong(fSI.count * 3);
		newXmac.writeLong(fSI.nUsedVerts);
		newXmac.writeLong(newMatIds[fSI.matId]);
		newXmac.writeLong(nUsedBones);
		for (rmU32 j = 0; j != fSI.count; ++j) {
			newXmac.writeLong(f->c - nVertsPassed);
			newXmac.writeLong(f->b - nVertsPassed);
			newXmac.writeLong(f->a - nVertsPassed);
			++f;
		}
		for (rmU32 j = 0; j != nUsedBones; ++j) {
			newXmac.writeLong(usedBones[j]);
		}
		nVertsPassed += fSI.nUsedVerts;
	}

	newXacFileSize = (xacFileSize + newXmac.tell() - endCorrespondingMeshSectionOffset);

	bufferSize = (correspondingSkinSectionOffset - endCorrespondingMeshSectionOffset);
	buffer = new rmU8 [bufferSize];
	xmac.read(buffer, bufferSize);
	xmac.seek(endCorrespondingSkinSectionOffset);
	newXmac.write(buffer, bufferSize);
	delete [] buffer;

	//
	// Skin section.
	//

	if (iSkin >= 0) {
		// Composing new skin section
		rmU32 nBaseVerts = mesh.getNumVerts();
		rmU32 nWeightings = 0;
		for (rmU32 i = 0; i != nBaseVerts; ++i) {
			nWeightings += vertWeightingsInfos[skinReferenceVerts[i]].nWeightings;
		}

		rmU32 nUsedBones = 0;
		vector<rmBool> isUsed;
		isUsed.resize(nNodes, false);
		for (vector<rmWeighting>::iterator i = weightings.begin(), ie = weightings.end(); i != ie; ++i) {
			if (isUsed[i->iBone] == false) {
				isUsed[i->iBone] = true;
				++nUsedBones;
			}
		}

		newXmac.writeLong(2);
		newXmac.writeLong(16 + nWeightings * sizeof(rmWeighting) + mesh.getNumVerts() * sizeof(rmVertWeightingsInfo));
		newXmac.writeLong(3);
		newXmac.writeLong(iNode);
		newXmac.writeLong(nUsedBones);
		newXmac.writeLong(nWeightings);
		newXmac.writeLong(iSkin);

		rmU32 iWeighting = 0;
		vector<rmVertWeightingsInfo> newVertWeightsInfos;
		newVertWeightsInfos.resize(skinReferenceVerts.size());

		for (vector<rmU32>::iterator i = skinReferenceVerts.begin(), ib = i, ie = skinReferenceVerts.end(); i != ie; ++i) {
			rmVertWeightingsInfo & vWInfo = vertWeightingsInfos[*i];
			newVertWeightsInfos[i - ib].iFirstWeighting = iWeighting;
			newVertWeightsInfos[i - ib].nWeightings = vWInfo.nWeightings;
			iWeighting += vWInfo.nWeightings;
			rmWeighting * w = &weightings[vWInfo.iFirstWeighting];
			for (rmU32 j = 0; j != vWInfo.nWeightings; ++j) {
				newXmac.writeFloat(w->weight);
				newXmac.writeShort(w->iBone);
				newXmac.writeShort(528);
				++w;
			}
		}

		for (vector<rmVertWeightingsInfo>::iterator i = newVertWeightsInfos.begin(), ie = newVertWeightsInfos.end(); i != ie; ++i) {
			newXmac.writeLong(i->iFirstWeighting);
			newXmac.writeLong(i->nWeightings);
		}

		newXacFileSize = (xacFileSize + newXmac.tell() - endCorrespondingSkinSectionOffset);
	}

	bufferSize = (xmac.getFileSize() - endCorrespondingSkinSectionOffset);
	buffer = new rmU8 [bufferSize];
	xmac.read(buffer, bufferSize);
	newXmac.write(buffer, bufferSize);
	delete [] buffer;

	rmU32 newFileSize = newXmac.tell();
	newXmac.seek(20);
	newXmac.writeLong(newFileSize - 136);
	newXmac.seek(136);
	newXmac.writeLong(newXacFileSize);
}

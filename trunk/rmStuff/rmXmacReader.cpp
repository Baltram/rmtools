#include "rmXmacReader.h"

rmOut rmXmacReader::out = rmOut("rmXmac.cpp");
rmInFileStream * rmXmacReader::fileStream = 0;
rmScene * rmXmacReader::scene = 0;
rmMesh * rmXmacReader::curMesh = 0;
rmU32 * rmXmacReader::baseVertIndices = 0;
rmU32 rmXmacReader::nVertices = 0;
vector<PosDescriptor> rmXmacReader::info;
PosDescriptor rmXmacReader::pDesc;

void rmXmacReader::readXmacFileToScene(rmInFileStream * fileStream, rmScene * scene, vector<PosDescriptor> * getInfo)
{
	info.clear();
	delete [] baseVertIndices;
	baseVertIndices = 0;
	rmXmacReader::fileStream = fileStream;
	rmXmacReader::scene = scene;

	rmInFileStream & fs = *fileStream;

	rmU32 xacFileSize;

	fs.seek(136);
	fs.read(xacFileSize);

	string s = fs.readString(3);
	if (s != "XAC") {
		OUT_SERR(fs.getFileName() << " is no valid ._xmac file")
	}
	fs.seek(146);
	if (fs.readByte() != 0) {
		fs.bigEndianMode();
	}

	fs.seek(148);

	while ((xacFileSize + 140) != fs.tell()) {
		rmU32 secID = fs.readLong();
		if (secID == 1) { // Mesh.
			pDesc.description = "Mesh section";
			pDesc.position = (fs.tell() - 4);
			info.push_back(pDesc);
			readMeshSection();
		}
		else if (secID == 2) { // Weighting.
			pDesc.description = "Weightings section";
			pDesc.position = (fs.tell() - 4);
			info.push_back(pDesc);
			readWeightingSection();
		}
		else if (secID == 7) { // General scene information.
			pDesc.description = "Info section";
			pDesc.position = (fs.tell() - 4);
			info.push_back(pDesc);
			readInfoSection();
		}
		else if (secID == 11) { // Nodes.
			pDesc.description = "Nodes section";
			pDesc.position = (fs.tell() - 4);
			info.push_back(pDesc);
			readNodesSection();
		}
		else if (secID == 13) { // Mats.
			pDesc.description = "Materials section";
			pDesc.position = (fs.tell() - 4);
			info.push_back(pDesc);
			readMatsSection(); 
		}
		// (secID == 3) would belong to "MatSection" but all of them are read by readMatsSection().
		else {
			return; //$
			OUT_SERR("Unknown XAC Section identifier (" << secID << ") in " << fs.getFileName() << " at position " << (fs.tell() - 4))
		}
	}

	if (getInfo != 0) {
		info.swap(*getInfo);
	}
}

void rmXmacReader::readMeshSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmU32 size = fs.readLong();
	rmU32 sectionEnd = (fs.tell() + size + 4);
	fs.skip(4);
	rmU32 nodeNum = fs.readLong();
	rmMesh tmp;
	scene->getNode(nodeNum).setMesh(tmp);
	curMesh = scene->getNode(nodeNum).getMesh();

	pDesc.description = "Mesh information";
	pDesc.position = fs.tell();
	info.push_back(pDesc);

	rmU32 nBaseVerts = fs.readLong();
	nVertices = fs.readLong();
	rmU32 nIndices = fs.readLong();
	rmU32 nFaces = (nIndices / 3);
	if (USE_BASEVERTS) {
		curMesh->setNumVerts(nBaseVerts);
	}
	else {
		curMesh->setNumVerts(nVertices);
	}
	curMesh->setNumFaces(nFaces);

	fs.skip(22);
	rmU16 magicValue = fs.readShort();
	fs.skip(-12);

	while (sectionEnd != fs.tell()) {
		fs.skip(10);
		rmU16 mvCheck = fs.readShort();
		fs.skip(-12);

		if (mvCheck == magicValue) {
			rmU32 subSecID = fs.readLong();
			rmU32 subSecBlockSize = fs.readLong();
			fs.skip(4);
			if ((subSecID == 0) && (subSecBlockSize == 12)) { // Vertices.
				pDesc.description = "Vertices subsection";
				pDesc.position = (fs.tell() - 12);
				info.push_back(pDesc);
				readVertsSubSection();
			}
			else if ((subSecID == 1) && (subSecBlockSize == 12)) { // (V)Normals.
				pDesc.description = "Normals subsection";
				pDesc.position = (fs.tell() - 12);
				info.push_back(pDesc);
				readNormalsSubSection();
			}
			else if ((subSecID == 2) && (subSecBlockSize == 16)) { // (V)Tangents.
				pDesc.description = "Tangents subsection";
				pDesc.position = (fs.tell() - 12);
				info.push_back(pDesc);
				readTangentsSubSection();
			}
			else if ((subSecID == 3) && (subSecBlockSize == 8)) { // Texture vertices.
				pDesc.description = "TVerts subsection";
				pDesc.position = (fs.tell() - 12);
				info.push_back(pDesc);
				readTVertsSubSection();
			}
			else if ((subSecID == 5) && (subSecBlockSize == 4)) { // Indicates which vertices are printed more than one time.
				pDesc.description = "Base verts subsection";
				pDesc.position = (fs.tell() - 12);
				info.push_back(pDesc);
				delete [] baseVertIndices;
				baseVertIndices = new rmU32 [nVertices];
				readBaseVertsSubSection();
			}
			else {
				OUT_SWARN("Potentially unknown XAC mesh sub-section identifier / block size (ID: " << subSecID << ", block size: " << subSecBlockSize <<") in " << fs.getFileName() << " at position " << (fs.tell() - 12) << endl << "Sub-section will be considered as face indices sub-section")
				fs.skip(-12);
				readFacesSubSection(); // mv is sometimes 0
			}
		}
		else {
			readFacesSubSection();
		}
	}

	delete [] baseVertIndices;
	baseVertIndices = 0;
}

void rmXmacReader::readWeightingSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmU32 size = fs.readLong();
	fs.skip(size + 4); // ToDo: weightings import.
}

void rmXmacReader::readInfoSection(void)
{
	// No important information so just skip this section.
	rmInFileStream & fs = *fileStream;
	rmU32 size = fs.readLong();
	fs.skip(size + 4);
}

void rmXmacReader::readNodesSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmU32 size = fs.readLong();
	fs.skip(4);
	rmU32 nNodes = fs.readLong();
	fs.skip(4);

	for (rmU32 i = 0; i < nNodes; ++i) {
		rmF32 transform[39]; // 8, 25, 29, 33, 34, and 36 are candidates for pos(x, y, z).
		for (rmU32 j = 0; j < 39; ++j) {
			fs.read(transform[j]);
		}
		rmNode newNode(fs.readString(fs.readLong()));
		scene->addNode(&newNode);
	}
}

void rmXmacReader::readMatsSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmU32 size = fs.readLong(); // 12
	fs.skip(4);
	rmU32 nMaterials = fs.readLong();
	rmU32 control = fs.readLong(); //$
	if (nMaterials != control) {
		OUT_SERR("Two different numbers for material count in " << fs.getFileName() << " at " << (fs.tell() - 8))
	}
	fs.skip(4); //$$8
	for (rmU32 i = 0; i < nMaterials; ++i) {
		fs.skip(95);
		rmU8 nMaps = fs.readByte();
		rmMaterial mat(fs.readString(fs.readLong()));
		for (rmU32 j = 0; j < nMaps; ++j) {
			fs.skip(26);
			rmU16 xmacMapType = fs.readByte();
			fs.skip(1);
			rmChar buf[4];
			itoa((j + 1), buf, 10);
			rmMap map((string("Map Nr. ").append(string(buf))), fs.readString(fs.readLong()).append(string(".jpg")));
			if (xmacMapType == 2) { // Diffuse.
				mat.setDiffuseMap(map);
			}
			else if (xmacMapType == 3) { // Specular.
				mat.setSpecularMap(map);
			}
			else if (xmacMapType == 5) { // Normal.
				mat.setNormalMap(map);
			}
			else {
				OUT_SWARN("Unknown map type (" << xmacMapType << ") in " << fs.getFileName() << " at position " << (fs.tell() - 2 - map.getBitmapPath().size()) << "Map won't be considered.")
			}
		}
		scene->addMat(&mat);
	}
}

void rmXmacReader::readVertsSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmMesh & m = *curMesh;
	rmPoint3 p;
	for (rmU32 i = 0; i < nVertices; ++i) {
		fs.read(p.x);
		fs.read(p.z);
		fs.read(p.y);
		if (USE_BASEVERTS) {
			m.setVert(baseVertIndices[i], p);
		}
		else {
			m.setVert(i, p);
		}
	}
}

void rmXmacReader::readNormalsSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmMesh & m = *curMesh;
	rmVec3 n;
	m.setNumVNormals(nVertices);
	for (rmU32 i = 0; i < nVertices; ++i) {
		fs.read(n.x);
		fs.read(n.z);
		fs.read(n.y);
		m.setVNormal(i, n);
	}
}

void rmXmacReader::readTangentsSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmMesh & m = *curMesh;
	rmVec4 t;
	m.setNumVTangents(nVertices);
	for (rmU32 i = 0; i < nVertices; ++i) {
		fs.read(t.vec3.x);
		fs.read(t.vec3.z);
		fs.read(t.vec3.y);
		fs.read(t.w);
		t.w *= (-1);
		m.setVTangent(i, t);
	}
}

void rmXmacReader::readTVertsSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmMesh & m = *curMesh;
	rmPoint3 tv(0, 0, 0);
	m.setNumTVerts(nVertices);
	for (rmU32 i = 0; i < nVertices; ++i) {
		fs.read(tv.x);
		fs.read(tv.y);
		tv.y *= (-1);
		m.setTVert(i, tv);
	}
}

void rmXmacReader::readBaseVertsSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	for (rmU32 i = 0; i < nVertices; ++i) {
		fs.read(baseVertIndices[i]);
	}
}

void rmXmacReader::readFacesSubSection(void)
{
	rmInFileStream & fs = *fileStream;
	rmMesh & m = *curMesh;
	rmU32 nStrangeDWords = 0;
	rmU32 nFacesToRead = m.getNumFaces();
	rmU32 nFacesRead = 0;
	rmU32 nFacesReadAfter = 0;
	rmU32 nVertsPassed = 0;
	rmU32 nVertsPassedAfter = 0;
	rmFace f(0, 0, 0, 0, 0);
	rmU32 i = 0;
	rmChar buffer [20];

	while (nFacesRead != nFacesToRead) {
		pDesc.description = "Face list ";
		pDesc.description.append(itoa(i++, buffer, 10));
		pDesc.position = fs.tell();
		info.push_back(pDesc);

		nFacesReadAfter += (fs.readLong() / 3);
		nVertsPassedAfter += fs.readLong();
		f.matId = fs.readLong();
		fs.read(nStrangeDWords);

		for (; nFacesRead != nFacesReadAfter; ++nFacesRead) {
			f.c = (fs.readLong() + nVertsPassed);
			f.b = (fs.readLong() + nVertsPassed);
			f.a = (fs.readLong() + nVertsPassed);
			m.setFace(nFacesRead, f);
		}

		nVertsPassed = nVertsPassedAfter;
		fs.skip(nStrangeDWords * sizeof(rmU32));
	}

	if (m.getNumTVerts() != 0) {
		m.copyFacesToTVFaces();
	}
	if (m.getNumVNormals() != 0) {
		m.copyFacesToVNFaces();
	}
	if (m.getNumVTangents() != 0) {
		m.copyFacesToVTFaces();
	}
	if (USE_BASEVERTS) {
		for (rmU32 i = 0; i < nFacesToRead; ++i) {
			f = m.getFace(i);
			f.a = baseVertIndices[f.a];
			f.b = baseVertIndices[f.b];
			f.c = baseVertIndices[f.c];
			m.setFace(i, f);
		}
	}
}


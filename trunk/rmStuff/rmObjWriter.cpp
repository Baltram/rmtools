#include "rmObjWriter.h"

rmOut rmObjWriter::out = rmOut("rmXmac.cpp");

rmScene * rmObjWriter::scene = 0;
rmOutFileStream * rmObjWriter::fileStream = 0;

rmU32 rmObjWriter::curMatId = 0;

void rmObjWriter::writeSceneToObjFile(rmOutFileStream * fileStream, rmScene * scene, rmU32 nodeIndex)
{
	rmObjWriter::scene = scene;
	rmObjWriter::fileStream = fileStream;
	rmOutFileStream & fs = *fileStream;
	
	if (scene->getNumMaterials() > 0) {
		fs << "mtllib " << scene->getName() << ".mtl" << newLine;
	}

	rmNode & n = scene->getNode(nodeIndex);
	rmMesh * m = n.getMesh();
	fs << newLine << "o " << n.getName() << newLine << newLine;
	writeMesh(*m);
};

void rmObjWriter::writeSceneToMtlFile(rmOutFileStream * fileStream, rmScene * scene) 
{
	rmU32 nMaterials = scene->getNumMaterials();
	rmOutFileStream & mtl = *fileStream;

	for (rmU32 i = 0; i < nMaterials; ++i) {
		rmMaterial & m = scene->getMat(i);
		rmMap * map = m.getDiffuseMap();
		string name = m.getName();
		
		/*rmI32 firstSpace = name.find_first_of(" ");
		while (firstSpace >= 0) {
			name.replace(firstSpace, 1, "_", 1);
			firstSpace = name.find_first_of(" ");
		}*/

		mtl << "newmtl " << name << newLine;
		if (map != 0) {
			mtl << "map_Kd " << map->getBitmapPath() << newLine;
		}
		mtl << newLine;
	}
}

void rmObjWriter::writeMesh(rmMesh & m)
{
	rmBool hasMaterials = (scene->getNumMaterials() > 0);
	rmOutFileStream & fs = *fileStream;
	rmU32 nVerts = m.getNumVerts();
	rmU32 nFaces = m.getNumFaces();
	rmU32 nTVerts = m.getNumTVerts();
	rmU32 nVNormals = m.getNumVNormals();
	curMatId = -1;

	// vertices
	if (nVerts != 0) {
		rmPoint3 v;
		for (rmU32 i = 0; i < nVerts; ++i) {
			v = m.getVert(i);
			fs << "v " << v.x << " " << v.z << " " << ((-1) * v.y) << newLine;
		}
		fs << newLine;
	}

	// texture vertices
	if (nTVerts != 0) {
		rmPoint3 tv;
		for (rmU32 i = 0; i < nTVerts; ++i) {
			tv = m.getTVert(i);
			fs << "vt " << tv.x << " " << tv.y << " " << tv.z << newLine;
		}
		fs << newLine;
	}

	// vertex normals
	if (nVNormals != 0) {
		rmVec3 vn;
		for (rmU32 i = 0; i < nVNormals; ++i) {
			vn = m.getVNormal(i);
			fs << "vn " << vn.x << " " << vn.z << " " << ((-1) * vn.y) << newLine;
		}
	}

	// faces
	if (nFaces != 0) {
		curMatId = 99999;
		rmFace f;
		rmTVFace tvf;
		rmVNFace vnf;
		
		if ((nTVerts != 0) && (nVNormals != 0)) {
			for (rmU32 i = 0; i < nFaces; ++i) {
				f = m.getFace(i);
				tvf = m.getTVFace(i);
				vnf = m.getVNFace(i);
				(hasMaterials) ? checkMatId(f.matId) : static_cast<void>(true);
				fs << "f " << (f.a + 1) << "/" << (tvf.a + 1) << "/" << (vnf.a + 1) << " " << (f.b + 1) << "/" << (tvf.b + 1) << "/" << (vnf.b + 1) << " " << (f.c + 1) << "/" << (tvf.c + 1) << "/" << (vnf.c + 1) << newLine;
			}
		}

		if ((nTVerts != 0) && (nVNormals == 0)) {
			for (rmU32 i = 0; i < nFaces; ++i) {
				f = m.getFace(i);
				tvf = m.getTVFace(i);
				(hasMaterials) ? checkMatId(f.matId) : static_cast<void>(true);
				fs << "f " << (f.a + 1) << "/" << (tvf.a + 1) << " " << (f.b + 1) << "/" << (tvf.b + 1) << " " << (f.c + 1) << "/" << (tvf.c + 1) << newLine;
			}
		}

		if ((nTVerts == 0) && (nVNormals != 0)) {
			for (rmU32 i = 0; i < nFaces; ++i) {
				f = m.getFace(i);
				vnf = m.getVNFace(i);
				(hasMaterials) ? checkMatId(f.matId) : static_cast<void>(true);
				fs << "f " << (f.a + 1) << "//" << (vnf.a + 1) << " " << (f.b + 1) << "//" << (vnf.b + 1) << " " << (f.c + 1) << "//" << (vnf.c + 1) << newLine;
			}
		}

		if ((nTVerts == 0) && (nVNormals == 0)) {
			for (rmU32 i = 0; i < nFaces; ++i) {
				f = m.getFace(i);
				(hasMaterials) ? checkMatId(f.matId) : static_cast<void>(true);
				fs << "f " << (f.a + 1) << " " << (f.b + 1) << " " << (f.c + 1) << newLine;
			}
		}

		fs << newLine;
	}
}

void rmObjWriter::checkMatId(rmU32 newMatId)
{
	if (newMatId != curMatId) {
		curMatId = newMatId;
		*fileStream << newLine << "usemtl " << scene->getMat(curMatId).getName() << newLine << newLine;
	}
}
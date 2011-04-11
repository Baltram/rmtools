#include "rmObjReader.h"

rmOut rmObjReader::out = rmOut("rmObjReader.cpp");
rmInFileStream * rmObjReader::fileStream = 0;
rmScene * rmObjReader::scene = 0;

rmLPCChar const rmObjReader::tokenStrings[TOKEN_COUNT] = {
	"f",
	"map_Kd",
	"mtllib",
	"newmtl",
	"usemtl",
	"v",
	"vn",
	"vt"
};

void rmObjReader::readObjFileToScene(rmInFileStream * fileStream, rmScene * scene)
{
	rmObjReader::fileStream = fileStream;
	rmObjReader::scene = scene;

	rmInFileStream & fs = *fileStream;
	fs.initTokenStringList(tokenStrings, TOKEN_COUNT);

	string sceneName = fs.getFileName();
        sceneName.resize(max(sceneName.find_last_of('\\'), sceneName.size()));
	sceneName.replace(0, (sceneName.find_last_of('\\') + 1), "");
	scene->setName(sceneName);

	vector<Token> lineTokens;
	lineTokens.reserve(fs.getFileSize() / 40);
	lineTokens.resize(0);

	vector<string> matNames;

	string s;
	Token token;

	// First pass.

	list<string> fallbackMaterialNames;

	rmU32 nVerts = 0;
	rmU32 nFaces = 0;
	rmU32 nTVerts = 0;
	rmU32 nVNormals = 0;

	rmBool hasTVerts = false;
	rmBool hasVNormals = false;
	rmBool checkedForFaceFormat = false;

	rmBool hasWComponent = false;
	rmBool checkedForTVertFormat = false;

	while (fs.endOfFile() == false) {
		fs.getToken(&token);
		lineTokens.push_back(token);
		switch (token)
		{
		case TOKEN_F :
			{
				++nFaces;
				if (checkedForFaceFormat == false) {
					rmU32 curPos = fs.tell();
					fs.skipToCharacter("\n", 1);
					rmU32 maxPos = fs.tell() - 1;
					fs.seek(curPos);
					
					if (fs.skipToCharacter("/", 1, maxPos) == true) {
						if (fs.readShort() == 12079) { // Two slashes.
							hasVNormals = true;
						}
						else
						{
							hasTVerts = true;
							fs.skip(-1);
							rmU32 dummyInt;
							fs >> dummyInt;
							if (fs.readByte() == 47) { // One slash.
								hasVNormals = true;
							}
						}
					}

					checkedForFaceFormat = true;
				}
				break;
			}
		case TOKEN_MTLLIB :
			{
				string mtlLibName;
				fs >> mtlLibName;
				readMtlFile(mtlLibName); // Materials already sorted by name.
				matNames.resize(scene->getNumMaterials());
				for (rmU32 i = 0; i < matNames.size(); ++i) {
					matNames[i] = scene->getMat(i).getName();
				}
				break;
			}
		case TOKEN_USEMTL :
			{
			    string matName;
			    fs >> matName;
			    fallbackMaterialNames.push_back(matName);
			    break;
			}
		case TOKEN_V :
			{
				++nVerts;
				break;
			}
		case TOKEN_VN :
			{
				++nVNormals;
				break;
			}
		case TOKEN_VT :
			{
				++nTVerts;
				if (checkedForTVertFormat == false) {
					rmF32 dummyFloat;
					fs >> dummyFloat >> dummyFloat;
					rmU32 curPos = fs.tell();
					fs.skipToCharacter("\n", 1);
					rmU32 maxPos = fs.tell() - 1;
					fs.seek(curPos);
					hasWComponent = fs.skipToCharacter("0123456789", 10, maxPos);
					checkedForTVertFormat = true;
				}
				break;
			}
		}
		fs >> nextLine;
	}

	if ((matNames.size() == 0) && (fallbackMaterialNames.size() != 0)) { // No material file found / specified.
	    scene->createMaterials(fallbackMaterialNames); // Materials already sorted by name.
	    matNames.resize(scene->getNumMaterials());
	    for (rmU32 i = 0; i < matNames.size(); ++i) {
		matNames[i] = scene->getMat(i).getName();
	    }
	}

	string nodeName = fs.getFileName();
	nodeName.replace(0, (nodeName.find_last_of("\\") + 1), "");
	nodeName.resize(nodeName.find_last_of("."));

	rmNode & node = scene->addNode();
	node.setName(nodeName);

	rmMesh & mesh = node.setMesh();
	mesh.setNumVerts(nVerts);
	mesh.setNumFaces(nFaces);
	mesh.setNumTVerts(nTVerts);
	mesh.setNumVNormals(nVNormals);

	// Second pass.

	rmPoint3 v;
	rmPoint3 tv(0.0, 0.0, 0.0);
	rmVec3 vn;
	rmFace f(0, 0, 0, 0, 0);
	rmTVFace tvf;
	rmVNFace vnf;

	rmU32 iVert = 0;
	rmU32 iFace = 0;
	rmU32 iTVert = 0;
	rmU32 iVNormal = 0;

	string dummyString;
	Token * tokenPointer = &lineTokens[0];

	fs.seek(0);
	fs >> dummyString;
	while (fs.endOfFile() == false) {
		switch (*tokenPointer++)
		{
		case TOKEN_F :
			{
				rmChar c; // Dummy char.
				if (hasTVerts && hasVNormals) {
					fs >> f.a >> c >> tvf.a >> c >> vnf.a >> f.b >> c >> tvf.b >> c >> vnf.b >> f.c >> c >> tvf.c >> c >> vnf.c;
					--tvf.a;
					--tvf.b;
					--tvf.c;
					--vnf.a;
					--vnf.b;
					--vnf.c;
					mesh.setTVFace(iFace, tvf);
					mesh.setVNFace(iFace, vnf);
				}
				else if (hasTVerts) {
					fs >> f.a >> c >> tvf.a >> f.b >> c >> tvf.b >> f.c >> c >> tvf.c;
					--tvf.a;
					--tvf.b;
					--tvf.c;
					mesh.setTVFace(iFace, tvf);
				}
				else if (hasVNormals) {
					fs >> f.a >> c >> c >> vnf.a >> f.b >> c >> c >> vnf.b >> f.c >> c >> c >> vnf.c;
					--vnf.a;
					--vnf.b;
					--vnf.c;
					mesh.setVNFace(iFace, vnf);
				}
				else {
					fs >> f.a >> f.b >> f.c;
				}
				--f.a;
				--f.b;
				--f.c;
				mesh.setFace(iFace++, f);
				break;
			}
		case TOKEN_USEMTL :
			{
				if (matNames.size() == 0) {
					break;
				}
				string matName;
				fs >> matName;
				f.matId = findStringInArray(matName, &matNames[0], matNames.size());
                                if (f.matId == (-1)) {

                                }
				break;
			}	
		case TOKEN_V :
			{
				fs >> v.x >> v.z >> v.y;
				v.y *= (-1);
				mesh.setVert(iVert++, v);
				break;
			}
		case TOKEN_VN :
			{
				fs >> vn.x >> vn.y >> vn.z;
				mesh.setVNormal(iVNormal++, vn);
				break;
			}
		case TOKEN_VT :
			{
				fs >> tv.x >> tv.y;
				if (hasWComponent) {
					fs >> tv.z;
				}
				mesh.setTVert(iTVert++, tv);
				break;
			}
		}
		fs >> nextLine >> dummyString;
	}
}

void rmObjReader::readMtlFile(string materialLibraryName)
{
	Token token;
	rmMaterial mat;
	rmMap map;

	string objFileName = fileStream->getFileName();
	objFileName.resize(objFileName.find_last_of("\\") + 1);
	string mtlFileName = objFileName + materialLibraryName;

	rmInFileStream mtl;
	try {
		mtl.open(mtlFileName);
	}
        catch (rmException & e) {
                OUT_SWARN("Could not open " << mtlFileName)
		return;
	}
	mtl.initTokenStringList(tokenStrings, TOKEN_COUNT);

	while (mtl.endOfFile() == false) {
		mtl.getToken(&token);
		switch (token)
		{
		case TOKEN_NEWMTL :
			{
				string materialName;
				mtl >> materialName >> nextLine;
				mat.setName(materialName);
				scene->addMat(&mat);
				break;
			}
		case TOKEN_MAP_KD :
			{
				string mapPath;
				mtl >> mapPath >> nextLine;
				map.setName(mapPath.substr(0, mapPath.find_last_of(".")).substr(mapPath.find_last_of("\\") + 1));
				map.setBitmapPath(mapPath);
				scene->getMat(scene->getNumMaterials() - 1).setDiffuseMap(map);
				break;
			}
		default :
			{
				mtl >> nextLine;
			}
		}
	}

	scene->sortMaterialsByName();
}

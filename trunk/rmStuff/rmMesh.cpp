#include "rmMesh.h"

rmOut rmMesh::out = rmOut("rmMesh.cpp");

rmMesh::rmMesh(void)
{
	extents = 0;
	resetUnInit();
}

rmMesh::rmMesh(const rmMesh & m)
{
	extents = 0;
	resetUnInit();
	*this = m;
}

rmMesh::~rmMesh(void)
{
	reset();
}

void rmMesh::reset(void)
{
	delete [] vertices;
	delete [] faces;
	delete [] TVerts;
	delete [] TVFaces;
	delete [] VNormals;
	delete [] VNFaces;
	delete [] VTangents;
	delete [] VTFaces;

	resetUnInit();
}

void rmMesh::resetUnInit(void)
{
	this->numVerts = 0;
	this->numFaces = 0;
	this->numTVerts = 0;
	this->numVNormals = 0;
	this->numVTangents = 0;

	this->vertices = 0;
	this->faces = 0;
	this->TVerts = 0;
	this->TVFaces = 0;
	this->VNormals = 0;
	this->VNFaces = 0;
	this->VTangents = 0;
	this->VTFaces = 0;

	extentsChanged();
}

void rmMesh::swap(rmMesh * other)
{
	rmBox * extents_tmp = extents;
	extents = other->extents;
	other->extents = extents_tmp;

	rmU32 numVerts_tmp(numVerts);
	rmU32 numFaces_tmp(numFaces);
	rmU32 numTVerts_tmp(numTVerts);
	rmU32 numVNormals_tmp(numVNormals);
	rmU32 numVTangents_tmp(numVTangents);

	rmPoint3 * vertices_tmp(vertices);
	rmFace * faces_tmp(faces);
	rmPoint3 * TVerts_tmp(TVerts);
	rmTVFace * TVFaces_tmp(TVFaces);
	rmVec3 * VNormals_tmp(VNormals);
	rmVNFace * VNFaces_tmp(VNFaces);
	rmVec4 * VTangents_tmp(VTangents);
	rmVTFace * VTFaces_tmp(VTFaces);	

	numVerts = other->numVerts;
	numFaces = other->numFaces;
	numTVerts = other->numTVerts;
	numVNormals = other->numVNormals;
	numVTangents = other->numVTangents;

	vertices = other->vertices;
	faces = other->faces;
	TVerts = other->TVerts;
	TVFaces = other->TVFaces;
	VNormals = other->VNormals;
	VNFaces = other->VNFaces;
	VTangents = other->VTangents;
	VTFaces = other->VTFaces;

	other->numVerts = numVerts_tmp;
	other->numFaces = numFaces_tmp;
	other->numTVerts = numTVerts_tmp;
	other->numVNormals = numVNormals_tmp;
	other->numVTangents = numVTangents_tmp;

	other->vertices = vertices_tmp;
	other->faces = faces_tmp;
	other->TVerts = TVerts_tmp;
	other->TVFaces = TVFaces_tmp;
	other->VNormals = VNormals_tmp;
	other->VNFaces = VNFaces_tmp;
	other->VTangents = VTangents_tmp;
	other->VTFaces = VTFaces_tmp;
}

const rmPoint3 * rmMesh::getVertsContainer(void) const
{
	return vertices;
}

const rmFace * rmMesh::getFacesContainer(void) const
{
	return faces;
}

const rmPoint3 * rmMesh::getTVertsContainer(void) const
{
	return TVerts;
}

const rmTVFace * rmMesh::getTVFacesContainer(void) const
{
	return TVFaces;
}

const rmVec3 * rmMesh::getVNormalsContainer(void) const
{
	return VNormals;
}
const rmVNFace * rmMesh::getVNFacesContainer(void) const
{
	return VNFaces;
}

const rmVec4 * rmMesh::getVTangentsContainer(void) const
{
	return VTangents;
}

const rmVTFace * rmMesh::getVTFacesContainer(void) const
{
	return VTFaces;
}

void rmMesh::setNumVerts(rmU32 num)
{
	try {
		if (num > 0) {
			delete [] vertices;
			vertices = new rmPoint3 [num];
			memset(vertices, 0, (num * sizeof(rmPoint3)));
		}
		else {
			setNumFaces(0);
			setNumTVerts(0);
			setNumVNormals(0);
			setNumVTangents(0);
		}
		numVerts = num;
		extentsChanged();
	}
	catch (bad_alloc &) {
		OUT_SERR("Bad alloc when initializing mesh components")
	}
}

void rmMesh::setNumFaces(rmU32 num)
{
	try {
		if (num > 0) { 
			delete [] faces;
			faces = new rmFace [num];
			memset(faces, 0, (num * sizeof(rmFace)));

			if (numTVerts > 0) {
				delete [] TVFaces;
				TVFaces = new rmTVFace [num];
				memset(TVFaces, 0, (num * sizeof(rmTVFace)));
			}
			
			if (numVNormals > 0) {
				delete [] VNFaces;
				VNFaces = new rmVNFace [num];
				memset(VNFaces, 0, (num * sizeof(rmVNFace)));
			}

			if (numVTangents > 0) {
				delete [] VTFaces;
				VTFaces = new rmVTFace [num];
				memset(VTFaces, 0, (num * sizeof(rmVTFace)));
			}
		}
		else {
			delete [] faces;
			faces = 0;
			delete [] TVFaces;
			TVFaces = 0;
			delete [] VNFaces;
			VNFaces = 0;
			delete [] VTFaces;
			VTFaces = 0;
		}
		numFaces = num;
	}
	catch (bad_alloc &) {
		OUT_SERR("Bad alloc when initializing mesh components")
	}
}

void rmMesh::setNumTVerts(rmU32 num)
{
	try {
		if (num > 0) {
			delete [] TVerts;
			TVerts = new rmPoint3 [num];
			memset(TVerts, 0, (num * sizeof(rmPoint3)));

			if (numTVerts == 0) {
				delete [] TVFaces;
				TVFaces = new rmTVFace [numFaces];
				memset(TVFaces, 0, (numFaces * sizeof(rmTVFace)));
			}
		}
		else {
			delete [] TVerts;
			TVerts = 0;
			delete [] TVFaces;
			TVFaces = 0;
		}
		numTVerts = num;
	}
	catch (bad_alloc &) {
		OUT_SERR("Bad alloc when initializing mesh components")
	}
}

void rmMesh::setNumVNormals(rmU32 num)
{
	try {
		if (num > 0) {
			delete [] VNormals;
			VNormals = new rmVec3 [num];
			memset(VNormals, 0, (num * sizeof(rmVec3)));

			if (numVNormals == 0) {
				delete [] VNFaces;
				VNFaces = new rmVNFace [numFaces];
				memset(VNFaces, 0, (numFaces * sizeof(rmVNFace)));
			}
		}
		else {
			delete [] VNormals;
			VNormals = 0;
			delete [] VNFaces;
			VNFaces = 0;
		}
		numVNormals = num;
	}
	catch (bad_alloc &) {
		OUT_SERR("Bad alloc when initializing mesh components")
	}
}

void rmMesh::setNumVTangents(rmU32 num)
{
	try {
		if (num > 0) {
			delete [] VTangents;
			VTangents = new rmVec4 [num];
			memset(VTangents, 0, (num * sizeof(rmVec4)));

			if (numVTangents == 0) {
				delete [] VTFaces;
				VTFaces = new rmVTFace [numFaces];
				memset(VTFaces, 0, (numFaces * sizeof(rmVTFace)));
			}
		}
		else {
			delete [] VTangents;
			VTangents = 0;
			delete [] VTFaces;
			VTFaces = 0;
		}
		numVTangents = num;
	}
	catch (bad_alloc &) {
		OUT_SERR("Bad alloc when initializing mesh components")
	}
}

rmU32 rmMesh::getNumVerts(void) const
{
	return this->numVerts;
}

rmU32 rmMesh::getNumFaces(void) const
{
	return this->numFaces;
}

rmU32 rmMesh::getNumTVerts(void) const
{
	return this->numTVerts;
}

rmU32 rmMesh::getNumVNormals(void) const
{
	return this->numVNormals;
}

rmU32 rmMesh::getNumVTangents(void) const
{
	return this->numVTangents;
}

void rmMesh::setVert(rmU32 num, const rmPoint3 & val)
{
	this->vertices[num] = val;
	extentsChanged();
}

void rmMesh::setFace(rmU32 num, const rmFace & val)
{
	this->faces[num] = val;
}

void rmMesh::setTVert(rmU32 num, const rmPoint3 & val)
{
	this->TVerts[num] = val;
}

void rmMesh::setTVFace(rmU32 num, const rmTVFace & val)
{
	this->TVFaces[num] = val;
}

void rmMesh::setVNormal(rmU32 num, const rmVec3 & val)
{
	this->VNormals[num] = val;
}

void rmMesh::setVNFace(rmU32 num, const rmVNFace & val)
{
	this->VNFaces[num] = val;
}

void rmMesh::setVTangent(rmU32 num, const rmVec4 & val)
{
	this->VTangents[num] = val;
}

void rmMesh::setVTFace(rmU32 num, const rmVTFace & val)
{
	this->VTFaces[num] = val;
}

void rmMesh::setVerts(rmU32 count, const rmPoint3 * src)
{
    setNumVerts(count);
    memcpy(vertices, src, (sizeof(rmPoint3) * count));
    extentsChanged();
}

void rmMesh::setFaces(rmU32 count, const rmFace * src)
{
    setNumFaces(count);
    memcpy(faces, src, (sizeof(rmFace) * count));
}

void rmMesh::setTVerts(rmU32 count, const rmPoint3 * src)
{
    setNumTVerts(count);
    memcpy(TVerts, src, (sizeof(rmPoint3) * count));
}

void rmMesh::setTVFaces(rmU32 count, const rmTVFace * src)
{
    if (count != numFaces) {
	OUT_SERR("Got wrong count.")
    }
    memcpy(TVFaces, src, (sizeof(rmTVFace) * count));
}

void rmMesh::setVNormals(rmU32 count, const rmVec3 * src)
{
    setNumVNormals(count);
    memcpy(VNormals, src, (sizeof(rmVec3) * count));
}

void rmMesh::setVNFaces(rmU32 count, const rmVNFace * src)
{
    if (count != numFaces) {
	OUT_SERR("Got wrong count.")
    }
    memcpy(VNFaces, src, (sizeof(rmVNFace) * count));
}

void rmMesh::setVTangents(rmU32 count, const rmVec4 * src)
{
    setNumVTangents(count);
    memcpy(VTangents, src, (sizeof(rmVec4) * count));
}

void rmMesh::setVTFaces(rmU32 count, const rmVTFace * src)
{
    if (count != numFaces) {
	OUT_SERR("Got wrong count.")
    }
    memcpy(VTFaces, src, (sizeof(rmVTFace) * count));
}

const rmPoint3 & rmMesh::getVert(rmU32 num) const
{
	return this->vertices[num];
}

const rmFace & rmMesh::getFace(rmU32 num) const
{
	return this->faces[num];
}

const rmPoint3 & rmMesh::getTVert(rmU32 num) const
{
	return this->TVerts[num];
}

const rmTVFace & rmMesh::getTVFace(rmU32 num) const
{
	return this->TVFaces[num];
}

const rmVec3 & rmMesh::getVNormal(rmU32 num) const
{
	return this->VNormals[num];
}

const rmVNFace & rmMesh::getVNFace(rmU32 num) const
{
	return this->VNFaces[num];
}

const rmVec4 & rmMesh::getVTangent(rmU32 num) const
{
	return this->VTangents[num];
}

const rmVTFace & rmMesh::getVTFace(rmU32 num) const
{
	return this->VTFaces[num];
}

rmVec3 rmMesh::getFNormal(rmU32 num) const
{
	const rmFace & f = this->faces[num];

	const rmPoint3 & v1 = this->vertices[f.a];
	const rmPoint3 & v2 = this->vertices[f.b];
	const rmPoint3 & v3 = this->vertices[f.c];

	return rmVec3::cross((v2 - v1), (v3 - v1)).normalizeTo();
}

rmVec4 rmMesh::getFTangent(rmU32 num) const
{
	return getFTangent(num, getFNormal(num));
}

rmVec4 rmMesh::getFTangent(rmU32 num, const rmVec3 & normal) const
{
	const rmFace & f = this->faces[num];
	const rmTVFace & tf = this->TVFaces[num];

	const rmPoint3 & v1 = this->vertices[f.a];
	const rmPoint3 & v2 = this->vertices[f.b];
	const rmPoint3 & v3 = this->vertices[f.c];

	const rmPoint3 & t1 = this->TVerts[tf.a];
	const rmPoint3 & t2 = this->TVerts[tf.b];
	const rmPoint3 & t3 = this->TVerts[tf.c];

	rmVec3 v21 = v2 - v1;
	rmVec3 v31 = v3 - v1;

	rmVec3 t21 = t2 - t1;
	rmVec3 t31 = t3 - t1;

	rmVec3 tangent = (v21 * t31.y - v31 * t21.y).normalizeTo();
	rmVec3 bitangent = (v31 * t21.x - v21 * t31.x).normalizeTo();

	rmVec4 tmp(tangent, 1);

	if (rmVec3::dot(rmVec3::cross(normal, tangent), bitangent) < 0) {
		tmp.w = -1; //Left handed TBN space
	}

	return tmp;
}

void rmMesh::calcFacesUsingVerts(rmSArr2<rmFVIndex> & dest)
{
	rmArr2<rmFVIndex> tmp((numFaces * 3), numVerts);
	rmFace * f = faces;
	rmU32 nf_3 = numFaces * 3;
	for (rmU32 fi_3 = 0; fi_3 != nf_3;) {
		tmp[f->a].append(fi_3++);
		tmp[f->b].append(fi_3++);
		tmp[f->c].append(fi_3++);
		++f;
	}
	tmp.sort(dest);
}

void rmMesh::calcVNormals(bool useSgs, rmF32 minCosOfAngle)
{
	rmU32 nIndices = numFaces * 3;
	rmWFNormal * weightedFNormals = new rmWFNormal [numFaces]; //weighted face normals
	calcFNormalsWeightedByAngle(weightedFNormals);
	rmSArr2<rmFVIndex> facesPerVert;
	calcFacesUsingVerts(facesPerVert);

	rmVec3 * vNormals = new rmVec3 [nIndices];
	memset(vNormals, 0, nIndices * sizeof(rmVec3));
	rmVec3 * vNormal = vNormals;
	rmU32 * indices = new rmU32 [nIndices];
	memset(indices, 0, nIndices * sizeof(rmU32));

	rmU32 k = -1;	
	for (rmFVIndex i = 0; i != nIndices; ++i) {
		if (indices[i] != 0) {
			continue;
		}
		else {
			indices[i] = ++k;
		}
		rmU32 v = faces[i / 3].verts[i % 3];
		rmSArr2<rmFVIndex>::SAList facesUsingVert = facesPerVert[v];
		rmU32 numFacesUsingVert = facesUsingVert.getCount();
		rmU32 sgs = faces[i / 3].sgs;
		if ((sgs == 0) && (useSgs)) {
			vNormals[k] = weightedFNormals[i / 3].normal;
		}
		else {
			for (rmU32 j = 0; j < numFacesUsingVert; ++j) {
				rmFVIndex f = facesUsingVert[j];
				
				rmBool include;
				if (useSgs) {
					include = (sgs & faces[f / 3].sgs);
				}
				else {
					include = (rmVec3::dot(weightedFNormals[i / 3].normal, weightedFNormals[f / 3].normal) >= minCosOfAngle);
				}
				
				if (include) {
					rmWFNormal & n = weightedFNormals[f / 3];
					vNormals[k] += (n.normal * n.weightings[f % 3]);
					indices[f] = k;
				}
			}
			vNormals[k].normalize();
		}
	}

	setNumVNormals(++k);
	memcpy(this->VNormals, vNormals, k * sizeof(rmVec3));
	memcpy(VNFaces, indices, numFaces * sizeof(rmVNFace));

	delete [] indices;
	delete [] vNormals;
	delete [] weightedFNormals;
}

rmBox rmMesh::calcExtents(void)
{
    if (extents != 0) {
	return *extents;
    }

    rmBox b;
    rmPoint3 & min = b.min;
    rmPoint3 & max = b.max;
    if (numVerts == 0) {
	max = min = rmPoint3(0, 0, 0);
	return b;
    }
    max = min = *vertices;

    for (rmU32 i = 0; i != numVerts; ++i) {
	rmPoint3 & p = vertices[i];
	if (p.x < min.x) {
	    min.x = p.x;
	}
	else if (p.x > max.x) {
	    max.x = p.x;
	}
	if (p.y < min.y) {
	    min.y = p.y;
	}
	else if (p.y > max.y) {
	    max.y = p.y;
	}
	if (p.z < min.z) {
	    min.z = p.z;
	}
	else if (p.z > max.z) {
	    max.z = p.z;
	}
    }

    return b;
}

void rmMesh::extentsChanged(void)
{
    delete extents;
    extents = 0;
}

void rmMesh::calcVNormalsBySGs(void)
{
	calcVNormals(true);
}

void rmMesh::calcVNormalsByAngle(rmF32 maxAngleInDegrees)
{
	calcVNormals(false, cos(maxAngleInDegrees * M_PI / 180.0));
}

void rmMesh::calcFNormalsWeightedByAngle(rmWFNormal * dest)
{
	for (rmU32 i = 0; i < numFaces; ++i) {
		const rmFace & f = faces[i];

		rmVec3 e1 = vertices[f.a] - vertices[f.c];
		rmVec3 e2 = vertices[f.b] - vertices[f.a];
		rmVec3 e3 = vertices[f.c] - vertices[f.b];

		dest[i].normal = (rmVec3::cross(e2, e1).normalizeTo() * (-1));
		dest[i].weightings[0] = acos(-rmVec3::cosOfAngle(e1, e2));
		dest[i].weightings[1] = acos(-rmVec3::cosOfAngle(e2, e3));
		dest[i].weightings[2] = M_PI - dest[i].weightings[0] - dest[i].weightings[1];
	}
}

void rmMesh::buildSimpleVNFaces(void)
{
	if (numVNormals != (numFaces * 3)) {
		OUT_SERR("Not able to build simple VNFaces due to incorrect numVNormals")
	}
	else {
		rmVNFace * vnf = VNFaces;
		rmU32 j = 0;
		for (rmU32 i = 0; i < numFaces; ++i) {
			vnf->a = (j++);
			vnf->b = (j++);
			vnf->c = (j++);
			++vnf;
		}
	}
}

rmMesh & rmMesh::operator = (const rmMesh & m)
{
	extentsChanged();
	if (m.extents != 0) {
	    extents = new rmBox;
	    *extents = *m.extents;
	}

	numVerts = m.numVerts;
	numFaces = m.numFaces;
	numTVerts = m.numTVerts;
	numVNormals = m.numVNormals;
	numVTangents = m.numVTangents;

	delete [] vertices;
	delete [] faces;
	delete [] TVerts;
	delete [] TVFaces;
	delete [] VNormals;
	delete [] VNFaces;
	delete [] VTangents;
	delete [] VTFaces;

	if (numFaces != 0) {
		faces = new rmFace [numFaces];
		memcpy(faces, m.faces, (numFaces * sizeof(rmFace)));
		if (numTVerts != 0) {
			TVerts = new rmPoint3 [numTVerts];
			TVFaces = new rmTVFace [numFaces];
			memcpy(TVerts, m.TVerts, (numTVerts * sizeof(rmPoint3)));
			memcpy(TVFaces, m.TVFaces, (numFaces * sizeof(rmTVFace)));
		}
		if (numVNormals != 0) {
			VNormals = new rmVec3 [numVNormals];
			VNFaces = new rmVNFace [numFaces];
			memcpy(VNormals, m.VNormals, (numVNormals * sizeof(rmVec3)));
			memcpy(VNFaces, m.VNFaces, (numFaces * sizeof(rmVNFace)));
		}
		if (numVTangents != 0) {
			VTangents = new rmVec4 [numVTangents];
			VTFaces = new rmVTFace [numFaces];
			memcpy(VTangents, m.VTangents, (numVTangents * sizeof(rmVec4)));
			memcpy(VTFaces, m.VTFaces, (numFaces * sizeof(rmVTFace)));
		}
	}
	if (numVerts != 0) {
		vertices = new rmPoint3 [numVerts];
		memcpy(vertices, m.vertices, (numVerts * sizeof(rmPoint3)));
	}

	return *this;
}

void rmMesh::copyFacesToTVFaces(void)
{
	if (numTVerts == 0) {
		OUT_SERR("There are no texture vertices")
	}
	for (rmU32 i = 0; i < numFaces; ++i) {
		memcpy(TVFaces[i].verts, faces[i].verts, (3 * sizeof(rmU32)));
	}
}

void rmMesh::copyFacesToVNFaces(void)
{
	if (numVNormals == 0) {
		OUT_SERR("There are no vertex normals")
	}
	for (rmU32 i = 0; i < numFaces; ++i) {
		memcpy(VNFaces[i].verts, faces[i].verts, (3 * sizeof(rmU32)));
	}
}

void rmMesh::copyFacesToVTFaces(void)
{
	if (numVTangents == 0) {
		OUT_SERR("There are no vertex tangents")
	}
	for (rmU32 i = 0; i < numFaces; ++i) {
		memcpy(VTFaces[i].verts, faces[i].verts, (3 * sizeof(rmU32)));
	}
}

void rmMesh::swapYZ(void)
{
    // Vertices.
    for (rmU32 i = 0; i != numVerts; ++i) {
		vertices[i].swapYZ();
    }

    // Texture Vertices.
    for (rmU32 i = 0; i != numTVerts; ++i) {
		TVerts[i].y *= (-1);
    }

    // Vertex Normals.
    for (rmU32 i = 0; i != numVNormals; ++i) {
		VNormals[i].swapYZ();
    }

    // Vertex Tangents.
	for (rmU32 i = 0; i != numVTangents; ++i) {
		VTangents[i].swapYZ();
    }

    // Faces.
	for (rmU32 i = 0; i != numFaces; ++i) {
	faces[i].invert();
    }

    // TVFaces.
    if (numTVerts != 0) {
		for (rmU32 i = 0; i != numFaces; ++i) {
			TVFaces[i].invert();
		}
    }

    // VNFaces.
    if (numVNormals != 0) {
		for (rmU32 i = 0; i != numFaces; ++i) {
			VNFaces[i].invert();
		}
    }

    // VTFaces.
    if (numVTangents != 0) {
		for (rmU32 i = 0; i != numFaces; ++i) {
			VTFaces[i].invert();
		}
    }
}

void rmMesh::sortFacesByMaterial(void)
{
	const rmFace * const endFaces = faces + numFaces;
	rmFace * newFaces = new rmFace [numFaces];
	const rmFace * const endNewFaces = newFaces + numFaces;

	rmU32 * nFacesPerMat = new rmU32 [1000];
	memset(nFacesPerMat, 0, 1000 * sizeof(rmU32));
	for (const rmFace * f = faces; f != endFaces; ++f) {
		++nFacesPerMat[f->matId];
	}

	rmFace ** facesPerMat = new rmFace * [1000];
	rmFace * f = newFaces;
	for (rmU32 i = 0; f != endNewFaces; ++i) {
		facesPerMat[i] = f;
		f += nFacesPerMat[i];
	}

	for (const rmFace * f = faces; f != endFaces; ++f) {
		*facesPerMat[f->matId]++ = *f;
	}

	delete [] faces;
	faces = newFaces;
}

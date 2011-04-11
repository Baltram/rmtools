#ifdef RM_PHYSX

#include "rmXcomFile.h"

rmOut rmXcomFile::out("rmXcomFile.cpp");

rmLPCChar const rmXcomFile::shapeMaterialNames[NShapeMaterials] = {"none", "wood", "metal", "water", "stone", "earth", "ice", "leather", "clay", "glass", "flesh", "snow", "debris", "foliage", "magic", "grass", "springanddamper1", "springanddamper2", "springanddamper3", "damage", "sand", "movement", "axe"};

rmXcomFile::rmXcomFile() {}

rmXcomFile::~rmXcomFile()
{
    for (rmU32 i = 0, ie = triangleMeshes.size(); i != ie; ++i) {
	delete triangleMeshes[i];
    }
}

rmXcomFile::TriangleMesh * rmXcomFile::getTriangleMesh(rmU32 index)
{
    return triangleMeshes[index];
}

void rmXcomFile::reset(void)
{
    triangleMeshes.clear();
}

void rmXcomFile::removeTriangleMesh(rmU32 index)
{
    delete triangleMeshes[index];
    triangleMeshes.erase(triangleMeshes.begin() + index);
}

rmXcomFile::TriangleMesh * rmXcomFile::addTriangleMesh(void)
{
    TriangleMesh * tm = new TriangleMesh;
    triangleMeshes.push_back(tm);
    initTriangleMesh(tm);
    return tm;
}

rmU32 rmXcomFile::getNumTriangleMeshes(void)
{
    return triangleMeshes.size();
}

bool rmXcomFile::read(string xcomFilePath)
{
    reset();
    rmNxInFileStream xcom;
    try {
	xcom.open(xcomFilePath);
    }
    catch (exception & e) {
	OUT_LOG("Could not open " << xcomFilePath)
	return false;
    }
    if (xcom.readString(8) != "GR01CM00") {
	OUT_LOG("File not compatible: " << xcomFilePath)
	return false;
    }
    xcom.seek(111);
    if (xcom.readByte() != 0) {
	OUT_LOG("No support for convex meshes")
	return false;
    }
    xcom.seek(201);
    xcom.skip(xcom.readLong() + 2);

    for (rmU32 i = 0, ie = xcom.readLong(); i != ie; ++i) {
	readTriangleMesh(&xcom, &addTriangleMesh()->mesh);
    }
    xcom.skip(4);

    if (triangleMeshes.size() != xcom.readLong()) {
	reset();
	OUT_LOG("Material count not equal to triangle mesh count")
	return false;
    }
    for (rmU32 i = 0, ie = triangleMeshes.size(); i != ie; ++i) {
	triangleMeshes[i]->shapeMaterial = static_cast<ShapeMaterial>(xcom.readByte());
	triangleMeshes[i]->ignoredByTraceRay = (xcom.readByte() != 0);
	triangleMeshes[i]->disableCollision = (xcom.readByte() != 0);
	triangleMeshes[i]->disableResponse = (xcom.readByte() != 0);
    }
    return true;
}

bool rmXcomFile::write(string xcomFilePath)
{
    initSdk();

    rmNxOutFileStream xcom;
    try {
	xcom.open(xcomFilePath);
    }
    catch (exception & e) {
	OUT_LOG("Error writing to " << xcomFilePath)
	return false;
    }

    rmU32 nTriangleMeshes = triangleMeshes.size();

    xcom.writeString("GR01CM00");

    rmU32 propSize = (172 + 24 * nTriangleMeshes);
    xcom.writeLong(40); // PropOffset.
    xcom.writeLong(propSize); // PropSize.
    xcom.writeLong(propSize + 40); // DataOffset.
    xcom.writeLong(0); // DataSize (yet unknown) @20.

    xcom.writeLong64(time(0) * 10000000 + 116444736000000000); // RawTimestamp.
    xcom.writeLong64(0); // RawFileExt.

    xcom.writeShort(1); // Version.
    xcom.writeByte(1); // HasPropertyObject.
    xcom.writeShort(1); // Reserved1.
    xcom.writeByte(1); // Reserved2.
    xcom.writeShort(24); xcom.writeString("eCCollisionMeshResource2"); // ClassName.
    xcom.writeShort(1); // Reserved3.
    xcom.writeByte(0); // Reserved4.
    xcom.writeShort(201); // ClassVersion.
    xcom.writeShort(201); // Reserved5.
    xcom.writeLong(propSize - 43); // DataSize.

    xcom.writeShort(201); // Version.
    xcom.writeLong(3); // PropCount.

    // Properties.

    xcom.writeShort(8); xcom.writeString("IsConvex"); // Name.
    xcom.writeShort(4); xcom.writeString("bool"); // Type.
    xcom.writeShort(30); // Version.
    xcom.writeLong(1); // Size.
    xcom.writeByte(0); // Not convex.

    xcom.writeShort(8); xcom.writeString("Boundary"); // Name.
    xcom.writeShort(5); xcom.writeString("bCBox"); // Type.
    xcom.writeShort(30); // Version.
    xcom.writeLong(24); // Size.
    writeFakeBBox(xcom);

    xcom.writeShort(13); xcom.writeString("SubBoundaries"); // Name.
    xcom.writeShort(23); xcom.writeString("bTObjArray<class bCBox>"); // Type.
    xcom.writeShort(30); // Version.
    xcom.writeLong(5 + 24 * nTriangleMeshes); // Size.
    xcom.writeByte(1); // Version.
    xcom.writeLong(nTriangleMeshes); // Count
    for (rmU32 i = 0; i != nTriangleMeshes; ++i) {
	xcom.writeBox(triangleMeshes[i]->mesh.calcExtents().swapYZ());
    }

    xcom.writeShort(201); // Version.
    xcom.writeLong(nTriangleMeshes); // TriangleMeshCount.

    for (rmU32 i = 0; i != nTriangleMeshes; ++i) {
	rmMesh m(triangleMeshes[i]->mesh);
	m.swapYZ();

	rmU16 * matIds = new rmU16 [m.getNumFaces()];
	for (rmU32 i = 0, ie = m.getNumFaces(); i != ie; ++i) {
	    matIds[i] = 1;
	}

	for (rmU32 i = 0, ie = m.getNumVerts(); i != ie; ++i) {
	    rmPoint3 v = m.getVert(i);
	    v.x /= 100;
	    v.y /= 100;
	    v.z /= 100;
	    m.setVert(i, v);
	}

	NxTriangleMeshDesc desc;
	desc.flags = 0;
	desc.numTriangles = m.getNumFaces();
	desc.numVertices = m.getNumVerts();
	desc.triangleStrideBytes = sizeof(rmFace);
	desc.pointStrideBytes = sizeof(rmPoint3);
	desc.materialIndexStride = sizeof(rmU16);
	desc.triangles = m.getFacesContainer();
	desc.points = m.getVertsContainer();
	desc.materialIndices = matIds;

	if (!gCooking->NxCookTriangleMesh(desc, xcom)) {
	    OUT_WARN("Failed to cook triangle mesh")
	    m.swapYZ();
	    return false;
	}

	delete [] matIds;
    }

    xcom.writeLong(0); // ConvexMeshCount.

    xcom.writeLong(nTriangleMeshes); // MaterialCount.
    for (rmU32 i = 0; i != nTriangleMeshes; ++i) {
	xcom.writeByte(triangleMeshes[i]->shapeMaterial);
	xcom.writeByte(triangleMeshes[i]->ignoredByTraceRay);
	xcom.writeByte(triangleMeshes[i]->disableCollision);
	xcom.writeByte(triangleMeshes[i]->disableResponse);
    }

    xcom.seek(20);
    xcom.writeLong(xcom.getFileSize() - (propSize + 40));

    return true;
}

void rmXcomFile::readTriangleMesh(rmNxInFileStream * xcom, rmMesh * dest)
{
    initSdk();

    NxTriangleMesh * tm = 0;
    tm = gPhysicsSdk->createTriangleMesh(*xcom);
    if (tm == 0) {
	OUT_LOG("Corrupt XCOM file")
    }

    dest->reset();

    rmU32 nVerts = tm->getCount(0, NX_ARRAY_VERTICES);
    rmU32 nFaces = tm->getCount(0, NX_ARRAY_TRIANGLES);

    // Reading out vertices.

    rmPoint3 * verts = new rmPoint3 [nVerts];

    flexCopy(verts, tm->getBase(0, NX_ARRAY_VERTICES), nVerts, sizeof(rmPoint3), tm->getStride(0, NX_ARRAY_VERTICES));

    for (rmU32 i = 0; i != nVerts; ++i) {
	rmPoint3 & p = verts[i];
	rmF32 y = p.y;
	p.x = p.x * 100;
	p.y = p.z * 100;
	p.z = y * 100;
    }
    dest->setVerts(nVerts, verts);

    delete [] verts;

    // Reading out faces.

    rmU32 elementSize = getElementSize(tm, NX_ARRAY_TRIANGLES);
    rmFace * faces = new rmFace [nFaces];
    rmU8 * tmpFaces = new rmU8 [elementSize * 3 * nFaces];
    rmU32 * tmpFaces2 = new rmU32 [nFaces * 3];

    flexCopy(tmpFaces, tm->getBase(0, NX_ARRAY_TRIANGLES), nFaces, (elementSize * 3), tm->getStride(0, NX_ARRAY_TRIANGLES));
    flexCopy(tmpFaces2, tmpFaces, (nFaces * 3), elementSize, 0, sizeof(rmU32), true);
    flexCopy(faces, tmpFaces2, nFaces, (sizeof(rmU32) * 3), 0, sizeof(rmFace), true);

    for (rmU32 i = 0; i != nFaces; ++i) {
	rmFace & f = faces[i];
	rmU32 a = f.a;
	f.a = f.c;
	f.c = a;
	f.matId = tm->getTriangleMaterial(i);
	if (f.matId != 1) {
	    OUT_MSG("Triangle mesh face has matId other than 1") //$
	}
    }
    dest->setFaces(nFaces, faces);

    delete [] faces;
    delete [] tmpFaces;
    delete [] tmpFaces2;

    gPhysicsSdk->releaseTriangleMesh(*tm);
}

rmU32 rmXcomFile::getElementSize(NxTriangleMesh * tm, NxInternalArray type)
{
    switch (tm->getFormat(0, type))
    {
	case NX_FORMAT_FLOAT:
	{
	    return sizeof(rmF32);
	}
	case NX_FORMAT_BYTE:
	{
	    return sizeof(rmU8);
	}
	case NX_FORMAT_SHORT:
	{
	    return sizeof(rmU16);
	}
	case NX_FORMAT_INT:
	{
	    return sizeof(rmU32);
	}
	default:
	{
	    return 0;
	}
    }
}

void rmXcomFile::initTriangleMesh(TriangleMesh * tm)
{
    tm->shapeMaterial = Earth;
    tm->disableCollision = false;
    tm->disableResponse = false;
    tm->ignoredByTraceRay = false;
}

void rmXcomFile::writeFakeBBox(rmOutFileStream & out)
{
    out.writeFloat(XCOL_F_MAX);
    out.writeFloat(XCOL_F_MAX);
    out.writeFloat(XCOL_F_MAX);
    out.writeFloat(XCOL_F_MIN);
    out.writeFloat(XCOL_F_MIN);
    out.writeFloat(XCOL_F_MIN);
}

#endif // RM_PHYSX

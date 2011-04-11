#ifdef RM_PHYSX

#include "rmCollisionBinWriter.h"

// ErrorIdRange: 34762XXX
// Used: 34762001, 34762002, 34762003, 34762004, 34762005, 34762006

rmStringTable rmCollisionBinWriter::stringTable(800);

rmInFileStream rmCollisionBinWriter::binIn;
rmOutFileStream rmCollisionBinWriter::bin;
rmInFileStream rmCollisionBinWriter::xcom;

rmOut rmCollisionBinWriter::out;

void rmCollisionBinWriter::writeXcomFileAsRecord(void)
{
	if (stringTable.count() == 0) {
		stringTable.registerString("eCCollisionMeshResource2");
		stringTable.registerString("IsConvex");
		stringTable.registerString("bool");
		stringTable.registerString("Boundary");
		stringTable.registerString("bCBox");
		stringTable.registerString("SubBoundaries");
		stringTable.registerString("bTObjArray<class bCBox>");
	}

	string fileName = xcom.getFileName();
	fileName = fileName.substr((fileName.find_last_of('\\') + 1), (fileName.find_last_of('.') - fileName.find_last_of('\\') - 1));

	if (xcom.readString(8).compare("GR01CM00") != 0) {
		OUT_SERR("XCOM file not valid")
	}
	xcom.seek(20);
	rmU32 dataSize = xcom.readLong();
	xcom.seek(111);
	rmBool isConvex = (xcom.readByte() != 0);

	bin.writeString("CM00"); // DataRevision.

	bin.writeShort(1); // Version.
	bin.writeByte(1); // HasPropertyObject.
	bin.writeShort(1); // Reserved1.
	bin.writeByte(1); // Reserved2.
	bin.writeShort(0); // ClassName: "eCCollisionMeshResource2".
	bin.writeShort(1); // Reserved3.
	bin.writeByte(0); // Reserved4.
	bin.writeShort(201); // ClassVersion.
	bin.writeShort(201); // Reserved5.
	bin.writeLong(68); // DataSize.

	bin.writeShort(201); // Version.
	bin.writeLong(3); // PropCount.

	// Properties.

	bin.writeShort(1); // "IsConvex".
	bin.writeShort(2); // "bool".
	bin.writeShort(30); // Version.
	bin.writeLong(1); // Size.
	bin.writeByte(isConvex);

	bin.writeShort(3); // "Boundary".
	bin.writeShort(4); // "bCBox".
	bin.writeShort(30); // Version.
	bin.writeLong(24); // Size.
	rmXcomFile::writeFakeBBox(bin);

	bin.writeShort(5); // "SubBoundaries".
	bin.writeShort(6); // "bTObjArray<class bCBox>".
	bin.writeShort(30); // Version.
	bin.writeLong(5); // Size.
	bin.writeByte(1); // Version.
	bin.writeLong(0); // Count.

	bin.writeShort(201); // Version.
	bin.writeShort(stringTable.registerString(fileName));
	bin.writeShort(8); // RawFileExtension: "".
	bin.writeLong(dataSize);

	if (stringTable.count() == 8) {
		stringTable.registerString("");
	}
}

void rmCollisionBinWriter::registerFile(string xcomFilePath, string binFilePath)
{
	string tmpBinFilePath = string(binFilePath).append("_tmp");
	if (!xcom.open(xcomFilePath)) {
		OUT_SERR_E("Could not open file: " << xcomFilePath, 34762001)
	}
	if (!binIn.open(binFilePath)) {
		OUT_SERR_E("Could not open file: " << binFilePath, 34762002)
	}
	if ((binIn.readString(8).compare("GENOMFLE") != 0) || (binIn.getFileSize() < 31)) {
		OUT_SERR_E("File not valid: " << binFilePath, 34762004)
	}
	binIn.seek(41);
	if (binIn.readLong() == 92) {
		OUT_SERR_E("Old bin file version: " << binFilePath, 34762006)
	}
	if (!bin.open(tmpBinFilePath)) {
		OUT_SERR_E("Could not write to file: " << tmpBinFilePath, 34762003)
	}
	string fileName = xcomFilePath.substr((xcomFilePath.find_last_of('\\') + 1), (xcomFilePath.find_last_of('.') - xcomFilePath.find_last_of('\\') - 1));

	binIn.seek(10);
	rmU32 stringTableOffset = binIn.readLong();
	binIn.seek(18);
	rmU32 nRecords = binIn.readLong();

	rmChar * buffer = new rmChar [stringTableOffset];
	binIn.seek(0);
	binIn.read(buffer, stringTableOffset);
	bin.write(buffer, stringTableOffset);
	delete [] buffer;

	stringTable.reset();
	stringTable.readInStringList(binIn);
	binIn.close();

	rmU32 recordOffset = ((max<rmU32>(stringTable.registerString(fileName), 8) - 8) * 99 + 22);
	if (recordOffset == stringTableOffset) {
		stringTableOffset += 99;
		++nRecords;
		bin.seek(10);
		bin.writeLong(stringTableOffset);
		bin.seek(18);
		bin.writeLong(nRecords);
	}
	bin.seek(recordOffset);
	writeXcomFileAsRecord();
	xcom.close();

	bin.seek(stringTableOffset);
	stringTable.writeStringList(bin);

	bin.close();
	remove(binFilePath.c_str());
	if (rename(tmpBinFilePath.c_str(), binFilePath.c_str())) {
		remove(tmpBinFilePath.c_str());
		OUT_SERR_E("Could not write to file: " << binFilePath, 34762005)
	}
}

void rmCollisionBinWriter::rebuildBin(string binFilePath)
{
	stringTable.reset();
	string tmpBinFilePath = string(binFilePath).append("_tmp");
	if (!bin.open(tmpBinFilePath)) {
		OUT_SERR_E("Could not write to file: " << tmpBinFilePath, 34762003)
	}

	bin.writeString("GENOMFLE");
	bin.writeShort(1); // Version.
	bin.writeLong(0); // @10: placeholder for stringTableOffset.
	bin.writeString("GH01"); // Revision.
	bin.writeLong(0); // @18: placeholder for nEntries.

	rmU32 nEntries = 0;
	rmDir binDir(binFilePath.substr(0, binFilePath.find_last_of('\\')));
	string xcomFilePath;

	while (binDir.getDeepFileAbs(xcomFilePath)) {
		if (xcomFilePath.substr(xcomFilePath.find_last_of('.') + 1).compare("_xcom") != 0) {
			continue;
		}
		if (!xcom.open(xcomFilePath)) {
			continue;
		}
		try {
			writeXcomFileAsRecord();
			++nEntries;
		}
		catch (exception & e) {
		}
	}
	xcom.close();
	rmU32 stringTableOffset = bin.tell();
	stringTable.writeStringList(bin);

	bin.seek(10);
	bin.writeLong(stringTableOffset);
	bin.seek(18);
	bin.writeLong(nEntries);

	bin.close();
	remove(binFilePath.c_str());
	if (rename(tmpBinFilePath.c_str(), binFilePath.c_str())) {
		remove(tmpBinFilePath.c_str());
		OUT_SERR_E("Could not write to file: " << binFilePath, 34762005)
	}
}

#endif // RM_PHYSX

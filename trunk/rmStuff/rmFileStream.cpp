#include "rmFileStream.h"

using namespace std;


//////////////////////////////////////////////////////
// begin rmFileStream
//////////////////////////////////////////////////////

rmFileStream::rmFileStream(void)
{
	reset();
}

rmFileStream::~rmFileStream(void)
{
	delete [] buffer;
	delete fileStream;
}

string rmFileStream::getFileName(void)
{
	return fileName;
}

rmU32 rmFileStream::getFileSize(void)
{
	return fileSize;
}

void rmFileStream::reset(void)
{
	buffer = 0;
	inBuffer = 0;
	bufferPosition = 0;
	bufferEndPosition = 0;
	bufferSize = 0;
	fileStream = 0;
	fileName = "";
	fileSize = 0;
}

rmBool rmFileStream::failed(void)
{
	rmBool result = false;
	if (fileStream != 0) {
		result = fileStream->fail();
	}
	return result;
}

void rmFileStream::close(void)
{
	delete [] buffer;
	delete fileStream;
	reset();
}

rmU32 rmFileStream::tell(void)
{
	return bufferPosition + (inBuffer - buffer);
}

void rmFileStream::bigEndianMode(void)
{
	handle16 = &swap16;
	handle32 = &swap32;
	handle64 = &swap64;
}

void rmFileStream::littleEndianMode(void)
{
	handle16 = &noSwap;
	handle32 = &noSwap;
	handle64 = &noSwap;
}

void rmFileStream::noSwap(void * destination) {}

void rmFileStream::swap16(void * destination)
{
	swapDest = reinterpret_cast<rmI8 *>(destination);
	swapBuff[0] = swapDest[0];
	swapDest[0] = swapDest[1];
	swapDest[1] = swapBuff[0];
}

void rmFileStream::swap32(void * destination)
{
	swapDest = reinterpret_cast<rmI8 *>(destination);
	swapBuff[0] = swapDest[0];
	swapBuff[1] = swapDest[1];
	swapBuff[2] = swapDest[2];
	swapDest[0] = swapDest[3];
	swapDest[1] = swapBuff[2];
	swapDest[2] = swapBuff[1];
	swapDest[3] = swapBuff[0];
}

void rmFileStream::swap64(void * destination)
{
	swapDest = reinterpret_cast<rmI8 *>(destination);
	swapBuff[0] = swapDest[0];
	swapBuff[1] = swapDest[1];
	swapBuff[2] = swapDest[2];
	swapBuff[3] = swapDest[3];
	swapBuff[4] = swapDest[4];
	swapBuff[5] = swapDest[5];
	swapBuff[6] = swapDest[6];
	swapDest[0] = swapDest[7];
	swapDest[1] = swapBuff[6];
	swapDest[2] = swapBuff[5];
	swapDest[3] = swapBuff[4];
	swapDest[4] = swapBuff[3];
	swapDest[5] = swapBuff[2];
	swapDest[6] = swapBuff[1];
	swapDest[7] = swapBuff[0];
}

rmOut rmFileStream::out = rmOut("rmFileStream.cpp");

rmI8 rmFileStream::swapBuff[8];
rmI8 * rmFileStream::swapDest;

//////////////////////////////////////////////////////
// end rmFileStream
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// begin rmInFileStream
//////////////////////////////////////////////////////

rmInFileStream::rmInFileStream(void)
{
	inBuffer = endBuffer = 0;
}

rmInFileStream::rmInFileStream(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode)
{
	inBuffer = endBuffer = 0;
	open(filename, bufferSize, openMode);
	tokenStringList = 0;
}

rmInFileStream::~rmInFileStream(void) {}

rmBool rmInFileStream::open(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode)
{
	OUT_LOG("Opening \"" << filename << "\".")
	if (fileStream != 0) {
		close();
	}
	inFileStream = new ifstream(filename.c_str(), openMode);
	if (inFileStream->fail() == true) {
		delete inFileStream;
		OUT_SWARN("Unable to open \"" << filename << "\".")
		return false;
	}
	else {
		littleEndianMode();
		considerQuotesSet = false;
		fileStream = inFileStream;
		this->fileName = filename;
		inFileStream->seekg(0, ios_base::end);
		fileSize = inFileStream->tellg();
		this->bufferSize = min(max<rmU32>(bufferSize, (BUFFER_PADDING * 4)), fileSize);
		buffer = new rmI8 [this->bufferSize + 1];
		endBuffer = (buffer + this->bufferSize);
		buffer[this->bufferSize] = 0;
		maxBufferPosition = fileSize - this->bufferSize;
		bufferPosition = 123456; // If bufferPosition was 0 no buffering would be performed.
		fillBuffer(0);
		return true;
	}
}

rmBool rmInFileStream::endOfFile(void)
{
	return ((bufferEndPosition == fileSize) && (inBuffer == endBuffer));
}

void rmInFileStream::seek(rmU32 position)
{
	rmI32 offset = (position - bufferPosition);
	if ((offset < 0) || (offset > static_cast<rmI32>(bufferSize))) {
		if (fileStream != 0) {
			if (position <= fileSize) {
				fillBuffer(position);
			}
			else {
				OUT_SERR("Tried to seek over end of file.")
			}
		}
		else {
			OUT_SERR("Tried to seek in closed file stream.")
		}
	}
	else {
		inBuffer = buffer + offset;
	}
}

void rmInFileStream::skip(rmI32 nBytes)
{
	inBuffer += nBytes;
	if ((inBuffer > endBuffer) || (buffer > inBuffer)) {
		fillBuffer(tell());
	}
}

void rmInFileStream::read(void * destination, rmU32 nBytes)
{
	rmU32 nAviableBytes = (endBuffer - inBuffer);
	if (nBytes <= nAviableBytes) {
		memcpy(destination, inBuffer, nBytes);
		inBuffer += nBytes;
	}
	else {
		if (nBytes <= (fileSize - (bufferPosition + (inBuffer - buffer)))) {
			memcpy(destination, inBuffer, nAviableBytes);
			destination = (reinterpret_cast<rmI8 *>(destination) + nAviableBytes);
			nBytes -= nAviableBytes;
			if (nBytes > MUCH_DATA) {
				directRead(destination, bufferEndPosition, nBytes);
				fillBuffer(bufferEndPosition + nBytes);
			}
			else {
				fillBuffer(bufferEndPosition);
				memcpy(destination, inBuffer, nBytes);
				inBuffer += nBytes;
			}
		}
		else {
			if (fileStream != 0) {
				OUT_SERR("Read over end of file.")
			}
			else {
				OUT_SERR("Read in closed file stream.")
			}
		}
	}
}

void rmInFileStream::read(rmChar & dest)
{
	read(&dest, sizeof(rmChar));
}

void rmInFileStream::read(rmI8 & dest)
{
	read(&dest, sizeof(rmI8));
}

void rmInFileStream::read(rmU8 & dest)
{
	read(&dest, sizeof(rmU8));
}

void rmInFileStream::read(rmI16 & dest)
{
	read(&dest, sizeof(rmI16));
	(handle16)(&dest);
}

void rmInFileStream::read(rmU16 & dest)
{
	read(&dest, sizeof(rmU16));
	(*handle16)(&dest);
}

void rmInFileStream::read(rmI32 & dest)
{
	read(&dest, sizeof(rmI32));
	(*handle32)(&dest);
}

void rmInFileStream::read(rmU32 & dest)
{
	read(&dest, sizeof(rmU32));
	(*handle32)(&dest);
}

void rmInFileStream::read(rmI64 & dest)
{
	read(&dest, sizeof(rmI64));
	(*handle64)(&dest);
}

void rmInFileStream::read(rmU64 & dest)
{
	read(&dest, sizeof(rmU64));
	(*handle64)(&dest);
}

rmI8 rmInFileStream::readByte(void)
{
	rmI8 tmp;
	read(&tmp, sizeof(rmI8));
	return tmp;
}

rmI16 rmInFileStream::readShort(void)
{
	rmI16 tmp;
	read(&tmp, sizeof(rmI16));
	(*handle16)(&tmp);
	return tmp;
}

rmI32 rmInFileStream::readLong(void)
{
	rmI32 tmp;
	read(&tmp, sizeof(rmI32));
	(*handle32)(&tmp);
	return tmp;
}

rmI64 rmInFileStream::readLong64(void)
{
	rmI64 tmp;
	read(&tmp, sizeof(rmI64));
	(*handle64)(&tmp);
	return tmp;
}

void rmInFileStream::read(rmF32 & dest)
{
	read(&dest, sizeof(rmF32));
	(*handle32)(&dest);
}

void rmInFileStream::read(rmF64 & dest)
{
	read(&dest, sizeof(rmF64));
	(*handle64)(&dest);
}

rmF32 rmInFileStream::readFloat(void)
{
	rmF32 tmp;
	read(&tmp, sizeof(rmF32));
	(*handle32)(&tmp);
	return tmp;
}

rmF64 rmInFileStream::readDouble(void)
{
	rmF64 tmp;
	read(&tmp, sizeof(rmF64));
	(*handle64)(&tmp);
	return tmp;
}

void rmInFileStream::read(rmBox & dest)
{
    read(dest.min.x);
    read(dest.min.y);
    read(dest.min.z);
    read(dest.max.x);
    read(dest.max.y);
    read(dest.max.z);
}

rmBox rmInFileStream::readBox(void)
{
    rmBox b;
    read(b);
    return b;
}

void rmInFileStream::readString(string & dest, rmU32 size)
{
	dest.resize(size);
	read(const_cast<rmLPChar>(dest.data()), size);
}

void rmInFileStream::readString(string & dest)
{
	rmU32 size = strlen(inBuffer_char);
	if ((inBuffer + size) == endBuffer) {
		fillBuffer(tell());
		size += strlen(reinterpret_cast<rmLPChar>(inBuffer + size));
	}
	dest.resize(size);
	read(const_cast<rmLPChar>(dest.data()), size);
}

void rmInFileStream::readUnicodeStringToAscii(string & dest, rmU32 size)
{
	if ((inBuffer + (size * 2)) > endBuffer) {
		fillBuffer(tell());
	}
	dest.resize(size);
	rmLPChar stringBuffer = const_cast<rmLPChar>(dest.data());
	for (rmU32 i = 0; i < size; ++i) {
		stringBuffer[i] = inBuffer[2 * i];
	}
}

void rmInFileStream::readUnicodeStringToAscii(string & dest)
{
	rmI8 * max = (endBuffer - 1);
	rmI8 * c = inBuffer;
	while ((*c != 0) && (c < max)) {
		c += 2;
	}
	rmU32 size = ((c - inBuffer) / 2);
	if (c >= max) {
		fillBuffer(tell());
		max -= 2;
		c = inBuffer + (size * 2);
		while ((*c != 0) && (c < max)) {
			c += 2;
		}
		size = ((c - inBuffer) / 2);
	}
	readUnicodeStringToAscii(dest, size);
}

string rmInFileStream::readString(rmU32 size)
{
	string tmp;
	readString(tmp, size);
	return tmp;
}

string rmInFileStream::readString(void)
{
	string tmp;
	readString(tmp);
	return tmp;
}

string rmInFileStream::readUnicodeStringToAscii(rmU32 size)
{
	string tmp;
	readUnicodeStringToAscii(tmp, size);
	return tmp;
}

string rmInFileStream::readUnicodeStringToAscii(void)
{
	string tmp;
	readUnicodeStringToAscii(tmp);
	return tmp;
}

void rmInFileStream::directRead(void * destination, rmU32 position, rmU32 nBytes)
{
	inFileStream->seekg(position);
	inFileStream->read((static_cast<rmLPChar>(destination)), nBytes);
}

void rmInFileStream::fillBuffer(rmU32 position)
{
	if ((position > fileSize) || (fileStream == 0)) {
		OUT_SERR("Tried to exceed end of file.")
	}
	rmU32 tmp = min<rmU32>(max<rmI32>(0, (position - BUFFER_PADDING)), maxBufferPosition);
	if (tmp == bufferPosition) {
		seek(position);
		return;
	}
	bufferPosition = tmp;
	bufferEndPosition = (tmp + bufferSize);
	directRead(buffer, bufferPosition, bufferSize);
	inBuffer = buffer + (position - bufferPosition);
	if (inBuffer > (endBuffer)) {
		OUT_SERR("Tried to exceed end of file.")
	}
}

rmI32 rmInFileStream::readAsciiLong(void)
{
	rmI32 tmp;
	rmLPChar endPtr;
	skipWhiteSpaceCharacters();
	tmp = strtol(inBuffer_char, &endPtr, 10);
	if (endPtr == endBuffer_char) {
		if (fileSize > bufferEndPosition) {
			fillBuffer(tell());
			tmp = strtol(inBuffer_char, &endPtr, 10);
		}
	}
	inBuffer_char = endPtr;
	return tmp;
}

rmF64 rmInFileStream::readAsciiDouble(void)
{
	rmF64 tmp;
	rmLPChar endPtr;
	skipWhiteSpaceCharacters();
	tmp = strtod(inBuffer_char, &endPtr);
	if (endPtr == endBuffer_char) {
		if (fileSize > bufferEndPosition) {
			fillBuffer(tell());
			tmp = strtod(inBuffer_char, &endPtr);
		}
	}
	inBuffer_char = endPtr;
	return tmp;
}

rmInFileStream & rmInFileStream::operator >> (rmChar & dest)
{
	read(dest);
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmI8 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmU8 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmI16 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmU16 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmI32 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmU32 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmI64 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmU64 & dest)
{
	dest = readAsciiLong();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmF32 & dest)
{
	dest = readAsciiDouble();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmF64 & dest)
{
	dest = readAsciiDouble();
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (string & dest)
{
	rmChar quoteChar = 34;
	skipWhiteSpaceCharacters();
	rmU32 stringStartPos = tell();
	if (stringStartPos == fileSize) {
		dest = "";
		return *this;
	}
	if ((considerQuotesSet == true) && (*inBuffer == quoteChar)) {
		++stringStartPos;
		++inBuffer;
		skipToCharacter(&quoteChar, 1);
	}
	else {
		skipToNextWhiteSpaceCharacter();
	}
	rmU32 stringSize = (tell() - stringStartPos);
	seek(stringStartPos);
	readString(dest, stringSize);
	return *this;
}

rmInFileStream & rmInFileStream::operator >> (rmInFileStream & (* manip)(rmInFileStream &))
{
	return (*manip)(*this);
}

rmInFileStream & considerQuotes(rmInFileStream & s)
{
	s.considerQuotesSet = true;
	return s;
}

rmInFileStream & notConsiderQuotes(rmInFileStream & s)
{
	s.considerQuotesSet = false;
	return s;
}

rmInFileStream & nextLine(rmInFileStream & s)
{
	s.skipToCharacter("\n", 1);
	if (s.bufferEndPosition != s.fileSize) {
		s.skip(1);
	}
	return s;
}

rmBool rmInFileStream::skipCharacters(rmLPChar charactersToSkip, rmU8 arraySize, rmU32 maxPosition)
{
	maxPosition = min(maxPosition, fileSize);
	rmI8 * maxInBuffer = min(inBuffer + (maxPosition - tell()), endBuffer);
	while ((inBuffer < maxInBuffer) && (memchr(charactersToSkip, *inBuffer, arraySize) != 0)) {
		++inBuffer;
	}
	if (inBuffer == endBuffer) {
		if (maxPosition > tell()) {
			fillBuffer(tell());
			return skipCharacters(charactersToSkip, arraySize, maxPosition);
		}
	}
	return ((memchr(charactersToSkip, *inBuffer, arraySize) == 0) || ((inBuffer == endBuffer) && (bufferEndPosition == fileSize)));
}

rmBool rmInFileStream::skipToCharacter(rmLPChar charactersToNotSkip, rmU8 arraySize, rmU32 maxPosition)
{
	maxPosition = min(maxPosition, fileSize);
	rmI8 * maxInBuffer = min(inBuffer + (maxPosition - tell()), endBuffer);
	while ((inBuffer < maxInBuffer) && (memchr(charactersToNotSkip, *inBuffer, arraySize) == 0)) {
		++inBuffer;
	}
	if (inBuffer == endBuffer) {
		if (maxPosition > tell()) {
			fillBuffer(tell());
			return skipToCharacter(charactersToNotSkip, arraySize);
		}
	}
	return ((memchr(charactersToNotSkip, *inBuffer, arraySize) != 0) && ((inBuffer != endBuffer) || (bufferEndPosition != fileSize)));
}

rmBool rmInFileStream::skipWhiteSpaceCharacters(rmU32 maxPosition)
{
	maxPosition = min(maxPosition, fileSize);
	rmI8 * maxInBuffer = min(inBuffer + (maxPosition - tell()), endBuffer);
	while ((inBuffer < maxInBuffer) && (*inBuffer_u < 33)) {
		++inBuffer;
	}
	if (inBuffer == endBuffer) {
		if (maxPosition > tell()) {
			fillBuffer(tell());
			return skipWhiteSpaceCharacters();
		}
	}
	return ((*inBuffer_u > 32) || ((inBuffer == endBuffer) && (bufferEndPosition == fileSize)));
}

rmBool rmInFileStream::skipToNextWhiteSpaceCharacter(rmU32 maxPosition)
{
	maxPosition = min(maxPosition, fileSize);
	rmI8 * maxInBuffer = min(inBuffer + (maxPosition - tell()), endBuffer);
	while ((inBuffer < maxInBuffer) && (*inBuffer_u > 32)) {
		++inBuffer;
	}
	if (inBuffer == endBuffer) {
		if (maxPosition > tell()) {
			fillBuffer(tell());
			return skipToNextWhiteSpaceCharacter();
		}
	}
	return ((*inBuffer_u < 33) && ((inBuffer != endBuffer) || (bufferEndPosition != fileSize)));
}

rmBool rmInFileStream::findBytes(rmI8 * bytesToFind, rmU32 size, rmBool searchOverEndOfFile)
{
	rmU32 maxBufferPos = (fileSize - size + 1); // Actually 1 more than real maximum.
	rmI8 * maxBuffer = (endBuffer - size + 1);   // Again 1 more.
	rmU32 searchStartPos = tell();
	if (searchStartPos >= maxBufferPos) {
		if (searchOverEndOfFile == false) {
			return false;
		}
		searchStartPos = 0;
	}
	rmU32 currentPos;
	void * occurance;
	while (true) {
		if (inBuffer >= maxBuffer) {
			currentPos = tell();
			if (currentPos >= maxBufferPos) {
				if ((maxBufferPos == searchStartPos) || (searchOverEndOfFile == false)) {
					return false;
				}
				else {
					maxBufferPos = searchStartPos;
					currentPos = 0;
				}
			}
			fillBuffer(currentPos);
		}
		occurance = memchr(inBuffer, *bytesToFind, (maxBuffer - inBuffer));
		if (occurance == 0) {
			inBuffer = maxBuffer;
		}
		else {
			inBuffer = reinterpret_cast<rmI8 *>(occurance);
			if (memcmp(occurance, bytesToFind, size) == 0) {
				skip(size);
				return true;
			}
			else {
				++inBuffer;
			}
		}
	}
	return false; // Never reached.
}

rmBool rmInFileStream::findBytes(rmI8 * bytesToFind, rmU32 size, rmI8 ignoreRepresentation, rmBool searchOverEndOfFile)
{
	rmU32 maxBufferPos = (fileSize - size + 1); // Actually 1 more than real maximum.
	rmI8 * maxBuffer = (endBuffer - size + 1); // Again 1 more.
	rmU32 searchStartPos = tell();
	if (searchStartPos >= maxBufferPos) {
		if (searchOverEndOfFile == false) {
			return false;
		}
		searchStartPos = 0;
	}
	rmU32 currentPos;
	void * occurance;
	while (true) {
		if (inBuffer >= maxBuffer) {
			currentPos = tell();
			if (currentPos >= maxBufferPos) {
				if ((maxBufferPos == searchStartPos) || (searchOverEndOfFile == false)) {
					return false;
				}
				else {
					maxBufferPos = searchStartPos;
					currentPos = 0;
				}
			}
			fillBuffer(currentPos);
		}
		occurance = memchr(inBuffer, *bytesToFind, (maxBuffer - inBuffer));
		if (occurance == 0) {
			inBuffer = maxBuffer;
		}
		else {
			inBuffer = reinterpret_cast<rmI8 *>(occurance);
			rmU32 i = 1; 
			for (; i < size; ++i) {
				if (inBuffer[i] != bytesToFind[i]) {
					if (bytesToFind[i] != ignoreRepresentation) {
						break;
					}
				}
			}
			if (i == size) {
				skip(size);
				return true;
			}
			else {
				++inBuffer;
			}
		}
	}
	return false; // Never reached.
}

void rmInFileStream::initTokenStringList(rmLPCChar const * tokenStringList, rmU32 nTokens)
{
	this->tokenStringList = tokenStringList;
	this->nTokens = nTokens;
}

void rmInFileStream::getToken(void * dest)
{
	if (tokenStringList == 0) {
		OUT_SERR("Trying to get token before initializing token string list")
	}

	string tokenString;
	*this >> tokenString;

	*reinterpret_cast<rmU32 *>(dest) = findCStringInArray(tokenString.c_str(), tokenStringList, nTokens);
}


//////////////////////////////////////////////////////
// end rmInFileStream
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// begin rmMaxInFileStream
//////////////////////////////////////////////////////

rmMaxInFileStream::rmMaxInFileStream(void)
{
	properBlocks = 0;
}

rmMaxInFileStream::rmMaxInFileStream(string filename, rmU32 bufferSize, 
									 RM_IOS_OPENMODE openMode)
{
	properBlocks = 0;
	open(filename, bufferSize, openMode);
}

rmMaxInFileStream::~rmMaxInFileStream(void) {}

rmBool rmMaxInFileStream::open(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode)
{
	OUT_LOG("Opening \""  <<  filename  <<  "\".")
	close();
	inFileStream = new ifstream(filename.c_str(), openMode);
	if (inFileStream->fail() == true) {
		delete inFileStream;
		inFileStream = 0;
		OUT_SERR("Unable to open \""  <<  filename  <<  "\".")
		return false;
	}
	else {
		fileStream = inFileStream;
		this->fileName = filename;
		inFileStream->seekg(0, ios_base::end);
		fileSize = inFileStream->tellg();

		rmU32 nJunkBlocks;
		rmU32 nElementsToCopy; // Element means DWORD in junk block list
		rmU32 nCopiedElements = 0;
		rmU32 nextJunkBlockListPosCode;
		rmU32 * junkBlockList;

		inFileStream->seekg(44);
		inFileStream->read(reinterpret_cast<rmLPChar>(&nJunkBlocks), sizeof(rmU32));
		if (nJunkBlocks > 60000) { // 60000 junk blocks ~4GB
			OUT_SERR("3ds Max file not compatible.")
		}
		junkBlockList = new rmU32 [nJunkBlocks + 480]; // There are irregular junk blocks.
													  // 4GB 3ds Max files contain ~480.
		inFileStream->seekg(68);
		inFileStream->read(reinterpret_cast<rmLPChar>(&nextJunkBlockListPosCode), 
						   sizeof(rmU32));
		
		inFileStream->seekg(76); // First junk block list, ends at 512 -> up to 109 elements.
		nElementsToCopy = min<rmU32>(nJunkBlocks, 109);
		inFileStream->read(reinterpret_cast<rmLPChar>(junkBlockList), 
						   (nElementsToCopy * sizeof(rmU32)));
		nCopiedElements += nElementsToCopy;

		while (nJunkBlocks > nCopiedElements) { // Further junk block lists .
			++nJunkBlocks; // Junkblock itself.
			inFileStream->seekg((nextJunkBlockListPosCode + 1) * 512);
			nElementsToCopy = min<rmU32>((nJunkBlocks - nCopiedElements), 128);
			inFileStream->read(reinterpret_cast<rmLPChar>(&junkBlockList[
							   nCopiedElements++]), sizeof(rmU32));
			--nElementsToCopy;
			junkBlockList[nCopiedElements++] = nextJunkBlockListPosCode;
			--nElementsToCopy;
			inFileStream->read(reinterpret_cast<rmLPChar>(&junkBlockList[nCopiedElements]), 
							  (nElementsToCopy * sizeof(rmU32)));
			nCopiedElements += nElementsToCopy;
			inFileStream->read(reinterpret_cast<rmLPChar>(&nextJunkBlockListPosCode), 
							   sizeof(rmU32));
		}

		// Assumes there is at least one junk block (which is always the case).
		nProperBlocks = 2;
		for (rmU32 i = 1; i < nJunkBlocks; ++i) {
			if ((junkBlockList[i] - junkBlockList[i - 1]) != 1) {
				++nProperBlocks;
			}
		}
		properBlocks = new ProperBlock [nProperBlocks];
		
		ProperBlock * b = properBlocks;
		b->position_start = 0;
		b->position_end = ((junkBlockList[0] + 1) * 512);
		for (rmU32 i = 1; i < nJunkBlocks; ++i) {
			if ((junkBlockList[i] - junkBlockList[i - 1]) != 1) {
				++b;
				b->position_start = ((junkBlockList[i - 1] + 2) * 512);
				b->position_end = ((junkBlockList[i] + 1) * 512);
			}
		}
		++b;
		b->position_start = ((junkBlockList[nJunkBlocks - 1] + 2) * 512);
		b->position_end = fileSize;

		rmU32 properDataSize = (fileSize - (nJunkBlocks * 512));
		this->bufferSize = min<rmU32>(max<rmU32>(bufferSize, (BUFFER_PADDING * 4)), 
									  properDataSize);
		buffer = new rmI8 [this->bufferSize + 1];
		endBuffer = (buffer + bufferSize);
		buffer[this->bufferSize] = 0;
		b = (properBlocks + nProperBlocks - 1);
		rmU32 properDataSizeFromEnd = (b->position_end - b->position_start);
		while (properDataSizeFromEnd < bufferSize) {
			--b;
			properDataSizeFromEnd += (b->position_end - b->position_start);
		}
		maxBufferPosition = (b->position_start + (properDataSizeFromEnd - bufferSize));
		maxFirstBufferedBlock = b;
		bufferPosition = 123456; // If bufferPosition was 0 no buffering would be performed.
		fillBuffer(0);

		delete [] junkBlockList;
		return true;
	}
}

void rmMaxInFileStream::close(void)
{
	delete [] properBlocks;
	properBlocks = 0;
	rmFileStream::close();
}

rmU32 rmMaxInFileStream::tell(void)
{
	// This works also if inBuffer exceeds endBuffer.
	if (fileStream == 0) {
		return 0;
	}
	rmI32 offset = (inBuffer - buffer);
	rmU32 absolutePosition = (bufferPosition + offset);
	ProperBlock * b = firstBufferedProperBlock;
	offset -= (b->position_end - b->position_start);
	while (offset > 0) {
		++b;
		absolutePosition += (b->position_start - (b - 1)->position_end);
		offset -= (b->position_end - b->position_start);
	}
	return absolutePosition;
}

void rmMaxInFileStream::seek(rmU32 position)
{
	if (position > fileSize) {
		OUT_SERR("Tried to seek over end of file.")
	}
	ProperBlock * b = findProperBlock(position);
	position = max(position, b->position_start);
	rmI32 offset = (position - bufferPosition);
	if ((offset < 0) || (position > bufferEndPosition)) {
		if (fileStream != 0) {
			fillBuffer(position);
		}
		else {
			OUT_SERR("Tried to seek in closed file stream.")
		}
	}
	else {
		inBuffer = buffer + offset;
		while (b > firstBufferedProperBlock) {
			inBuffer -= (b->position_start - (b - 1)->position_end);
			--b;
		}
	}
}

void rmMaxInFileStream::skip(rmI32 nBytes)
{
	inBuffer += nBytes;
	if ((inBuffer > endBuffer) || (buffer > inBuffer)) {
		inBuffer -= nBytes;
		if (nBytes > 0) {
			nBytes -= (endBuffer - inBuffer);
			fillBuffer(bufferEndPosition);
			skip(nBytes);
		}
		else if (nBytes < 0) {
			nBytes += (inBuffer - buffer);
			rmU32 tmp = bufferPosition;
			rmU32 newPos = max<rmU32>(0, (tmp - bufferSize + BUFFER_PADDING)); // tmp will still be buffered
			fillBuffer(newPos);
			seek(tmp);
			skip(nBytes);
		}
	}
}

void rmMaxInFileStream::read(void * destination, rmU32 nBytes)
{
	rmU32 nAviableBytes = (endBuffer - inBuffer);
	if (nBytes <= nAviableBytes) {
		memcpy(destination, inBuffer, nBytes);
		inBuffer += nBytes;
	}
	else {
		memcpy(destination, inBuffer, nAviableBytes);
		destination = (reinterpret_cast<rmI8 *>(destination) + nAviableBytes);
		nBytes -= nAviableBytes;
		if (nBytes > MUCH_DATA) {
			readSkippingJunkBlocks(destination, bufferEndPosition, nBytes);
			fillBuffer(inFileStream->tellg());
		}
		else {
			fillBuffer(bufferEndPosition);
			memcpy(destination, inBuffer, nBytes);
			inBuffer += nBytes;
		}
	}
}

void rmMaxInFileStream::readSkippingJunkBlocks(void * destination, rmU32 position, 
											   rmU32 nBytes, ProperBlock * firstDataBlock)
{
	if ((position > fileSize) || (fileStream == 0)) {
		OUT_SERR("Tried to exceed end of file.")
	}
	if (firstDataBlock == 0) {
		firstDataBlock = findProperBlock(position);
	}

	// Pass through firstDataBlock and following ProperBlock[s] until enough data is read
	rmU32 tmp = firstDataBlock->position_start;
	firstDataBlock->position_start = max(position, firstDataBlock->position_start);
	ProperBlock * b = firstDataBlock;
	while (nBytes != 0) {
		if ((b - properBlocks) >= nProperBlocks) {
			OUT_SERR("Tried to exceed end of file.")
		}
		rmU32 size = min((b->position_end - b->position_start), nBytes);
		inFileStream->seekg(b->position_start);	
		inFileStream->read((static_cast<rmLPChar>(destination)), size);
		destination = (static_cast<rmI8 *>(destination) + size);
		nBytes -= size;
		++b;	
	}
	firstDataBlock->position_start = tmp;
}

void rmMaxInFileStream::fillBuffer(rmU32 position)
{
	if ((position > fileSize) || (fileStream == 0)) {
		inBuffer = 0;
		OUT_SERR("Tried to exceed end of file.")
	}
	rmU32 oldBufferPosition = bufferPosition;
	bufferPosition = position;
	ProperBlock * b = findProperBlock(bufferPosition);
	if (bufferPosition < BUFFER_PADDING) { // File gets buffered at 0.
		inBuffer = (buffer + bufferPosition);
		bufferPosition = BUFFER_PADDING;
	}
	else {
		inBuffer = (buffer + BUFFER_PADDING);
	}
	if ((bufferPosition - b->position_start) < BUFFER_PADDING) {
		bufferPosition -= (b->position_start - (b - 1)->position_end);
		--b;
	}
	bufferPosition -= BUFFER_PADDING;
	if (bufferPosition > maxBufferPosition) {
		inBuffer += (bufferPosition - maxBufferPosition);
		bufferPosition = maxBufferPosition;
		b = maxFirstBufferedBlock;
	}
	if (oldBufferPosition == bufferPosition) {
		seek(position);
		return;
	}
	readSkippingJunkBlocks(buffer, bufferPosition, bufferSize, b);
	bufferEndPosition = inFileStream->tellg();
	firstBufferedProperBlock = b;
	inBuffer = buffer + (position - bufferPosition);
	while (position > b->position_end) {
		inBuffer -= ((b + 1)->position_start - b->position_end);
		++b;
	}
}

rmMaxInFileStream::ProperBlock * rmMaxInFileStream::findProperBlock(rmU32 position)
{
	ProperBlock * firstProperBlock = properBlocks;	
	ProperBlock * lastProperBlock = properBlocks + nProperBlocks - 1;
	ProperBlock * b;

	if (position > firstProperBlock->position_end) {
		b = lastProperBlock - ((lastProperBlock - (++firstProperBlock)) / 2);
		while (((b - 1)->position_end > position) || (b->position_end < position)) {
			if (b->position_end < position) {
				firstProperBlock = b + 1;
			}
			else {
				lastProperBlock = b - 1;
			}
			b = lastProperBlock - ((lastProperBlock - firstProperBlock) / 2);
		}
	}
	else {
		b = firstProperBlock;
	}

	return b;
}

//////////////////////////////////////////////////////
// end rmMaxInFileStream
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// begin rmOutFileStream
//////////////////////////////////////////////////////

rmOutFileStream::rmOutFileStream(void)
{
	inBuffer = endBuffer = 0;
	bufferPosition = 0;
}

rmOutFileStream::rmOutFileStream(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode)
{
	inBuffer = endBuffer = 0;
	open(filename, bufferSize, openMode);
}

rmOutFileStream::~rmOutFileStream(void)
{
	if (fileStream == 0) {
		return;
	}
	flush();
}

rmBool rmOutFileStream::open(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode)
{
	OUT_LOG("Opening \""  <<  filename  <<  "\".")
	if (fileStream != 0) {
		close();
	}
	outFileStream = new ofstream(filename.c_str(), openMode);
	if (outFileStream->fail() == true) {
		delete outFileStream;
		OUT_SWARN("Unable to open \""  <<  filename  <<  "\".")
		return false;
	}
	else {
		littleEndianMode();
		writeStringsQuotedSet = false;
		fileStream = outFileStream;
		this->fileName = filename;
		outFileStream->seekp(0, ios_base::end);
		fileSize = outFileStream->tellp();
		bufferPosition = fileSize;
		this->bufferSize = max<rmU32>(bufferSize, BUFFER_PADDING);
		buffer = new rmI8 [this->bufferSize];
		endBuffer = (buffer + bufferSize);
		inBuffer = buffer;
		return true;
	}
}

void rmOutFileStream::close(void)
{
	flush();
	bufferPosition = 0;
	rmFileStream::close();
	reset();
}

rmU32 rmOutFileStream::getFileSize(void)
{
	if ((bufferPosition + (inBuffer - buffer)) > fileSize) {
		flush();
	}
	return fileSize;
}

void rmOutFileStream::seek(rmU32 position)
{
	flush();
	if (position <= fileSize) {
		bufferPosition = position;
	}
	else {
		if (fileStream != 0) {
			OUT_SERR("Tried to seek over end of file.")
		}
		else {
			OUT_SERR("Tried to seek in closed file stream.")
		}
	}
}

void rmOutFileStream::skip(rmI32 nBytes)
{
	seek(bufferPosition + (inBuffer - buffer) + nBytes);
}

void rmOutFileStream::flush(void)
{
	if (fileStream == 0) {
			OUT_SERR("Tried to write in closed file stream.")
	}
	if ((bufferPosition + (inBuffer - buffer)) > fileSize) {
		fileSize = (bufferPosition + (inBuffer - buffer));
	}
	if ((inBuffer - buffer) != 0) {
		directWrite(buffer, bufferPosition, (inBuffer -  buffer));
		outFileStream->flush();
		bufferPosition = outFileStream->tellp();
		inBuffer = buffer;
	}
}

void rmOutFileStream::write(const void * source, const rmU32 nBytes)
{
	if ((endBuffer - inBuffer) >= nBytes) {
		memcpy(inBuffer, source, nBytes);
		inBuffer += nBytes;
	}
	else {
		flush();
		if (nBytes <= bufferSize) {
			memcpy(inBuffer, source, nBytes);
			inBuffer += nBytes;
		}
		else {
			directWrite(source, bufferPosition, nBytes);
			bufferPosition = outFileStream->tellp();
		}
	}
}

void rmOutFileStream::write(const rmI8 src)
{
	writeByte(src);
}

void rmOutFileStream::write(const rmI16 src)
{
	writeShort(src);
}

void rmOutFileStream::write(const rmI32 src)
{
	writeLong(src);
}

void rmOutFileStream::write(const rmI64 & src)
{
	writeLong64(src);
}

void rmOutFileStream::write(const rmU8 src)
{
	writeByte(src);
}

void rmOutFileStream::write(const rmU16 src)
{
	writeShort(src);
}

void rmOutFileStream::write(const rmU32 src)
{
	writeLong(src);
}

void rmOutFileStream::write(const rmU64 & src)
{
	writeLong64(src);
}

void rmOutFileStream::write(const rmF32 src)
{
	writeFloat(src);
}

void rmOutFileStream::write(const rmF64 & src)
{
	writeDouble(src);
}

void rmOutFileStream::write(const rmBox & src)
{
    writeBox(src);
}

void rmOutFileStream::write(const string & src)
{
	writeString(src);
}

void rmOutFileStream::write(const rmLPCChar src)
{
	writeString(src);
}

void rmOutFileStream::writeByte(rmI8 src)
{
	write(&src, 1);
}

void rmOutFileStream::writeShort(rmI16 src)
{
	(*handle16)(&src);
	write(&src, 2);
}

void rmOutFileStream::writeLong(rmI32 src)
{
	(*handle32)(&src);
	write(&src, 4);
}

void rmOutFileStream::writeLong64(rmI64 src)
{
	(*handle64)(&src);
	write(&src, 8);
}

void rmOutFileStream::writeFloat(rmF32 src)
{
	(*handle32)(&src);
	write(&src, 4);
}

void rmOutFileStream::writeDouble(rmF64 src)
{
	(*handle64)(&src);
	write(&src, 8);
}

void rmOutFileStream::writeBox(const rmBox & src)
{
    write(src.min.x);
    write(src.min.y);
    write(src.min.z);
    write(src.max.x);
    write(src.max.y);
    write(src.max.z);
}

void rmOutFileStream::writeString(const string & src)
{
	write(src.data(), src.size());
}

void rmOutFileStream::writeString(const  rmLPCChar src)
{
	write(src, strlen(src));
}

void rmOutFileStream::writeAsciiToUnicodeString(const string & src)
{
	rmLPCChar data = src.data();
	rmU32 size = src.size();
	if ((endBuffer - inBuffer) < (2 * size)) {
		if (bufferSize < (2 * size)) {
			OUT_SERR("String is too large")
		}
		else {
			flush();
		}
	}
	for (rmU32 i = 0; i < size; ++i) {
		*inBuffer_char = data[i];
		inBuffer[1] = 0;
		inBuffer += 2;
	}
}

void rmOutFileStream::writeAsciiToUnicodeString(const rmLPCChar src)
{
	string tmp(src);
	writeAsciiToUnicodeString(tmp);
}

rmOutFileStream & rmOutFileStream::operator << (const rmBool src)
{
	if (src) {
		writeString("true");
	}
	else {
		writeString("false");
	}
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmI8 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmU8 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmI16 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmU16 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmI32 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmU32 src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmI64 & src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmU64 & src)
{
	writeAsciiInt64(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmF32 src)
{
	writeAsciiDouble(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmF64 & src)
{
	writeAsciiDouble(src);
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const string & src)
{
	if (writeStringsQuotedSet == true) {
		string tmp("\"");
		tmp += src;
		tmp += "\"";
		write(tmp.data(), tmp.size());
	}
	else {
		write(src.data(), src.size());
	}
	return *this;
}

rmOutFileStream & rmOutFileStream::operator << (const rmLPCChar src)
{
	string tmp(src);
	return this->operator << (tmp);
}

rmOutFileStream & rmOutFileStream::operator << (rmOutFileStream & (* manip)(rmOutFileStream &))
{
	return (*manip)(*this);
}

rmOutFileStream & writeStringsQuoted(rmOutFileStream & s)
{
	s.writeStringsQuotedSet = true;
	return s;
}

rmOutFileStream & writeStringsUnquoted(rmOutFileStream & s)
{
	s.writeStringsQuotedSet = false;
	return s;
}

rmOutFileStream & newLine(rmOutFileStream & s)
{
	s.writeShort(2573);
	return s;
}

void rmOutFileStream::directWrite(const void * source, const rmU32 position, const rmU32 nBytes)
{
	outFileStream->seekp(position);
	outFileStream->write(static_cast<rmLPCChar>(source), nBytes);
}


void rmOutFileStream::writeAsciiInt64(const rmI64 & src)
{
	if ((endBuffer - inBuffer) < 30) {
		flush();
	}
	inBuffer += sprintf(inBuffer_char, "%d", src);
}

void rmOutFileStream::writeAsciiDouble(const rmF64 & src)
{
	if ((endBuffer - inBuffer) < 30) {
		flush();
	}
	inBuffer += sprintf(inBuffer_char, "%.8g", src);
}

//////////////////////////////////////////////////////
// end rmOutFileStream
//////////////////////////////////////////////////////

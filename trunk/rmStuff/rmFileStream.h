#pragma once

#include "rmDataTypes.h"
#include "rmOut.h"
#include <iostream>
#include <fstream>

using namespace std;

class rmFileStream
{
public:
	rmFileStream(void);
	virtual ~rmFileStream(void);

	virtual rmBool open(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode) = 0;
	virtual void close(void);

	rmBool failed(void);

	string getFileName(void);
	virtual rmU32 getFileSize(void);

	virtual rmU32 tell(void);
	virtual void seek(rmU32 position) = 0;
	virtual void skip(rmI32 nBytes) = 0;

	void bigEndianMode(void);
	void littleEndianMode(void);

protected:
	enum {BUFFER_PADDING = 128, STANDARD_BUFFER_SIZE = 200000, MUCH_DATA = 20000};

	void reset(void);

	static void noSwap(void * destination);
	static void swap16(void * destination);
	static void swap32(void * destination);
	static void swap64(void * destination);

	void (* handle16)(void *);
	void (* handle32)(void *);
	void (* handle64)(void *);

	ios * fileStream;
	string fileName;
	rmU32 fileSize;
	union
	{
		rmI8 * buffer;
		rmLPChar buffer_char;
	};
	union
	{
		rmI8 * inBuffer;
		rmU8 * inBuffer_u;
		rmLPChar inBuffer_char;
	};
	union
	{
		rmI8 * endBuffer;
		rmLPChar endBuffer_char;
	};
	rmU32 bufferPosition;
	rmU32 bufferEndPosition;
	rmU32 bufferSize;

	static rmOut out;

	static rmI8 swapBuff[8];
	static rmI8 * swapDest;
};

class rmInFileStream : public rmFileStream
{
public:
	rmInFileStream(void);
	rmInFileStream(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
				   RM_IOS_OPENMODE openMode = (ios_base::in | ios_base::binary));
	virtual ~rmInFileStream(void);

	virtual rmBool open(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
					  RM_IOS_OPENMODE openMode = (ios_base::in | ios_base::binary));

	virtual void seek(rmU32 position);
	virtual void skip(rmI32 nBytes);

	rmBool endOfFile(void);

	virtual void read(void * destination, rmU32 nBytes);

	void read(rmI8 & dest);
	void read(rmU8 & dest);
	void read(rmI16 & dest);
	void read(rmU16 & dest);
	void read(rmI32 & dest);
	void read(rmU32 & dest);
	void read(rmI64 & dest);
	void read(rmU64 & dest);

	rmI8 readByte(void);
	rmI16 readShort(void);
	rmI32 readLong(void);
	rmI64 readLong64(void);

	void read(rmF32 & dest);
	void read(rmF64 & dest);

	rmF32 readFloat(void);
	rmF64 readDouble(void);

	void read(rmBox & dest);
	rmBox readBox(void);

	void read(rmChar & dest);
	void readString(string & dest, rmU32 size);
	void readString(string & dest);
	void readUnicodeStringToAscii(string & dest, rmU32 size);
	void readUnicodeStringToAscii(string & dest);

	string readString(rmU32 size);
	string readString(void);
	string readUnicodeStringToAscii(rmU32 size);
	string readUnicodeStringToAscii(void);

	rmInFileStream & operator >> (rmI8 & dest);
	rmInFileStream & operator >> (rmU8 & dest);
	rmInFileStream & operator >> (rmI16 & dest);
	rmInFileStream & operator >> (rmU16 & dest);
	rmInFileStream & operator >> (rmI32 & dest);
	rmInFileStream & operator >> (rmU32 & dest);
	rmInFileStream & operator >> (rmI64 & dest);
	rmInFileStream & operator >> (rmU64 & dest);
	
	rmInFileStream & operator >> (rmF32 & dest);
	rmInFileStream & operator >> (rmF64 & dest);

	rmInFileStream & operator >> (rmChar & dest);
	rmInFileStream & operator >> (string & dest);

	rmInFileStream & operator >> (rmInFileStream & (* manip)(rmInFileStream &));

	friend rmInFileStream & considerQuotes(rmInFileStream & s);
	friend rmInFileStream & notConsiderQuotes(rmInFileStream & s);
	friend rmInFileStream & nextLine(rmInFileStream & s);

	rmBool skipCharacters(rmLPChar charactersToSkip, rmU8 arraySize, rmU32 maxPosition = -1);
	rmBool skipToCharacter(rmLPChar charactersToNotSkip, rmU8 arraySize, rmU32 maxPosition = -1);
	rmBool skipWhiteSpaceCharacters(rmU32 maxPosition = -1);
	rmBool skipToNextWhiteSpaceCharacter(rmU32 maxPosition = -1);
	rmBool findBytes(rmI8 * bytesToFind, rmU32 size, rmBool searchOverEndOfFile = false);
	rmBool findBytes(rmI8 * bytesToFind, rmU32 size, rmI8 ignoreRepresentation, rmBool searchOverEndOfFile = false);

	void initTokenStringList(rmLPCChar const * tokenStringList, rmU32 nTokens);
	void getToken(void * dest);

protected:
	void directRead(void * destination, rmU32 position, rmU32 nBytes);
	virtual void fillBuffer(rmU32 position);

	rmI32 readAsciiLong(void);
	rmF64 readAsciiDouble(void);

	ifstream* inFileStream;

	rmU32 maxBufferPosition;

	rmLPCChar const * tokenStringList;
	rmU32 nTokens;

	rmBool considerQuotesSet;
};

class rmMaxInFileStream : public rmInFileStream
{
public:
	rmMaxInFileStream(void);
	rmMaxInFileStream(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
					  RM_IOS_OPENMODE openMode = (ios_base::in | ios_base::binary));
	~rmMaxInFileStream(void);

	rmBool open(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
			  RM_IOS_OPENMODE openMode = (ios_base::in | ios_base::binary));
	void close(void);

	rmU32 tell(void);
	void seek(rmU32 position);
	void skip(rmI32 nBytes);

	void read(void * destination, rmU32 nBytes);

private:
	struct ProperBlock
	{
		rmU32 position_start;
		rmU32 position_end;
	};

	void readSkippingJunkBlocks(void * destination, rmU32 position, rmU32 nBytes, 
								ProperBlock* firstDataBlock = 0);
	void fillBuffer(rmU32 position);

	rmMaxInFileStream::ProperBlock * findProperBlock(rmU32 position);

	rmU32 nProperBlocks;
	ProperBlock * properBlocks;
	ProperBlock * maxFirstBufferedBlock;

	ProperBlock * firstBufferedProperBlock;
};

class rmOutFileStream : public rmFileStream
{
public:
	rmOutFileStream(void);
	rmOutFileStream(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
		RM_IOS_OPENMODE openMode = (ios_base::out | ios_base::binary));
	~rmOutFileStream(void);

	rmBool open(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, 
					  RM_IOS_OPENMODE openMode = (ios_base::out | ios_base::binary));
	void close(void);

	rmU32 getFileSize(void);

	void seek(rmU32 position);
	void skip(rmI32 nBytes);
	void flush(void);

	void write(const void* source, const rmU32 nBytes);

	void write(const rmI8 src);
	void write(const rmI16 src);
	void write(const rmI32 src);
	void write(const rmI64 & src);

	void write(const rmU8 src);
	void write(const rmU16 src);
	void write(const rmU32 src);
	void write(const rmU64 & src);

	void write(const rmF32 src);
	void write(const rmF64 & src);

	void write(const rmBox & src);

	void write(const string & src);
	void write(const rmLPCChar src);

	void writeByte(rmI8 src);
	void writeShort(rmI16 src);
	void writeLong(rmI32 src);
	void writeLong64(rmI64 src);

	void writeFloat(rmF32 src);
	void writeDouble(rmF64 src);

	void writeBox(const rmBox & src);

	void writeString(const string & src);
	void writeString(const rmLPCChar src);
	void writeAsciiToUnicodeString(const string & src);	
	void writeAsciiToUnicodeString(const rmLPCChar src);	

	rmOutFileStream & operator << (const rmBool src);

	rmOutFileStream & operator << (const rmI8 src);
	rmOutFileStream & operator << (const rmU8 src);
	rmOutFileStream & operator << (const rmI16 src);
	rmOutFileStream & operator << (const rmU16 src);
	rmOutFileStream & operator << (const rmI32 src);
	rmOutFileStream & operator << (const rmU32 src);
	rmOutFileStream & operator << (const rmI64 & src);
	rmOutFileStream & operator << (const rmU64 & src);
	
	rmOutFileStream & operator << (const rmF32 src);
	rmOutFileStream & operator << (const rmF64 & src);

	rmOutFileStream & operator << (const string & src);
	rmOutFileStream & operator << (const rmLPCChar src);

	rmOutFileStream & operator << (rmOutFileStream & (* manip)(rmOutFileStream &));

	friend rmOutFileStream & writeStringsQuoted(rmOutFileStream & s);
	friend rmOutFileStream & writeStringsUnquoted(rmOutFileStream & s);
	friend rmOutFileStream & newLine(rmOutFileStream & s);

protected:
	void directWrite(const void * source, const rmU32 position, const rmU32 nBytes);

	void writeAsciiInt64(const rmI64 & src);
	void writeAsciiDouble(const rmF64 & src);

	rmI32 writePosition;

	ostream * outFileStream;

	rmBool writeStringsQuotedSet;
};

rmInFileStream & considerQuotes(rmInFileStream & s);
rmInFileStream & notConsiderQuotes(rmInFileStream & s);
rmInFileStream & nextLine(rmInFileStream & s);
rmOutFileStream & writeStringsQuoted(rmOutFileStream & s);
rmOutFileStream & writeStringsUnquoted(rmOutFileStream & s);
rmOutFileStream & newLine(rmOutFileStream & s);

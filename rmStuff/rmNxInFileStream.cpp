#ifdef RM_PHYSX

#include "rmNxInFileStream.h"

rmNxInFileStream::rmNxInFileStream() {}

rmNxInFileStream::rmNxInFileStream(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode) : rmInFileStream(filename, bufferSize, openMode) {}

rmNxInFileStream::~rmNxInFileStream(void) {}

NxU8 rmNxInFileStream::readByte() const
{
    //return 0;
    return const_cast<rmNxInFileStream *>(this)->rmInFileStream::readByte();
    //return rmInFileStream::readByte();
}

NxU16 rmNxInFileStream::readWord() const
{
    return const_cast<rmNxInFileStream *>(this)->rmInFileStream::readShort();
}

NxU32 rmNxInFileStream::readDword() const
{
    return const_cast<rmNxInFileStream *>(this)->rmInFileStream::readLong();
}

float rmNxInFileStream::readFloat() const
{
    return const_cast<rmNxInFileStream *>(this)->rmInFileStream::readFloat();
}

double rmNxInFileStream::readDouble() const
{
    return const_cast<rmNxInFileStream *>(this)->rmInFileStream::readDouble();
}

void rmNxInFileStream::readBuffer(void * buffer, NxU32 size) const
{
    return const_cast<rmNxInFileStream *>(this)->read(buffer, size);
}


NxStream & rmNxInFileStream::storeByte(NxU8 b)
{
    return *this;
}

NxStream & rmNxInFileStream::storeWord(NxU16 w)
{
    return *this;
}

NxStream & rmNxInFileStream::storeDword(NxU32 d)
{
    return *this;
}

NxStream & rmNxInFileStream::storeFloat(NxReal f)
{
    return *this;
}

NxStream & rmNxInFileStream::storeDouble(NxF64 f)
{
    return *this;
}

NxStream & rmNxInFileStream::storeBuffer(const void * buffer, NxU32 size)
{
    return *this;
}

NxStream & rmNxInFileStream::storeByte(NxI8 b)
{
    return *this;
}

NxStream & rmNxInFileStream::storeWord(NxI16 w)
{
    return *this;
}

NxStream & rmNxInFileStream::storeDword(NxI32 d)
{
    return *this;
}

#endif // RM_PHYSX

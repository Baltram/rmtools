#ifdef RM_PHYSX

#include "rmNxOutFileStream.h"

rmNxOutFileStream::rmNxOutFileStream() {}

rmNxOutFileStream::rmNxOutFileStream(string filename, rmU32 bufferSize, RM_IOS_OPENMODE openMode) : rmOutFileStream(filename, bufferSize, openMode) {}

rmNxOutFileStream::~rmNxOutFileStream(void) {}

NxU8 rmNxOutFileStream::readByte() const
{
    return 0;
}

NxU16 rmNxOutFileStream::readWord() const
{
    return 0;
}

NxU32 rmNxOutFileStream::readDword() const
{
    return 0;
}

float rmNxOutFileStream::readFloat() const
{
    return 0;
}

double rmNxOutFileStream::readDouble() const
{
    return 0;
}

void rmNxOutFileStream::readBuffer(void * buffer, NxU32 size) const
{
    return;
}

NxStream & rmNxOutFileStream::storeByte(NxU8 b)
{
    write(b);
    return *this;
}

NxStream & rmNxOutFileStream::storeWord(NxU16 w)
{
    write(w);
    return *this;
}

NxStream & rmNxOutFileStream::storeDword(NxU32 d)
{
    write(d);
    return *this;
}

NxStream & rmNxOutFileStream::storeFloat(NxReal f)
{
    write(f);
    return *this;
}

NxStream & rmNxOutFileStream::storeDouble(NxF64 f)
{
    write(f);
    return *this;
}

NxStream & rmNxOutFileStream::storeBuffer(const void * buffer, NxU32 size)
{
    write(buffer, size);
    return *this;
}

NxStream & rmNxOutFileStream::storeByte(NxI8 b)
{
    write(b);
    return *this;
}

NxStream & rmNxOutFileStream::storeWord(NxI16 w)
{
    write(w);
    return *this;
}

NxStream & rmNxOutFileStream::storeDword(NxI32 d)
{
    write(d);
    return *this;
}

#endif // RM_PHYSX

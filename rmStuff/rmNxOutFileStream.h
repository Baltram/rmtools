#pragma once

#ifdef RM_PHYSX

#include "rmFileStream.h"
#include "NxPhysicsSDK.h"
#include "NxStream.h"


class rmNxOutFileStream : public rmOutFileStream, public NxStream
{
public:
    rmNxOutFileStream(void);
    rmNxOutFileStream(string filename, rmU32 bufferSize = STANDARD_BUFFER_SIZE, RM_IOS_OPENMODE openMode = (ios_base::in | ios_base::binary));

    virtual ~rmNxOutFileStream(void);

    virtual NxU8 readByte() const;
    virtual NxU16 readWord() const;
    virtual NxU32 readDword() const;
    virtual float readFloat() const;
    virtual double readDouble() const;
    virtual void readBuffer(void * buffer, NxU32 size) const;

    virtual NxStream & storeByte(NxU8 b);
    virtual NxStream & storeWord(NxU16 w);
    virtual NxStream & storeDword(NxU32 d);
    virtual NxStream & storeFloat(NxReal f);
    virtual NxStream & storeDouble(NxF64 f);
    virtual NxStream & storeBuffer(const void * buffer, NxU32 size);
    virtual NxStream & storeByte (NxI8 b);
    virtual NxStream & storeWord (NxI16 w);
    virtual NxStream & storeDword (NxI32 d);
};

#endif // RM_PHYSX

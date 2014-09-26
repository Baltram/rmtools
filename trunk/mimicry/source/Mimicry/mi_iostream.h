#ifndef MI_IOSTREAM_H_INCLUDED
#define MI_IOSTREAM_H_INCLUDED

enum mEStreamSeekMode
{
    mEStreamSeekMode_Begin,
    mEStreamSeekMode_Current,
    mEStreamSeekMode_End,
    mEStreamSeekMode_ForceDWORD = MI_FORCE_DWORD
};

template< mEStreamType M >
class mTIOStream :
    public mTIStream< M >,
    public mTOStream< M >
{
public:
    virtual ~mTIOStream( void );
public:
    virtual MIUInt   GetSize( void ) const = 0;
    virtual MIBool   IsAtEnd( void ) const = 0;
    virtual mEResult Seek( MIUInt a_uPosition, mEStreamSeekMode a_enuMode = mEStreamSeekMode_Begin ) = 0;
    virtual void     SetInvertEndianness( MIBool a_bMode );
    virtual mEResult Skip( MIInt a_iCount );
    virtual MIUInt   Tell( void ) const = 0;
};

typedef mTIOStream< mEStreamType_Binary >    mCIOStreamBinary;
typedef mTIOStream< mEStreamType_Formatted > mCIOStreamFormatted;

template< mEStreamType M, mEStreamType N > mTIStream< M > & operator >> ( mTIStream< M > & a_streamSource, mTOStream< N > & a_streamDest );
template< mEStreamType M, mEStreamType N > mTOStream< M > & operator << ( mTOStream< M > & a_streamDest, mTIStream< N > & a_streamSource );

#include "mi_iostream.inl"

#endif
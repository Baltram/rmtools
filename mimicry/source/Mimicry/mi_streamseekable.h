#ifndef MI_STREAMSEEKABLE_H_INCLUDED
#define MI_STREAMSEEKABLE_H_INCLUDED

enum mEStreamSeekMode
{
    mEStreamSeekMode_Begin,
    mEStreamSeekMode_Current,
    mEStreamSeekMode_End,
    mEStreamSeekMode_ForceDWORD = MI_FORCE_DWORD
};

class mCStreamSeekable
{
public:
    virtual MIUInt   GetSize( void ) const = 0;
    virtual mEResult Seek( MIUInt a_uPosition, mEStreamSeekMode a_enuMode = mEStreamSeekMode_Begin ) = 0;
    virtual mEResult Skip( MIInt a_iCount );
    virtual MIUInt   Tell( void ) const = 0;
    virtual         ~mCStreamSeekable( void );
};

template< mEStreamType M, mEStreamType N > mCIStream< M > & operator >> ( mCIStream< M > & a_streamSource, mCOStream< N > & a_streamDest );
template< mEStreamType M, mEStreamType N > mCOStream< M > & operator << ( mCOStream< M > & a_streamDest, mCIStream< N > & a_streamSource );

#include "mi_streamseekable.inl"

#endif
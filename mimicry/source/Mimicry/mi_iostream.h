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
    virtual MIU64    GetSize64( void ) const = 0;
    virtual MIBool   IsAtEnd( void ) const = 0;
    virtual mEResult Seek( MIU64 a_u64Position, mEStreamSeekMode a_enuMode = mEStreamSeekMode_Begin ) = 0;
    virtual void     SetInvertEndianness( MIBool a_bMode );
    virtual mEResult Skip( MII64 a_i64Count );
    virtual MIU64    Tell64( void ) const = 0;
public:
    MIUInt GetSize( void ) const;
    MIUInt Tell( void ) const;
};

typedef mTIOStream< mEStreamType_Binary >    mCIOStreamBinary;
typedef mTIOStream< mEStreamType_Formatted > mCIOStreamFormatted;

template< mEStreamType M, mEStreamType N > mTIStream< M > & operator >> ( mTIStream< M > & a_streamSource, mTOStream< N > & a_streamDest );
template< mEStreamType M, mEStreamType N > mTOStream< M > & operator << ( mTOStream< M > & a_streamDest, mTIStream< N > & a_streamSource );

#include "mi_iostream.inl"

#endif
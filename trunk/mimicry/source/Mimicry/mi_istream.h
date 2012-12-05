#ifndef MI_ISTREAM_H_INCLUDED
#define MI_ISTREAM_H_INCLUDED

template< mEStreamType M >
class mTIStream :
    public mCEndiannessHandler
{
public:
    virtual mEResult Read( MILPVoid a_pDest, MIUInt a_uSize ) = 0;
    virtual mEResult Read( mCString & a_strDest, MIUInt a_uSize ) = 0;
    virtual mEResult Read( mCString & a_strDest ) = 0;
    virtual         ~mTIStream< M >( void );
public:
    mTIStream< M > & operator >> ( MII8 & a_i8Dest );
    mTIStream< M > & operator >> ( MIU8 & a_u8Dest );
    mTIStream< M > & operator >> ( MII16 & a_i16Dest );
    mTIStream< M > & operator >> ( MIU16 & a_u16Dest );
    mTIStream< M > & operator >> ( MII32 & a_i32Dest );
    mTIStream< M > & operator >> ( MIU32 & a_u32Dest );
    mTIStream< M > & operator >> ( MIInt & a_iDest );
    mTIStream< M > & operator >> ( MIUInt & a_uDest );
    mTIStream< M > & operator >> ( MII64 & a_i64Dest );
    mTIStream< M > & operator >> ( MIU64 & a_u64Dest );
    mTIStream< M > & operator >> ( MIBool & a_bDest );
    mTIStream< M > & operator >> ( MIFloat & a_fDest );
    mTIStream< M > & operator >> ( MIDouble & a_dDest );
    mTIStream< M > & operator >> ( MIChar & a_cDest );
    mTIStream< M > & operator >> ( mCString & a_strDest );
public:
    MII8     ReadI8( void );
    MIU8     ReadU8( void );
    MII16    ReadI16( void );
    MIU16    ReadU16( void );
    MII32    ReadI32( void );
    MIU32    ReadU32( void );
    MIInt    ReadInt( void );
    MIUInt   ReadUInt( void );
    MII64    ReadI64( void );
    MIU64    ReadU64( void );
    MIBool   ReadBool( void );
    MIFloat  ReadFloat( void );
    MIDouble ReadDouble( void );
    MIChar   ReadChar( void );
    mCString ReadString( void );
    mCString ReadString( MIUInt a_uSize );
private:
    virtual mEResult ReadFormatted( MILPVoid a_pDest, MILPCChar a_pcFormat ) = 0;
};

typedef mTIStream< mEStreamType_Binary >    mCIStreamBinary;
typedef mTIStream< mEStreamType_Formatted > mCIStreamFormatted;

template< mEStreamType M > mTIStream< M > & operator << ( MII8 & a_i8Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIU8 & a_u8Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MII16 & a_i16Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIU16 & a_u16Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MII32 & a_i32Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIU32 & a_u32Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIInt & a_iDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIUInt & a_uDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MII64 & a_i64Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIU64 & a_u64Dest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIBool & a_bDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIFloat & a_fDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIDouble & a_dDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( MIChar & a_cDest, mTIStream< M > & a_streamSource );
template< mEStreamType M > mTIStream< M > & operator << ( mCString & a_strDest, mTIStream< M > & a_streamSource );

#include "mi_istream.inl"

#endif

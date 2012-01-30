#ifndef MI_ISTREAM_H_INCLUDED
#define MI_ISTREAM_H_INCLUDED

template< mEStreamType M >
class mCIStream :
    public virtual mCEndiannessHandler
{
public:
    virtual mEResult Read( MILPVoid a_pDest, MIUInt a_uSize ) = 0;
    virtual mEResult Read( mCString & a_strDest, MIUInt a_uSize ) = 0;
    virtual mEResult Read( mCString & a_strDest ) = 0;
    virtual         ~mCIStream< M >( void );
public:
    mCIStream< M > & operator >> ( MII8 & a_i8Dest );
    mCIStream< M > & operator >> ( MIU8 & a_u8Dest );
    mCIStream< M > & operator >> ( MII16 & a_i16Dest );
    mCIStream< M > & operator >> ( MIU16 & a_u16Dest );
    mCIStream< M > & operator >> ( MII32 & a_i32Dest );
    mCIStream< M > & operator >> ( MIU32 & a_u32Dest );
    mCIStream< M > & operator >> ( MIInt & a_iDest );
    mCIStream< M > & operator >> ( MIUInt & a_uDest );
    mCIStream< M > & operator >> ( MII64 & a_i64Dest );
    mCIStream< M > & operator >> ( MIU64 & a_u64Dest );
    mCIStream< M > & operator >> ( MIBool & a_bDest );
    mCIStream< M > & operator >> ( MIFloat & a_fDest );
    mCIStream< M > & operator >> ( MIDouble & a_dDest );
    mCIStream< M > & operator >> ( MIChar & a_cDest );
    mCIStream< M > & operator >> ( mCString & a_strDest );
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
private:
    virtual mEResult ReadFormatted( MILPVoid a_pDest, MILPCChar a_pcFormat ) = 0;
};

typedef mCIStream< mEStreamType_Binary >    mCIStreamBinary;
typedef mCIStream< mEStreamType_Formatted > mCIStreamFormatted;

template< mEStreamType M > mCIStream< M > & operator << ( MII8 & a_i8Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIU8 & a_u8Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MII16 & a_i16Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIU16 & a_u16Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MII32 & a_i32Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIU32 & a_u32Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIInt & a_iDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIUInt & a_uDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MII64 & a_i64Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIU64 & a_u64Dest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIBool & a_bDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIFloat & a_fDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIDouble & a_dDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( MIChar & a_cDest, mCIStream< M > & a_streamSource );
template< mEStreamType M > mCIStream< M > & operator << ( mCString & a_strDest, mCIStream< M > & a_streamSource );

#include "mi_istream.inl"

#endif
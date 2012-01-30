#ifndef MI_OSTREAM_H_INCLUDED
#define MI_OSTREAM_H_INCLUDED

template< mEStreamType M >
class mTOStream :
    public mCEndiannessHandler
{
public:
    virtual mEResult Write( MILPCVoid a_pSource, MIUInt a_uSize ) = 0;
    virtual mEResult Write( mCString const & a_strSource ) = 0;
    virtual         ~mTOStream< M >( void );
public:
    mTOStream< M > & operator << ( MII8 a_i8Source );
    mTOStream< M > & operator << ( MIU8 a_u8Source );
    mTOStream< M > & operator << ( MII16 a_i16Source );
    mTOStream< M > & operator << ( MIU16 a_u16Source );
    mTOStream< M > & operator << ( MII32 a_i32Source );
    mTOStream< M > & operator << ( MIU32 a_u32Source );
    mTOStream< M > & operator << ( MIInt a_iSource );
    mTOStream< M > & operator << ( MIUInt a_uSource );
    mTOStream< M > & operator << ( MII64 a_i64Source );
    mTOStream< M > & operator << ( MIU64 a_u64Source );
    mTOStream< M > & operator << ( MIBool a_bSource );
    mTOStream< M > & operator << ( MIFloat a_fSource );
    mTOStream< M > & operator << ( MIDouble a_dSource );
    mTOStream< M > & operator << ( MIChar a_cSource );
    mTOStream< M > & operator << ( MILPCChar a_pcSource );
    mTOStream< M > & operator << ( mCString const & a_strSource );
private:
    virtual mEResult WriteFormatted( MILPCVoid a_pSource, MILPCChar a_pcFormat ) = 0;
};

typedef mTOStream< mEStreamType_Binary >    mCOStreamBinary;
typedef mTOStream< mEStreamType_Formatted > mCOStreamFormatted;

template< mEStreamType M > mTOStream< M > & operator >> ( MII8 a_i8Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIU8 a_u8Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MII16 a_i16Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIU16 a_u16Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MII32 a_i32Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIU32 a_u32Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIInt a_iSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIUInt a_uSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MII64 a_i64Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIU64 a_u64Source, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIBool a_bSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIFloat a_fSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIDouble a_dSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MIChar a_cSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( MILPCChar a_pcSource, mTOStream< M > & a_streamDest );
template< mEStreamType M > mTOStream< M > & operator >> ( mCString const & a_strSource, mTOStream< M > & a_streamDest );

#include "mi_ostream.inl"

#endif
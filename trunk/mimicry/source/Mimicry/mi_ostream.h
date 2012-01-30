#ifndef MI_OSTREAM_H_INCLUDED
#define MI_OSTREAM_H_INCLUDED

template< mEStreamType M >
class mCOStream :
    public virtual mCEndiannessHandler
{
public:
    virtual mEResult Write( MILPCVoid a_pSource, MIUInt a_uSize ) = 0;
    virtual mEResult Write( mCString const & a_strSource ) = 0;
    virtual         ~mCOStream< M >( void );
public:
    mCOStream< M > & operator << ( MII8 a_i8Source );
    mCOStream< M > & operator << ( MIU8 a_u8Source );
    mCOStream< M > & operator << ( MII16 a_i16Source );
    mCOStream< M > & operator << ( MIU16 a_u16Source );
    mCOStream< M > & operator << ( MII32 a_i32Source );
    mCOStream< M > & operator << ( MIU32 a_u32Source );
    mCOStream< M > & operator << ( MIInt a_iSource );
    mCOStream< M > & operator << ( MIUInt a_uSource );
    mCOStream< M > & operator << ( MII64 a_i64Source );
    mCOStream< M > & operator << ( MIU64 a_u64Source );
    mCOStream< M > & operator << ( MIBool a_bSource );
    mCOStream< M > & operator << ( MIFloat a_fSource );
    mCOStream< M > & operator << ( MIDouble a_dSource );
    mCOStream< M > & operator << ( MIChar a_cSource );
    mCOStream< M > & operator << ( MILPCChar a_pcSource );
    mCOStream< M > & operator << ( mCString const & a_strSource );
private:
    virtual mEResult WriteFormatted( MILPCVoid a_pSource, MILPCChar a_pcFormat ) = 0;
};

typedef mCOStream< mEStreamType_Binary >    mCOStreamBinary;
typedef mCOStream< mEStreamType_Formatted > mCOStreamFormatted;

template< mEStreamType M > mCOStream< M > & operator >> ( MII8 a_i8Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIU8 a_u8Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MII16 a_i16Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIU16 a_u16Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MII32 a_i32Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIU32 a_u32Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIInt a_iSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIUInt a_uSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MII64 a_i64Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIU64 a_u64Source, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIBool a_bSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIFloat a_fSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIDouble a_dSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MIChar a_cSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( MILPCChar a_pcSource, mCOStream< M > & a_streamDest );
template< mEStreamType M > mCOStream< M > & operator >> ( mCString const & a_strSource, mCOStream< M > & a_streamDest );

#include "mi_ostream.inl"

#endif
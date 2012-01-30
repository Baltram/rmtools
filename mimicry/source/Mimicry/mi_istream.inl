template<> inline
mCIStreamBinary::~mCIStreamBinary( void )
{
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MII8 & a_i8Dest )
{
    Read( &a_i8Dest, static_cast< MIUInt >( sizeof( MII8 ) ) );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIU8 & a_u8Dest )
{
    Read( &a_u8Dest, static_cast< MIUInt >( sizeof( MIU8 ) ) );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MII16 & a_i16Dest )
{
    Read( &a_i16Dest, static_cast< MIUInt >( sizeof( MII16 ) ) );
    Handle16( &a_i16Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIU16 & a_u16Dest )
{
    Read( &a_u16Dest, static_cast< MIUInt >( sizeof( MIU16 ) ) );
    Handle16( &a_u16Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MII32 & a_i32Dest )
{
    Read( &a_i32Dest, static_cast< MIUInt >( sizeof( MII32 ) ) );
    Handle32( &a_i32Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIU32 & a_u32Dest )
{
    Read( &a_u32Dest, static_cast< MIUInt >( sizeof( MIU32 ) ) );
    Handle32( &a_u32Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIInt & a_iDest )
{
    Read( &a_iDest, static_cast< MIUInt >( sizeof( MIInt ) ) );
    Handle32( &a_iDest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIUInt & a_uDest )
{
    Read( &a_uDest, static_cast< MIUInt >( sizeof( MIUInt ) ) );
    Handle32( &a_uDest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MII64 & a_i64Dest )
{
    Read( &a_i64Dest, static_cast< MIUInt >( sizeof( MII64 ) ) );
    Handle64( &a_i64Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIU64 & a_u64Dest )
{
    Read( &a_u64Dest, static_cast< MIUInt >( sizeof( MIU64 ) ) );
    Handle64( &a_u64Dest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIBool & a_bDest )
{
    Read( &a_bDest, static_cast< MIUInt >( sizeof( MIBool ) ) );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIFloat & a_fDest )
{
    Read( &a_fDest, static_cast< MIUInt >( sizeof( MIFloat ) ) );
    Handle32( &a_fDest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIDouble & a_dDest )
{
    Read( &a_dDest, static_cast< MIUInt >( sizeof( MIDouble ) ) );
    Handle64( &a_dDest );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( MIChar & a_cDest )
{
    Read( &a_cDest, static_cast< MIUInt >( sizeof( MIChar ) ) );
    return *this;
}

template<> inline
mCIStreamBinary & mCIStreamBinary::operator >> ( mCString & a_strDest )
{
    Read( a_strDest );
    return *this;
}

template< mEStreamType M >
MII8 mTIStream< M >::ReadI8( void )
{
    MII8 i8Result;
    *this >> i8Result;
    return i8Result;
}

template< mEStreamType M >
MIU8 mTIStream< M >::ReadU8( void )
{
    MIU8 u8Result;
    *this >> u8Result;
    return u8Result;
}

template< mEStreamType M >
MII16 mTIStream< M >::ReadI16( void )
{
    MII16 i16Result;
    *this >> i16Result;
    return i16Result;
}

template< mEStreamType M >
MIU16 mTIStream< M >::ReadU16( void )
{
    MIU16 u16Result;
    *this >> u16Result;
    return u16Result;
}

template< mEStreamType M >
MII32 mTIStream< M >::ReadI32( void )
{
    MII32 i32Result;
    *this >> i32Result;
    return i32Result;
}

template< mEStreamType M >
MIU32 mTIStream< M >::ReadU32( void )
{
    MIU32 u32Result;
    *this >> u32Result;
    return u32Result;
}

template< mEStreamType M >
MIInt mTIStream< M >::ReadInt( void )
{
    MIInt iResult;
    *this >> iResult;
    return iResult;
}

template< mEStreamType M >
MIUInt mTIStream< M >::ReadUInt( void )
{
    MIUInt uResult;
    *this >> uResult;
    return uResult;
}

template< mEStreamType M >
MII64 mTIStream< M >::ReadI64( void )
{
    MII64 i64Result;
    *this >> i64Result;
    return i64Result;
}

template< mEStreamType M >
MIU64 mTIStream< M >::ReadU64( void )
{
    MIU64 u64Result;
    *this >> u64Result;
    return u64Result;
}

template< mEStreamType M >
MIBool mTIStream< M >::ReadBool( void )
{
    MIBool bResult;
    *this >> bResult;
    return bResult;
}

template< mEStreamType M >
MIFloat mTIStream< M >::ReadFloat( void )
{
    MIFloat fResult;
    *this >> fResult;
    return fResult;
}

template< mEStreamType M >
MIDouble mTIStream< M >::ReadDouble( void )
{
    MIDouble dResult;
    *this >> dResult;
    return dResult;
}

template< mEStreamType M >
MIChar mTIStream< M >::ReadChar( void )
{
    MIChar cResult;
    *this >> cResult;
    return cResult;
}

template< mEStreamType M >
mCString mTIStream< M >::ReadString( void )
{
    mCString strResult;
    *this >> strResult;
    return strResult;
}

template<> inline
mCIStreamFormatted::~mCIStreamFormatted( void )
{
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MII8 & a_i8Dest )
{
    MIInt iTemp;
    ReadFormatted( &iTemp, "  %i" );
    a_i8Dest = static_cast< MII8 >( iTemp );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIU8 & a_u8Dest )
{
    MIUInt uTemp;
    ReadFormatted( &uTemp, "  %u" );
    a_u8Dest = static_cast< MIU8 >( uTemp );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MII16 & a_i16Dest )
{
    MIInt iTemp;
    ReadFormatted( &iTemp, "  %i" );
    a_i16Dest = static_cast< MII16 >( iTemp );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIU16 & a_u16Dest )
{
    MIUInt uTemp;
    ReadFormatted( &uTemp, "  %u" );
    a_u16Dest = static_cast< MIU16 >( uTemp );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MII32 & a_i32Dest )
{
    ReadFormatted( &a_i32Dest, "  %i" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIU32 & a_u32Dest )
{
    ReadFormatted( &a_u32Dest, "  %u" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIInt & a_iDest )
{
    ReadFormatted( &a_iDest, "  %i" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIUInt & a_uDest )
{
    ReadFormatted( &a_uDest, "  %u" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MII64 & a_i64Dest )
{
    a_i64Dest = 0;
    ReadFormatted( &a_i64Dest, "  %i" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIU64 & a_u64Dest )
{
    a_u64Dest = 0;
    ReadFormatted( &a_u64Dest, "  %u" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIBool & a_bDest )
{
    mCString strTemp;
    Read( strTemp );
    a_bDest = ( strTemp.CompareNoCase( "true" ) == 0 ) ? MITrue : MIFalse;
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIFloat & a_fDest )
{
    MIDouble dTemp;
    ReadFormatted( &dTemp, " %lf" );
    a_fDest = static_cast< MIFloat >( dTemp );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIDouble & a_dDest )
{
    ReadFormatted( &a_dDest, " %lf" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( MIChar & a_cDest )
{
    ReadFormatted( &a_cDest, "  %c" );
    return *this;
}

template<> inline
mCIStreamFormatted & mCIStreamFormatted::operator >> ( mCString & a_strDest )
{
    Read( a_strDest );
    return *this;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MII8 & a_i8Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_i8Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIU8 & a_u8Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_u8Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MII16 & a_i16Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_i16Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIU16 & a_u16Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_u16Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MII32 & a_i32Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_i32Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIU32 & a_u32Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_u32Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIInt & a_iDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_iDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIUInt & a_uDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_uDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MII64 & a_i64Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_i64Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIU64 & a_u64Dest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_u64Dest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIBool & a_bDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_bDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIFloat & a_fDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_fDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIDouble & a_dDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_dDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( MIChar & a_cDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_cDest;
    return a_streamSource;
}

template< mEStreamType M >
mTIStream< M > & operator << ( mCString & a_strDest, mTIStream< M > & a_streamSource )
{
    a_streamSource >> a_strDest;
    return a_streamSource;
}
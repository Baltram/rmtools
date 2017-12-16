template<> inline
mCOStreamBinary::~mCOStreamBinary( void )
{
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MII8 a_i8Source )
{
    Write( &a_i8Source, static_cast< MIUInt >( sizeof( MII8 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIU8 a_u8Source )
{
    Write( &a_u8Source, static_cast< MIUInt >( sizeof( MIU8 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MII16 a_i16Source )
{
    Handle16( &a_i16Source );
    Write( &a_i16Source, static_cast< MIUInt >( sizeof( MII16 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIU16 a_u16Source )
{
    Handle16( &a_u16Source );
    Write( &a_u16Source, static_cast< MIUInt >( sizeof( MIU16 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MII32 a_i32Source )
{
    Handle32( &a_i32Source );
    Write( &a_i32Source, static_cast< MIUInt >( sizeof( MII32 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIU32 a_u32Source )
{
    Handle32( &a_u32Source );
    Write( &a_u32Source, static_cast< MIUInt >( sizeof( MIU32 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MII64 a_i64Source )
{
    Handle64( &a_i64Source );
    Write( &a_i64Source, static_cast< MIUInt >( sizeof( MII64 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIU64 a_u64Source )
{
    Handle64( &a_u64Source );
    Write( &a_u64Source, static_cast< MIUInt >( sizeof( MIU64 ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIBool a_bSource )
{
    Write( &a_bSource, static_cast< MIUInt >( sizeof( MIBool ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIFloat a_fSource )
{
    Handle32( &a_fSource );
    Write( &a_fSource, static_cast< MIUInt >( sizeof( MIFloat ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIDouble a_dSource )
{
    Handle64( &a_dSource );
    Write( &a_dSource, static_cast< MIUInt >( sizeof( MIDouble ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MIChar a_cSource )
{
    Write( &a_cSource, static_cast< MIUInt >( sizeof( MIChar ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( MILPCChar a_pcSource )
{
    Write( a_pcSource, static_cast< MIUInt >( g_strlen( a_pcSource ) ) );
    return *this;
}

template<> inline
mCOStreamBinary & mCOStreamBinary::operator << ( mCString const & a_strSource )
{
    Write( a_strSource );
    return *this;
}

template<> inline
mCOStreamFormatted::~mCOStreamFormatted( void )
{
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MII8 a_i8Source )
{
    MIInt iTemp = a_i8Source;
    WriteFormatted( &iTemp, "%i" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIU8 a_u8Source )
{
    MIUInt uTemp = a_u8Source;
    WriteFormatted( &uTemp, "%u" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MII16 a_i16Source )
{
    MIInt iTemp = a_i16Source;
    WriteFormatted( &iTemp, "%i" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIU16 a_u16Source )
{
    MIUInt uTemp = a_u16Source;
    WriteFormatted( &uTemp, "%u" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MII32 a_i32Source )
{
    WriteFormatted( &a_i32Source, "%i" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIU32 a_u32Source )
{
    WriteFormatted( &a_u32Source, "%u" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MII64 a_i64Source )
{
    WriteFormatted( &a_i64Source, "%i" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIU64 a_u64Source )
{
    WriteFormatted( &a_u64Source, "%u" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIBool a_bSource )
{
    *this << ( a_bSource ? "true" : "false" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIFloat a_fSource )
{
    MIDouble dTemp = a_fSource;
    WriteFormatted( &dTemp, "%g" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIDouble a_dSource )
{
    WriteFormatted( &a_dSource, "%g" );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MIChar a_cSource )
{
    Write( &a_cSource, 1 );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( MILPCChar a_pcSource )
{
    Write( a_pcSource, static_cast< MIUInt >( g_strlen( a_pcSource ) ) );
    return *this;
}

template<> inline
mCOStreamFormatted & mCOStreamFormatted::operator << ( mCString const & a_strSource )
{
    Write( a_strSource );
    return *this;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MII8 a_i8Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_i8Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIU8 a_u8Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_u8Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MII16 a_i16Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_i16Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIU16 a_u16Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_u16Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MII32 a_i32Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_i32Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIU32 a_u32Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_u32Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MII64 a_i64Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_i64Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIU64 a_u64Source, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_u64Source;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIBool a_bSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_bSource;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIFloat a_fSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_fSource;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIDouble a_dSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_dSource;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MIChar a_cSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_cSource;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( MILPCChar a_pcSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_pcSource;
    return a_streamDest;
}

template< mEStreamType M >
mTOStream< M > & operator >> ( mCString const & a_strSource, mTOStream< M > & a_streamDest )
{
    a_streamDest << a_strSource;
    return a_streamDest;
}
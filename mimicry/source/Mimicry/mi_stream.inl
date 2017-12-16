inline
mCEndiannessHandler::~mCEndiannessHandler( void )
{
}

inline
mCEndiannessHandler::mCEndiannessHandler( void ) :
    m_bInvertEndianness( MIFalse )
{
}

inline
void mCEndiannessHandler::SetInvertEndianness( MIBool a_bMode )
{
    m_bInvertEndianness = a_bMode;
}

inline
void mCEndiannessHandler::Handle16( MILPVoid a_pDest )
{
#ifdef _MSC_VER
    if ( m_bInvertEndianness )
        *static_cast< MIU16 * >( a_pDest ) = _byteswap_ushort( *static_cast< MIU16 * >( a_pDest ) );
#else
    if ( m_bInvertEndianness )
    {
        MIU16 & u16Value = *static_cast< MIU16 * >( a_pDest );
        u16Value = ( u16Value & 0x00FF ) << 8 | ( u16Value & 0xFF00 ) >> 8;
    }
#endif
}

inline
void mCEndiannessHandler::Handle32( MILPVoid a_pDest )
{
#ifdef _MSC_VER
    if ( m_bInvertEndianness )
        *static_cast< MIU32 * >( a_pDest ) = _byteswap_ulong( *static_cast< MIU32 * >( a_pDest ) );
#else
    if ( m_bInvertEndianness )
    {
        MIU32 & u32Value = *static_cast< MIU32 * >( a_pDest );
        u32Value = ( u32Value & 0x000000FF ) << 24 | ( u32Value & 0x0000FF00 ) << 8 | ( u32Value & 0x00FF0000 ) >> 8 | ( u32Value & 0xFF000000 ) >> 24;
    }
#endif
}

inline
void mCEndiannessHandler::Handle64( MILPVoid a_pDest )
{
#ifdef _MSC_VER
    if ( m_bInvertEndianness )
        *static_cast< MIU64 * >( a_pDest ) = _byteswap_uint64( *static_cast< MIU64 * >( a_pDest ) );
#else
    if ( m_bInvertEndianness )
    {
        MIU64 & u64Value = *static_cast< MIU64 * >( a_pDest );
        u64Value = ( u64Value & 0x00000000000000FF ) << 56 | ( u64Value & 0x000000000000FF00 ) << 40 | ( u64Value & 0x0000000000FF0000 ) << 24 | ( u64Value & 0x00000000FF000000 ) << 8 | ( u64Value & 0x000000FF00000000 ) >> 8 | ( u64Value & 0x0000FF0000000000 ) >> 24 | ( u64Value & 0x00FF000000000000 ) >> 40 | ( u64Value & 0xFF00000000000000 ) >> 56;
    }
#endif
}
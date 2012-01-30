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
    MI_ASSERT_MSVC
    if ( m_bInvertEndianness )
        *static_cast< MIU16 * >( a_pDest ) = _byteswap_ushort( *static_cast< MIU16 * >( a_pDest ) );
}

inline
void mCEndiannessHandler::Handle32( MILPVoid a_pDest )
{
    MI_ASSERT_MSVC
    if ( m_bInvertEndianness )
        *static_cast< MIU32 * >( a_pDest ) = _byteswap_ulong( *static_cast< MIU32 * >( a_pDest ) );
}

inline
void mCEndiannessHandler::Handle64( MILPVoid a_pDest )
{
    MI_ASSERT_MSVC
    if ( m_bInvertEndianness )
        *static_cast< MIU64 * >( a_pDest ) = _byteswap_uint64( *static_cast< MIU64 * >( a_pDest ) );
}
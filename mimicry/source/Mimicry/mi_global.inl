template< typename T >
T const & g_limit ( T const & a_Number, T const & a_LowerBound, T const & a_UpperBound )
{
    return g_max( a_LowerBound, g_min( a_UpperBound, a_Number ) );
}

template< typename T >
T const & g_max ( T const & a_1, T const & a_2 )
{
    return ( a_1 > a_2 ) ? a_1 : a_2;
}

template< typename T >
T const & g_min ( T const & a_1, T const & a_2 )
{
    return ( a_1 < a_2 ) ? a_1 : a_2;
}

MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_BEGIN

template< typename T >
void g_memswap( T & a_1, T & a_2 )
{
    if ( &a_1 == &a_2 )
        return;
    if ( sizeof( T ) > 12 )
    {
        MIByte arrBuffer[ sizeof( T ) ];
        g_memcpy( arrBuffer, &a_1, sizeof( T ) );
        g_memcpy( &a_1, &a_2, sizeof( T ) );
        g_memcpy( &a_2, arrBuffer, sizeof( T ) );
    }
    else
    {
        MIByte byTemp;
        MIByte * p1 = reinterpret_cast< MIByte * >( &a_1 );
        MIByte * p2 = reinterpret_cast< MIByte * >( &a_2 );
        for ( MIUInt u = sizeof( T ); u--; )
        {
            byTemp = *p1;
            *p1++ = *p2;
            *p2++ = byTemp;
        }
    }
};

MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_END

template< typename T >
void g_reorder( T * a_pElements, MIUInt const * a_pPattern, MIUInt a_uElementCount )
{
    T * pBuffer = static_cast< T * >( g_malloc( sizeof( T ) * a_uElementCount ) );
    g_memcpy( pBuffer, a_pElements, sizeof( T ) * a_uElementCount );
    for ( MIUInt u = a_uElementCount; u--; g_memswap( pBuffer[ a_pPattern[ u ] ], a_pElements[ u ] ) );
    g_free( pBuffer );
}

template< typename T >
void g_swap( T & a_1, T & a_2 )
{
    T Temp = a_1;
    a_1 = a_2;
    a_2 = Temp;
};

inline
MIBool g_getbit( MILPVoid a_pBase, MIUInt uOffset )
{
    return ( static_cast< MILPByte >( a_pBase )[ uOffset / 8 ] & ( ( MIByte ) 1 << uOffset % 8 ) ) != 0;
}

inline
void g_setbit( MILPVoid a_pBase, MIUInt uOffset )
{
    static_cast< MILPByte >( a_pBase )[ uOffset / 8 ] |= ( ( MIByte ) 1 << uOffset % 8 );
}

inline
void g_unsetbit( MILPVoid a_pBase, MIUInt uOffset )
{
    static_cast< MILPByte >( a_pBase )[ uOffset / 8 ] &= ~( ( MIByte ) 1 << uOffset % 8 );
}

inline
void g_free( MILPVoid a_pDest )
{
    return ( *g_funcFree )( a_pDest );
}

inline
MILPVoid g_malloc( MISize a_sizeSize )
{
    return ( *g_funcMalloc )( a_sizeSize );
}

inline
MILPCVoid g_memchr( MILPCVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum )
{
    return ( *g_funcMemchr_c )( a_pBlock, a_iValue, a_sizeNum );
}

inline
MILPVoid g_memchr( MILPVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum )
{
    return ( *g_funcMemchr )( a_pBlock, a_iValue, a_sizeNum );
}

inline
MIInt g_memcmp( MILPCVoid a_p1, MILPCVoid a_p2, MISize a_sizeSize )
{
    return ( *g_funcMemcmp )( a_p1, a_p2, a_sizeSize );
}

inline
MILPVoid g_memcpy( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize )
{
    return ( *g_funcMemcpy )( a_pDestination, a_pSource, a_sizeSize );
}

inline
MILPVoid g_memmove( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize )
{
    return ( *g_funcMemmove )( a_pDestination, a_pSource, a_sizeSize );
}

inline
MILPVoid g_memset( MILPVoid a_pDestination, MIInt a_iValue, MISize a_sizeSize )
{
    return ( *g_funcMemset )( a_pDestination, a_iValue, a_sizeSize );
}

inline
MILPVoid g_realloc( MILPVoid a_pDest, MISize a_sizeSize )
{
    return ( *g_funcRealloc )( a_pDest, a_sizeSize );
}

inline
MILPCChar g_strchr( MILPCChar a_pcText, MIInt a_iValue )
{
    return ( *g_funcStrchr_c )( a_pcText, a_iValue );
}

inline
MILPChar g_strchr( MILPChar a_pcText, MIInt a_iValue )
{
    return ( *g_funcStrchr )( a_pcText, a_iValue );
}

inline
MIInt g_strcmp( MILPCChar a_pcText1, MILPCChar a_pcText2 )
{
    return ( *g_funcStrcmp )( a_pcText1, a_pcText2 );
}

inline
MILPChar g_strcpy( MILPChar a_pcDestination, MILPCChar a_pcSource )
{
    return ( *g_funcStrcpy )( a_pcDestination, a_pcSource );
}

inline
MISize g_strlen( MILPCChar a_pcText )
{
    return ( *g_funcStrlen )( a_pcText );
}

inline
MIInt g_strncmp( MILPCChar a_pcText1, MILPCChar a_pcText2, MISize a_sizeNum )
{
    return ( *g_funcStrncmp )( a_pcText1, a_pcText2, a_sizeNum );
}
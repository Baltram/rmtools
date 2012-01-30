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
void g_memswap( T & a_1, T & a_2 )
{
    MIByte arrBuffer[ sizeof( T ) ];
    g_memcpy( arrBuffer, &a_1, sizeof( T ) );
    g_memcpy( &a_1, &a_2, sizeof( T ) );
    g_memcpy( &a_2, arrBuffer, sizeof( T ) );
};

template< typename T >
T const & g_min ( T const & a_1, T const & a_2 )
{
    return ( a_1 < a_2 ) ? a_1 : a_2;
}

template< typename T >
void g_swap( T & a_1, T & a_2 )
{
    T Temp = a_1;
    a_1 = a_2;
    a_2 = Temp;
};

inline
MILPCVoid g_memchr ( MILPCVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum )
{
    return ( *g_funcMemchr_c )( a_pBlock, a_iValue, a_sizeNum );
}

inline
MILPVoid g_memchr ( MILPVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum )
{
    return ( *g_funcMemchr )( a_pBlock, a_iValue, a_sizeNum );
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
MIInt g_strncmp( MILPCChar a_pcText1, MILPCChar a_pcText2, MISize a_sizeNum )
{
    return ( *g_funcStrncmp )( a_pcText1, a_pcText2, a_sizeNum );
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
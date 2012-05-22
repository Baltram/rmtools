inline
MIU32 g_djb2( MILPCChar a_pcText )
{
    MIU32 uResult = 5381;
    if ( a_pcText )
        while ( *a_pcText )
            uResult += ( uResult << 5 ) + *a_pcText++;
    return uResult;
}

template< typename K, typename T, class C >
mTMap< K, T, C >::mTMap( MIUInt a_uMinCapacity ) :
    m_uElementCount( 0 ),
    m_u32BucketMask( 0 )
{
    Reserve( g_max( a_uMinCapacity, 4U ) );
}

template< typename K, typename T, class C >
mTMap< K, T, C >::mTMap( mTMap< K, T, C > const & a_mapSource ) :
    m_arrBuckets( a_mapSource.m_arrBuckets ),
    m_uElementCount( a_mapSource.m_uElementCount ),
    m_u32BucketMask( a_mapSource.m_u32BucketMask )
{
}

template< typename K, typename T, class C >
T & mTMap< K, T, C >::operator [] ( K const & a_Key )
{
    return Add( 0, a_Key );
}

template< typename K, typename T, class C >
T const & mTMap< K, T, C >::operator [] ( K const & a_Key ) const
{
    return Add( 0, a_Key );
}

template< typename K, typename T, class C >
mTMap< K, T, C > & mTMap< K, T, C >::operator = ( mTMap< K, T, C > const & a_mapSource )
{
    if ( this == &a_mapSource )
        return *this;
    mTMap< K, T, C > mapNew( a_mapOther );
    Swap( mapNew );
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator mTMap< K, T, C >::Begin( void ) const
{
    return CConstIterator( &m_arrBuckets, 0 );
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator mTMap< K, T, C >::Begin( void )
{
    return CIterator( &m_arrBuckets, 0 );
}

template< typename K, typename T, class C >
void mTMap< K, T, C >::Clear( void )
{
    MIUInt uBucketCount = m_arrBuckets.GetCount();
    m_arrBuckets.Resize( 0 );
    m_arrBuckets.Resize( uBucketCount );
    m_uElementCount = 0;
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::Contains( K const & a_Key ) const
{
    MIU32 u32Hash = 0;
    SBucket const * pBaseBucket = 0;
    SBucket const * pBucket = 0;
    return Locate( a_Key, u32Hash, pBaseBucket, pBucket );
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator mTMap< K, T, C >::End( void ) const
{
    return CConstIterator( &m_arrBuckets, ( m_arrBuckets.GetCount() - 1 ) );
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator mTMap< K, T, C >::End( void )
{
    MIUInt const uCount = m_arrBuckets.GetCount();
    return CIterator( &m_arrBuckets, ( uCount ? uCount - 1 : 0 ) );
}

template< typename K, typename T, class C >
MIUInt mTMap< K, T, C >::GetCapacity( void ) const
{
    return ( m_u32BucketMask + 1 ) * 8 / 10;
}

template< typename K, typename T, class C >
MIUInt mTMap< K, T, C >::GetCount( void ) const
{
    return m_uElementCount;
}

template< typename K, typename T, class C >
MIUInt mTMap< K, T, C >::GetRemainingCapacity( void ) const
{
    return g_max< MIInt >( 0, ( GetCapacity() - GetCount() ) );
}

template< typename K, typename T, class C >
void mTMap< K, T, C >::RemoveAt( K const & a_Key, T * a_pDataDest, MIBool a_bMayShrink, MIBool * a_pHasShrunk )
{
    MIU32 u32Hash = 0;
    SBucket * pBaseBucket = 0;
    SBucket * pBucket = 0;
    if ( !Locate( a_Key, u32Hash, pBaseBucket, pBucket ) )
        return;
    if( a_pDataDest )
        *a_pDataDest = pBucket->m_Data;
    --m_uElementCount;
    pBaseBucket->m_u32HopInfo &= ~( 1 << ( pBucket - pBaseBucket ) );
    pBucket->m_u32Hash = 0;
    pBucket->m_Key = K();
    pBucket->m_Data = T();
    if ( !a_bMayShrink )
        return;
    a_pHasShrunk = ( a_pHasShrunk ? a_pHasShrunk : &a_bMayShrink );
    *a_pHasShrunk = Shrink();
}

template< typename K, typename T, class C >
void mTMap< K, T, C >::Reserve( MIUInt a_uCount )
{
    if ( a_uCount <= GetCapacity() )
        return;
    mTArray< SBucket > arrBucketsOld;
    arrBucketsOld.Swap( m_arrBuckets );
    m_uElementCount = 0;
    a_uCount = ( a_uCount * 10 + 7 ) / 8;  // 80% of new a_uCount is not less than old a_uCount.
    MIU32 uBucketCount = 1;
    while ( uBucketCount < a_uCount )
        uBucketCount <<= 1;
    m_u32BucketMask = uBucketCount - 1;
    uBucketCount += g_min< MIUInt >( EMaxInsertRange, ( 3 + uBucketCount / 8 ) );
    m_arrBuckets.Resize( uBucketCount );
    for ( MIUInt u = 0, ue = arrBucketsOld.GetCount(); u != ue; ++u )
    {
        SBucket const & Bucket = arrBucketsOld[ u ];
        if ( Bucket.m_u32Hash )
            Add( Bucket.m_u32Hash, Bucket.m_Key ) = Bucket.m_Data;
    }
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::Shrink( void )
{
    if ( m_u32BucketMask < g_max< MIUInt >( GetCount() * 3, 8 ) )
        return MIFalse;  // Shrinking not possible or ineffective.
    mTMap< K, T, C > mapNew( GetCount() * 7 / 6 );
    for ( MIUInt u = 0, ue = m_arrBuckets.GetCount(); u != ue; ++u )
    {
        SBucket const & Bucket = m_arrBuckets[ u ];
        if ( Bucket.m_u32Hash )
            mapNew.Add( Bucket.m_u32Hash, Bucket.m_Key ) = Bucket.m_Data;
    }
    Swap( mapNew );
    return MITrue;
}

template< typename K, typename T, class C >
void mTMap< K, T, C >::Swap( mTMap< K, T, C > & a_mapOther )
{
    if ( this == &a_mapOther )
        return;
    m_arrBuckets.Swap( a_mapOther.m_arrBuckets );
    g_swap( m_uElementCount, a_mapOther.m_uElementCount );
    g_swap( m_u32BucketMask, a_mapOther.m_u32BucketMask );
}

template< typename K, typename T, class C >
T & mTMap< K, T, C >::Add( MIU32 a_u32Hash, K const & a_Key )
{
    SBucket * pLastValidBucket = &m_arrBuckets.Back() - 1;
    SBucket * pBaseBucket = 0;
    SBucket * pBucket = 0;
    if ( Locate( a_Key, a_u32Hash, pBaseBucket, pBucket ) )
    {
        return pBucket->m_Data;
    }
    for ( ; ; )
    {
        if ( ( pBaseBucket->m_u32HopInfo ) == ( 0xFFFFFFFF >> ( 32 - EHopRange ) ) )
            break;
        pBucket = pBaseBucket;
        SBucket * pLastBucketInRange = pBaseBucket + EHopRange - 1;
        if ( pLastBucketInRange > pLastValidBucket )
            pLastBucketInRange = pLastValidBucket;

        for ( ; pBucket <= pLastBucketInRange; ++pBucket )
            if ( !pBucket->m_u32Hash )
                break;
        if ( pBucket > pLastBucketInRange )
        {
            if ( GetRemainingCapacity() == 0 )
                break;
            while ( pBucket->m_u32Hash )
                ++pBucket;
            if ( pBucket > pLastValidBucket )
                break;
            if ( pBucket - pBaseBucket > EMaxInsertRange )
                break;
            do
            {
                PullFreeBucket( pBucket );
            }
            while ( pBucket > pLastBucketInRange );
            if ( !pBucket )
                break;
        }
        ++m_uElementCount;
        pBaseBucket->m_u32HopInfo |= ( 1 << ( pBucket - pBaseBucket ) );
        pBucket->m_u32Hash = a_u32Hash;
        pBucket->m_Key = a_Key;
        g_zero( pBucket->m_Data );
        return pBucket->m_Data;
    }
    Reserve( m_arrBuckets.GetCount() );  // Causes a resize.
    return Add( a_u32Hash, a_Key );
}

template< typename K, typename T, class C >
MIU32 mTMap< K, T, C >::Hash( K const & a_Key )
{
    return g_max< MIU32 >( 1, C::Hash( a_Key ) );
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::Locate( K const & a_Key, MIU32 & a_u32Hash, SBucket const *& a_pBaseBucket, SBucket const *& a_pBucket ) const
{
    if ( !a_u32Hash )
        a_u32Hash = Hash( a_Key );
    a_pBaseBucket = &m_arrBuckets[ a_u32Hash & m_u32BucketMask ];
    MIU32 u32HopInfo = a_pBaseBucket->m_u32HopInfo;
    MIUInt uBitIndex = 0;
    MIU32 u32Mask = 1;
    while ( ( u32HopInfo >= u32Mask ) && u32Mask )
    {   
        for ( ; !( u32HopInfo & u32Mask ); u32Mask <<= 1 )
            ++uBitIndex;
        a_pBucket = a_pBaseBucket + uBitIndex;
        if ( a_u32Hash == a_pBucket->m_u32Hash )
            if ( C::Compare( a_Key, a_pBucket->m_Key ) )
                return MITrue;
        ++uBitIndex;
        u32Mask <<= 1;
    }
    a_pBucket = 0;
    return MIFalse;
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::Locate( K const & a_Key, MIU32 & a_u32Hash, SBucket *& a_pBaseBucket, SBucket *& a_pBucket )
{
    return Locate( a_Key, a_u32Hash, const_cast< SBucket const *& >( a_pBaseBucket ), const_cast< SBucket const *& >( a_pBucket ) );
}

template< typename K, typename T, class C >
void mTMap< K, T, C >::PullFreeBucket( SBucket *& a_pFreeBucket )
{
    SBucket * pBaseBucket = a_pFreeBucket - ( EHopRange - 1 );
    MIU32 u32Mask = 0xFFFFFFFF >> ( 32 - ( EHopRange - 1 ) );
    for ( MIUInt uOffsetToFree = ( EHopRange - 1 ); uOffsetToFree != 0; u32Mask >>= 1, ++pBaseBucket, --uOffsetToFree )
    {
        MIU32 u32BaseHopInfo = pBaseBucket->m_u32HopInfo;
        if ( 0 == ( u32Mask & u32BaseHopInfo ) )
            continue;
        MIUInt uOffsetToNewFree = 0;
        for ( MIU32 u32BitIt = 1; !( u32BitIt & u32BaseHopInfo ); u32BitIt <<= 1, ++uOffsetToNewFree )
        {
        }
        SBucket * pNewFreeBucket = pBaseBucket + uOffsetToNewFree;
        a_pFreeBucket->m_u32Hash = pNewFreeBucket->m_u32Hash;
        a_pFreeBucket->m_Key = pNewFreeBucket->m_Key;
        a_pFreeBucket->m_Data = pNewFreeBucket->m_Data;
        pNewFreeBucket->m_u32Hash = 0;
        pBaseBucket->m_u32HopInfo |= ( 1 << uOffsetToFree );
        pBaseBucket->m_u32HopInfo &= ~( 1 << uOffsetToNewFree );
        a_pFreeBucket = pNewFreeBucket;
        return;
    }
    a_pFreeBucket = 0;
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CConstIterator::CConstIterator( CConstIterator const & a_itIterator ) :
    m_pBuckets( a_itIterator.m_pBuckets ),
    m_uIndex( a_itIterator.m_uIndex )
{
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CConstIterator::CConstIterator( mTArray< SBucket > const * a_pBuckets, MIUInt a_uIndex ) :
    m_pBuckets( const_cast< mTArray< SBucket > * >( a_pBuckets ) ),
    m_uIndex( a_uIndex )
{
    if ( m_pBuckets->GetCount() <= m_uIndex )
        return;
    if ( !( *m_pBuckets )[ m_uIndex ].m_u32Hash )
        ++*this;
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CConstIterator::CConstIterator( void ) :
    m_uIndex( 0 )
{
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator & mTMap< K, T, C >::CConstIterator::operator = ( CConstIterator const & a_itIterator )
{
    m_pBuckets = a_itIterator.m_pBuckets;
    m_uIndex = a_itIterator.m_uIndex;
    return *this;
}

template< typename K, typename T, class C >
T const & mTMap< K, T, C >::CConstIterator::operator * ( void ) const
{
    return ( *m_pBuckets )[ m_uIndex ].m_Data;
}

template< typename K, typename T, class C >
T const * mTMap< K, T, C >::CConstIterator::operator -> ( void ) const
{
    return &**this;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator mTMap< K, T, C >::CConstIterator::operator ++ ( MIInt )
{
    CConstIterator itTemp( *this );
    ++*this;
    return itTemp;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator & mTMap< K, T, C >::CConstIterator::operator ++ ( void )
{
    MIUInt uMaxIndex = g_max< MIInt >( 0, m_pBuckets->GetCount() - 1 );
    if ( m_uIndex >= uMaxIndex )
        return *this;
    do ++m_uIndex;
    while ( ( m_uIndex != uMaxIndex ) && ( !( *m_pBuckets )[ m_uIndex ].m_u32Hash ) );
    return *this;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator mTMap< K, T, C >::CConstIterator::operator -- ( MIInt )
{
    CConstIterator itTemp( *this );
    --*this;
    return itTemp;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CConstIterator & mTMap< K, T, C >::CConstIterator::operator -- ( void )
{
    if ( m_uIndex == 0 )
        return *this;
    do --m_uIndex;
    while ( m_uIndex && ( !( *m_pBuckets )[ m_uIndex ].m_u32Hash ) );
    if ( !m_uIndex )
        if ( !( *m_pBuckets )[ m_uIndex ].m_u32Hash )
            ++*this;
    return *this;
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::CConstIterator::operator == ( CConstIterator const & a_itOther ) const
{
    return ( m_pBuckets == a_itOther.m_pBuckets ) && ( m_uIndex == a_itOther.m_uIndex );
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::CConstIterator::operator != ( CConstIterator const & a_itOther ) const
{
    return !( *this == a_itOther );
}

template< typename K, typename T, class C >
K const & mTMap< K, T, C >::CConstIterator::GetKey( void ) const
{
    return ( *m_pBuckets )[ m_uIndex ].m_Key;
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CIterator::CIterator( CIterator const & a_itIterator ) :
    CConstIterator( a_itIterator )
{
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CIterator::CIterator( mTArray< SBucket > * a_pBuckets, MIUInt a_uIndex ) :
    CConstIterator( a_pBuckets, a_uIndex )
{
}

template< typename K, typename T, class C >
mTMap< K, T, C >::CIterator::CIterator( void )
{
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator & mTMap< K, T, C >::CIterator::operator = ( CIterator const & a_itIterator )
{
    CConstIterator::operator = ( a_itIterator );
    return *this;
}

template< typename K, typename T, class C >
T & mTMap< K, T, C >::CIterator::operator * ( void ) const
{
    return ( *m_pBuckets )[ m_uIndex ].m_Data;
}

template< typename K, typename T, class C >
T * mTMap< K, T, C >::CIterator::operator -> ( void ) const
{
    return &**this;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator mTMap< K, T, C >::CIterator::operator ++ ( MIInt )
{
    CIterator itTemp( *this );
    ++*this;
    return itTemp;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator & mTMap< K, T, C >::CIterator::operator ++ ( void )
{
    CConstIterator::operator ++ ();
    return *this;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator mTMap< K, T, C >::CIterator::operator -- ( MIInt )
{
    CIterator itTemp( *this );
    --*this;
    return itTemp;
}

template< typename K, typename T, class C >
typename mTMap< K, T, C >::CIterator & mTMap< K, T, C >::CIterator::operator -- ( void )
{
    CConstIterator::operator -- ();
    return *this;
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::CIterator::operator == ( CIterator const & a_itOther ) const
{
    return CConstIterator::operator == ( a_itOther );
}

template< typename K, typename T, class C >
MIBool mTMap< K, T, C >::CIterator::operator != ( CIterator const & a_itOther ) const
{
    return !( *this == a_itOther );
}

template< typename K, typename T, class C >
mTMap< K, T, C >::SBucket::SBucket( void ) :
    m_u32HopInfo( 0 ),
    m_u32Hash( 0 )
{
}

template< typename K >
MIU32 mTKeyManager< K >::Hash( K const & a_Val )
{
    MIU32 uResult;
    g_MurmurHash3( &a_Val, sizeof( K ), &uResult );
    return uResult;
}

template< typename K >
MIBool mTKeyManager< K >::Compare( K const & a_ValLeft, K const & a_ValRight )
{
    return ( a_ValLeft == a_ValRight );
}

template< typename K >
MIU32 mTPointerKeyManager< K >::Hash( K * const & a_pVal )
{
    return mTKeyManager< K >::Hash( *a_pVal );
}

template< typename K >
MIBool mTPointerKeyManager< K >::Compare( K * const & a_pValLeft, K * const & a_pValRight )
{
    return ( *a_pValLeft == *a_pValRight );
}

template<> inline
MIU32 mTStringKeyManager< MILPCChar >::Hash( MILPCChar const & a_String )
{
    return g_djb2( a_String );
}

template<> inline
MIBool mTStringKeyManager< MILPCChar >::Compare( MILPCChar const & a_StringLeft, MILPCChar const & a_StringRight )
{
    return ( g_strcmp( a_StringLeft, a_StringRight ) == 0 );
}

template<> inline
MIU32 mTStringKeyManager< MILPChar >::Hash( MILPChar const & a_String )
{
    return mTStringKeyManager< MILPCChar >::Hash( a_String );
}

template<> inline
MIBool mTStringKeyManager< MILPChar >::Compare( MILPChar const & a_StringLeft, MILPChar const & a_StringRight )
{
    return mTStringKeyManager< MILPCChar >::Compare( a_StringLeft, a_StringRight );
}

template<> inline
MIU32 mTStringKeyManager< MILPCUnicodeChar >::Hash( MILPCUnicodeChar const & a_String )
{
    MIUInt uLength = 0;
    for ( MILPCUnicodeChar puText = a_String; *puText; ++puText )
        ++uLength;
    MIU32 uResult;
    g_MurmurHash3( a_String, ( sizeof( MIUnicodeChar ) * uLength ), &uResult );
    return uResult;
}

template<> inline
MIBool mTStringKeyManager< MILPCUnicodeChar >::Compare( MILPCUnicodeChar const & a_StringLeft, MILPCUnicodeChar const & a_StringRight )
{
    MILPCUnicodeChar puTextLeft = a_StringLeft;
    MILPCUnicodeChar puTextRight = a_StringRight;
    while ( *puTextLeft == *puTextRight )
    {
        if ( !*puTextLeft )
            return MITrue;
        ++puTextLeft;
        ++puTextRight;
    }
    return MIFalse;
}

template<> inline
MIU32 mTStringKeyManager< MILPUnicodeChar >::Hash( MILPUnicodeChar const & a_String )
{
    return mTStringKeyManager< MILPCUnicodeChar >::Hash( a_String );
}

template<> inline
MIBool mTStringKeyManager< MILPUnicodeChar >::Compare( MILPUnicodeChar const & a_StringLeft, MILPUnicodeChar const & a_StringRight )
{
    return mTStringKeyManager< MILPCUnicodeChar >::Compare( a_StringLeft, a_StringRight );
}

template<> inline
MIU32 mTStringKeyManager< mCString >::Hash( mCString const & a_String )
{
    return mTStringKeyManager< MILPCChar >::Hash( a_String.GetText() ); 
}

template<> inline
MIBool mTStringKeyManager< mCString >::Compare( mCString const & a_StringLeft, mCString const & a_StringRight )
{
    return ( a_StringLeft.Compare( a_StringRight ) == 0 );
}

template< typename K >
MIU32 mTHashKeyManager32< K >::Hash( K const & a_Val )
{
    MI_STATIC_ASSERT( sizeof( K ) == 4 )
    return *reinterpret_cast< MIU32 const * >( &a_Val );
}

template< typename K >
MIBool mTHashKeyManager32< K >::Compare( K const & a_ValLeft, K const & a_ValRight )
{
    return ( a_ValLeft == a_ValRight );
}

template< typename T >
mTStringMap< T >::mTStringMap( MIUInt a_uMinCapacity ) :
    mTMap( a_uMinCapacity )
{
}

template< typename T >
mTStringMap< T >::mTStringMap( mTStringMap< T > const & a_mapSource ) :
    mTMap( a_mapSource )
{
}

template< typename T >
mTNameMap< T >::mTNameMap( MIUInt a_uMinCapacity ) :
    mTMap( a_uMinCapacity )
{
}

template< typename T >
mTNameMap< T >::mTNameMap( mTNameMap< T > const & a_mapSource ) :
    mTMap( a_mapSource )
{
}

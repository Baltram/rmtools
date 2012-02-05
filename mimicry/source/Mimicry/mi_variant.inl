inline
MIBool mCVariant::IsEmpty( void ) const
{
    return !m_idElem.IsValid();
}

template< typename T >
T mCVariant::GetData( void ) const
{
    T Result;
    GetData( Result );
    return Result;
}

template< typename T >
mEResult mCVariant::GetData( T & a_Dest ) const
{
    if ( IsEmpty() )
        return mEResult_False;
    T * pElement = SPool::Access< T >( m_idElem );
    if ( !pElement )
        return mEResult_False;
    a_Dest = *pElement;
    return mEResult_Ok;
}

template< typename T >
MIBool mCVariant::IsOfType( void ) const
{
    if ( IsEmpty() )
        return MIFalse;
    return ( SPool::Access< T >( m_idElem ) != 0 );
}

template< typename T >
void mCVariant::SetData( T const & a_Source )
{
    if ( !IsEmpty() )
    {
        T * pElement = SPool::Access< T >( m_idElem );
        if ( pElement )
        {
            *pElement = a_Source;
            return;
        }
        Clear();
    }
    m_idElem = SPool::New< T >( &a_Source );
}

template< typename T >
mEResult mCVariant::SwapData( T & a_Element )
{
    if ( IsEmpty() )
        return mEResult_False;
    T * pElement = SPool::Access< T >( m_idElem );
    if ( !pElement )
        return mEResult_False;
    g_memswap( a_Element, *pElement );
    return mEResult_Ok;
}


inline
MIBool mCVariant::SId::operator == ( SId const & a_idOther ) const
{
    return ( m_uPoolIndex == a_idOther.m_uPoolIndex ) && ( m_uIndex == a_idOther.m_uIndex );
}

inline
void mCVariant::SId::Invalidate( void )
{
    m_uPoolIndex = ( 1 << EPoolIndexExp ) - 1;
}

inline
MIBool mCVariant::SId::IsValid( void ) const
{
    return ( m_uPoolIndex != ( 1 << EPoolIndexExp ) - 1 );
}

template< typename T >
T * mCVariant::SPool::Access( SId const & a_idID )
{
    SPool * pPool = GetPool( a_idID );
    if ( SFunctionAccessor::GetPointer< T >() != pPool->m_FunctionAccessor )
        return 0;
    return static_cast< T * >( pPool->m_arrElements ) + a_idID.m_uIndex;
}

inline
mCVariant::SId mCVariant::SPool::Clone( SId const & a_idID )
{
    SId idResult;
    ( *( GetPool( a_idID )->m_FunctionAccessor->m_funcClone ) )( idResult, a_idID ); 
    return idResult;
}

inline
MIBool mCVariant::SPool::Compare( SId const & a_idLeft, SId const & a_idRight )
{
    return ( *( GetPool( a_idLeft )->m_FunctionAccessor->m_funcCompare ) )( a_idLeft, a_idRight );
}

inline
void mCVariant::SPool::Free( SId & a_idID )
{
    SPool * const pPool = GetPool( a_idID );
    ( *( pPool->m_FunctionAccessor->m_funcFree ) )( pPool, a_idID );
}

template< typename T >
mCVariant::SId mCVariant::SPool::New( T const * a_pData )
{
    MIUInt uVacantPoolIndex = MI_DW_INVALID;
    HandleVacantPoolIndex< T >( uVacantPoolIndex );
    SPool & VacantPool = *s_arrPools[ uVacantPoolIndex ];
    SId idResult;
    idResult.m_uPoolIndex = uVacantPoolIndex;
    idResult.m_uIndex = VacantPool.m_arrVacantIndices[ EIndexCardinality ];
    VacantPool.m_arrVacantIndices[ EIndexCardinality ] = VacantPool.m_arrVacantIndices[ idResult.m_uIndex ];
    VacantPool.m_arrVacantIndices[ idResult.m_uIndex ] = idResult.m_uIndex;
    if ( a_pData )
        static_cast< T * >( VacantPool.m_arrElements )[ idResult.m_uIndex ] = *a_pData;
    ++VacantPool.m_uElementCount;
    return idResult;
}

template< typename T >
mCVariant::SPool::SFunctionAccessor mCVariant::SPool::SFunctionAccessor::GetObject( void )
{
    SFunctionAccessor Result;
    Result.m_funcClone = &SPool::CloneTemplated< T >;
    Result.m_funcCompare = &SPool::CompareTemplated< T >;
    Result.m_funcFree = &SPool::FreeTemplated< T >;
    return Result;
}

template< typename T >
mCVariant::SPool::SFunctionAccessor const * mCVariant::SPool::SFunctionAccessor::GetPointer( void )
{
    static SFunctionAccessor const s_FunctionAccessor = SFunctionAccessor::GetObject< T >();
    return &s_FunctionAccessor;
}

template< typename T >
void mCVariant::SPool::CloneTemplated( SId & a_idDest, SId const & a_idSource )
{
    a_idDest = New< T >( Access< T >( a_idSource ) );
}

template< typename T >
void mCVariant::SPool::Collapse( MIUInt a_uPoolIndex )
{
    if ( s_arrVacantMapIndices.GetCount() == 0 )
    {
        for ( MIUInt u = s_arrMaps.GetCount(); u--; )
            if ( static_cast< MIUInt >( EMinMapCapacity - EMaxMigrateCount ) >= s_arrMaps[ u ]->GetCount() )
            {
                s_arrMaps[ u ]->Reserve( EMinMapCapacity );
                s_arrVacantMapIndices.Add( u );
                break;
            }
        if ( s_arrVacantMapIndices.GetCount() == 0 )
        {
            s_arrVacantMapIndices.Add( s_arrMaps.GetCount() );
            s_arrMaps.Add( new Map( EMinMapCapacity ) );
        }
    }
    MIUInt const uMigratingMapIndex = s_arrVacantMapIndices.Back();
    Map & MigratingMap = *s_arrMaps[ uMigratingMapIndex ];

    SPool * pPool = s_arrPools[ a_uPoolIndex ];
    pPool->m_uElementCount = EIndexCardinality;  // Prevent insertion of new elements while collapsing.

    SId idOld;
    idOld.m_uPoolIndex = a_uPoolIndex;
    for ( MIUInt u = 0, ue = EIndexCardinality; u != ue; ++u )
    {
        if ( pPool->m_arrVacantIndices[ u ] == u )
        {
            idOld.m_uIndex = u;
            MigratingMap[ idOld ] = New( static_cast< T * >( pPool->m_arrElements ) + u );
        }
    }
    if ( MigratingMap.GetRemainingCapacity() < EMaxMigrateCount )
        s_arrVacantMapIndices.Remove( uMigratingMapIndex );

    delete [] static_cast< T * >( pPool->m_arrElements );
    delete [] pPool->m_arrVacantIndices;
    pPool->m_uMigratingMapIndex = uMigratingMapIndex;
    pPool->m_uElementCount = MI_DW_INVALID;
    --s_uCollapsablePoolCount;
    s_arrCollapsedPoolIndices.AddNew() = a_uPoolIndex;
}

template< typename T >
MIBool mCVariant::SPool::CompareTemplated( SId const & a_idLeft, SId const & a_idRight )
{
    T * ElemLeft = Access< T >( a_idLeft );
    T * ElemRight = Access< T >( a_idRight );
    if ( ElemLeft && ElemRight )
        return ( *ElemLeft == *ElemRight );
    return MIFalse;
}

template< typename T >
void mCVariant::SPool::FreeTemplated( SPool * a_pPool, SId & a_idID )
{
    static_cast< T * >( a_pPool->m_arrElements )[ a_idID.m_uIndex ] = T();
    if ( EIndexCardinality == ( a_pPool->m_arrVacantIndices[ a_idID.m_uIndex ] = a_pPool->m_arrVacantIndices[ EIndexCardinality ] ) )  // If a_pPool gets "vacant" by now.
    {
        MIUInt uPoolIndex = a_idID.m_uPoolIndex;
        HandleVacantPoolIndex< T >( uPoolIndex );
    }
    a_pPool->m_arrVacantIndices[ EIndexCardinality ] = a_idID.m_uIndex;
    a_idID.Invalidate();
    if ( --( a_pPool->m_uElementCount ) != EMaxMigrateCount )  // If a_pPool doesn't change from "non-collapsable" to "collapsable".
        return;
    if ( ++s_uCollapsablePoolCount < 4 )
        return;
    for ( MIUInt u = 0, ue = s_arrPools.GetCount(); u != ue; ++u )
        if ( s_arrPools[ u ]->m_uElementCount <= EMaxMigrateCount )
            Collapse< T >( u );
}

inline
mCVariant::SPool * mCVariant::SPool::GetPool( SId const & a_idID )
{
    for ( ; ; )
    {
        SPool * pPool = s_arrPools[ a_idID.m_uPoolIndex ];
        if ( pPool->m_uMigratingMapIndex == MI_DW_INVALID )
            return pPool;
        Map * pMap = s_arrMaps[ pPool->m_uMigratingMapIndex ];
        MIBool bHasMapShrunk = MIFalse;
        pMap->RemoveAt( a_idID, const_cast< SId * >( &a_idID ), ( s_arrVacantMapIndices.GetCount() >= 4 ), &bHasMapShrunk );
        // Note: a_idID still refers to the same element.
        if ( bHasMapShrunk )
        {
            if ( pMap->GetRemainingCapacity() < EMaxMigrateCount )
                s_arrVacantMapIndices.Remove( pPool->m_uMigratingMapIndex );
        }
        else if ( pMap->GetRemainingCapacity() == EMaxMigrateCount )  // If pMap is "vacant" by now.
        {
            s_arrVacantMapIndices.Add( pPool->m_uMigratingMapIndex );
        }
    }
}

template< typename T >
void mCVariant::SPool::HandleVacantPoolIndex( MIUInt & a_uVacantPoolIndex )
{
    static MIUInt s_uLastVacantPoolIndex = MI_DW_INVALID;
    if ( a_uVacantPoolIndex != MI_DW_INVALID )
    {
        SPool * pPool = s_arrPools[ a_uVacantPoolIndex ];
        if ( ( pPool->m_uNextVacantPoolIndex != MI_DW_INVALID ) || ( a_uVacantPoolIndex == s_uLastVacantPoolIndex ) )  // If pPool is already registered as "vacant".
            return;
        pPool->m_uNextVacantPoolIndex = s_uLastVacantPoolIndex;
        s_uLastVacantPoolIndex = a_uVacantPoolIndex;
        return;
    }
    for ( ; ; )
    {
        if ( s_uLastVacantPoolIndex != MI_DW_INVALID )
        {
            if ( s_arrPools[ s_uLastVacantPoolIndex ]->m_uElementCount < EIndexCardinality )
                break;
            SPool * pFilledPool = s_arrPools[ s_uLastVacantPoolIndex ];
            s_uLastVacantPoolIndex = pFilledPool->m_uNextVacantPoolIndex;
            pFilledPool->m_uNextVacantPoolIndex = MI_DW_INVALID;
            continue;
        }
        MIUInt uCollapsedPoolIndex = MI_DW_INVALID;
        for ( MIUInt u = 0, ue = s_arrCollapsedPoolIndices.GetCount(); u != ue; ++u )
            if ( SFunctionAccessor::GetPointer< T >() == s_arrPools[ s_arrCollapsedPoolIndices[ u ] ]->m_FunctionAccessor )
            {
                uCollapsedPoolIndex = s_arrCollapsedPoolIndices[ u ];
                break;
            }
        if ( uCollapsedPoolIndex != MI_DW_INVALID )
        {
            s_uLastVacantPoolIndex = uCollapsedPoolIndex;
            Uncollapse< T >( uCollapsedPoolIndex );
            break;
        }
        s_uLastVacantPoolIndex = s_arrPools.GetCount();
        Init< T >( s_arrPools.AddNew() = new SPool );
    }
    a_uVacantPoolIndex = s_uLastVacantPoolIndex;
}

template< typename T >
void mCVariant::SPool::Init( SPool * a_pPool )
{
    a_pPool->m_arrVacantIndices = new MIU8 [ EIndexCardinality + 1 ];
    a_pPool->m_arrElements = new T [ EIndexCardinality ];
    a_pPool->m_uNextVacantPoolIndex = MI_DW_INVALID;
    a_pPool->m_FunctionAccessor = SFunctionAccessor::GetPointer< T >();
    a_pPool->m_uMigratingMapIndex = MI_DW_INVALID;
    a_pPool->m_uElementCount = 0;
    for ( MIU8 u = 0, ue = EIndexCardinality; u != ue; ++u )
        a_pPool->m_arrVacantIndices[ u ] = u + 1;
    a_pPool->m_arrVacantIndices[ EIndexCardinality ] = 0;
}

template< typename T >
void mCVariant::SPool::Uncollapse( MIUInt a_uPoolIndex )
{
    SPool * pPool = s_arrPools[ a_uPoolIndex ];
    MIUInt const uMigratingMapIndex = pPool->m_uMigratingMapIndex;
    MIUInt const uNextVacantPoolIndex = pPool->m_uNextVacantPoolIndex;
    Init< T >( pPool );
    pPool->m_uMigratingMapIndex = uMigratingMapIndex;
    pPool->m_uNextVacantPoolIndex = uNextVacantPoolIndex;
    Map * pMap = s_arrMaps[ uMigratingMapIndex ];

    mTArray< SId > arrIDs( EMaxMigrateCount );
    for ( Map::CIterator i = pMap->Begin(), ie = pMap->End(); i != ie; ++i )
        if ( i.GetKey().m_uPoolIndex == a_uPoolIndex )
            arrIDs.Add( i.GetKey() );

    pPool->m_uElementCount = arrIDs.GetCount();
    MIU8 * arrVacantIndices = pPool->m_arrVacantIndices;
    for ( mTArray< SId >::CIterator i = arrIDs.Begin(), ie = arrIDs.End(); i != ie; ++i )
    {
        arrVacantIndices[ i->m_uIndex ] = i->m_uIndex;
        T & Dest = static_cast< T * >( pPool->m_arrElements )[ i->m_uIndex ];
        Dest = *Access< T >( *i );
    }

    MIU8 u = 0;
    while ( arrVacantIndices[ u ] == u )
        ++u;
    arrVacantIndices[ EIndexCardinality ] = u;
    for ( MIU8 ue = EIndexCardinality, v = u + 1; u != ue; ++u )
    {
        if ( arrVacantIndices[ u ] == u )
            continue;
        while ( arrVacantIndices[ v ] == v )
            ++v;
        arrVacantIndices[ u ] = v++;
    }

    pPool->m_uMigratingMapIndex = MI_DW_INVALID;
    s_arrCollapsedPoolIndices.Remove( a_uPoolIndex );
    for ( mTArray< SId >::CIterator i = arrIDs.Begin(), ie = arrIDs.End(); i != ie; ++i )
        Free( *i );
}
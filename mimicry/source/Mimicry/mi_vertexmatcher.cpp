#include "mi_include_3d.h"
#include "mi_include_standard.h"

mCVertexMatcher::mCVertexMatcher( mCVec3 const * a_pVertsToMatch, mCVec3 const * a_pTargetVerts, MIUInt a_uNumVertsToMatch, MIUInt a_uNumTargetVerts, MIBool a_bIndirectMatch ) :
    m_uUnmatchedPointCount( 0 )
{
    if ( !a_pVertsToMatch || !a_pTargetVerts || !a_uNumVertsToMatch || !a_uNumTargetVerts )
        return;
    g_memset( m_arrPointsE, 0, sizeof( m_arrPointsE ) );
    mCVec3 vecMin( *a_pVertsToMatch ), vecMax( *a_pVertsToMatch );
    for ( MIUInt u = a_uNumVertsToMatch; u--; )
        for ( MIUInt v = 3; v--; )
            vecMin[ v ] = g_min( vecMin[ v ], a_pVertsToMatch[ u ][ v ] ), vecMax[ v ] = g_max( vecMax[ v ], a_pVertsToMatch[ u ][ v ] );
    for ( MIUInt u = a_uNumTargetVerts; u--; )
        for ( MIUInt v = 3; v--; )
            vecMin[ v ] = g_min( vecMin[ v ], a_pTargetVerts[ u ][ v ] ), vecMax[ v ] = g_max( vecMax[ v ], a_pTargetVerts[ u ][ v ] );
    mCVec3 vecExtents = vecMax - vecMin;
    MIFloat const fAverageVertCountPerChunk = 25.0f;
    MIFloat const uRawChunkCount = ( a_uNumVertsToMatch + a_uNumTargetVerts ) / fAverageVertCountPerChunk;
    MIFloat const fMinExtent = vecExtents.CalcMagnitude() / sqrt( uRawChunkCount );
    for ( MIUInt u = 3; u--; vecExtents[ u ] = g_max( vecExtents[ u ], fMinExtent ) );
    MIFloat const fRawVolumePerChunk = vecExtents[ 0 ] * vecExtents[ 1 ] * vecExtents[ 2 ] / uRawChunkCount;
    MIFloat const fRawEdgeLength = pow( fRawVolumePerChunk, 1.0f / 3.0f );
    for ( MIUInt u = 3; u--; m_arrExtents[ u ] = static_cast< MIUInt >( ceil( vecExtents[ u ] / fRawEdgeLength ) ) );
    m_fEdgeLength = 0;
    for ( MIUInt u = 3; u--; m_fEdgeLength = g_max( m_fEdgeLength, vecExtents[ u ] / m_arrExtents[ u ] ) );
    m_fEdgeLength = ( m_fEdgeLength + FLT_EPSILON ) * ( 1.0f + FLT_EPSILON );
    m_vecOrigin = vecMin;
    m_arrChunks.Resize( m_arrExtents[ 0 ] * m_arrExtents[ 1 ] * m_arrExtents[ 2 ] );
    m_arrPointsToMatch.Resize( a_uNumVertsToMatch );
    m_arrTargetPoints.Resize( a_uNumTargetVerts );
    for ( MIUInt u = a_uNumVertsToMatch; u--; m_arrPointsToMatch[ u ].Init( this, a_pVertsToMatch + u ) );
    for ( MIUInt u = a_uNumTargetVerts; u--; m_arrTargetPoints[ u ].Init( this, a_pTargetVerts + u ) );
    if ( a_bIndirectMatch )
        for ( MIUInt u = a_uNumVertsToMatch; u--; m_arrPointsToMatch[ u ].RegisterC( this ) );
    for ( MIUInt u = a_uNumTargetVerts; u--; m_arrTargetPoints[ u ].RegisterC( this ) );
    for ( MIUInt u = a_uNumVertsToMatch; u--; m_arrPointsToMatch[ u ].RegisterE( this, u ) );
    MIBool bForceSuccess = MIFalse;
    while ( m_uUnmatchedPointCount )
    {
        for ( MIUInt u = 0; u != 64; ++u )
            for ( CPoint * pPoint = m_arrPointsE[ u ]; pPoint; pPoint = pPoint->GetNextE() )
                FindNearestPoint( pPoint, bForceSuccess );
        MIBool bSuccess = MIFalse;
        for ( MIUInt u = a_uNumVertsToMatch; u--; )
            if ( m_arrPointsToMatch[ u ].GetGroupID() == u )
                bSuccess |= m_arrPointsToMatch[ u ].ExpandGroup( this );
        if ( !bSuccess )
            bForceSuccess = MITrue;
    }
    m_arrResult.Resize( a_uNumVertsToMatch );
    CPoint const * const pBeginPointsToMatch = m_arrPointsToMatch.GetBuffer();
    for ( MIUInt u = a_uNumTargetVerts; u--; )
        for ( CPoint const * pTargetPoint = &m_arrTargetPoints[ u ], * pPoint = pTargetPoint->GetNextG(); pPoint != pTargetPoint; pPoint = pPoint->GetNextG() )
            m_arrResult[ pPoint - pBeginPointsToMatch ] = u;
}

MIUInt mCVertexMatcher::operator [] ( MIUInt a_uIndex ) const
{
    return m_arrResult[ a_uIndex ];
}

mTArray< MIUInt > & mCVertexMatcher::AccessResult( void )
{
    return m_arrResult;
}

mTArray< MIUInt > const & mCVertexMatcher::GetResult( void ) const
{
    return m_arrResult;
}

void mCVertexMatcher::FindNearestPoint( CPoint * a_pPoint, MIBool a_bForceSuccess ) const
{
    static CPointFinder PointFinder1( 0 );
    static CPointFinder PointFinder2( 3 );
    static CPointFinder PointFinder3( 8 );
    static CPointFinder PointFinder4( 14 );
    static CPointFinder * arrPointFinders[ 4 ] = { &PointFinder1, &PointFinder2, &PointFinder3, &PointFinder4 };
    if ( a_pPoint->GetMinSearchLevel() == 0 )
    {
        CChunk const & Chunk = m_arrChunks[ a_pPoint->GetChunkIndex() ];
        for ( CPoint * pPoint = Chunk.GetFirstPoint(); pPoint; pPoint = pPoint->GetNextC() )
        {
            if ( ( pPoint->GetExp() == a_pPoint->GetExp() ) &&
                 ( pPoint->GetGroupID() != a_pPoint->GetGroupID() ) &&
                 ( ( pPoint->GetPosition() - a_pPoint->GetPosition() ).CalcMagnitudeSqr() < 1e-6f ) )
            {
                a_pPoint->SetNearestPoint( pPoint, 0.0f );
                return;
            }
        }
    }
    a_pPoint->AssureMinSearchLevel( 1 );
    for ( MIUInt u = a_pPoint->GetMinSearchLevel() - 1; u != 4; ++u )
    {
        if ( arrPointFinders[ u ]->FindNearestPoint( this, a_pPoint ) )
            break;
        else
            a_pPoint->AssureMinSearchLevel( u + 2 );
    }
    if ( !a_bForceSuccess || ( a_pPoint->GetNearestPoint() && ( a_pPoint->GetNearestPoint()->GetGroupID() != a_pPoint->GetGroupID() ) ) )
        return;
    CPoint * pNearest = 0;
    MIFloat fDistanceMin = g_fPosInfinity;
    for ( MIUInt u = m_arrChunks.GetCount(); u--; )
    {
        for ( CPoint * pPoint = m_arrChunks[ u ].GetFirstPoint(); pPoint; pPoint = pPoint->GetNextC() )
        {
            MIFloat fDistance = ( pPoint->GetPosition() - a_pPoint->GetPosition() ).CalcMagnitudeSqr();
            if ( ( fDistance < fDistanceMin ) && ( pPoint->GetGroupID() != a_pPoint->GetGroupID() ) )
            {
                fDistanceMin = fDistance;
                pNearest = pPoint;
            }
        }
    }
    a_pPoint->SetNearestPoint( pNearest, fDistanceMin );
}

mCVertexMatcher::CPoint::CPoint( void ) :
    m_pPosition( 0 ),
    m_pNearest( 0 ),
    m_pNextC( 0 ),
    m_pNextE( 0 ),
    m_uGroupID( MI_DW_INVALID ),
    m_uMinSearchLevel( 0 )
{
    m_pNextG = this;
}

MIBool mCVertexMatcher::CPoint::And( MIU64 a_u64Mask ) const
{
    return ( ( ( MIU64 ) 1 << m_uExp ) & a_u64Mask ) != 0;
}

void mCVertexMatcher::CPoint::AssureMinSearchLevel( MIUInt a_uMinSearchLevel )
{
    m_uMinSearchLevel = g_max( m_uMinSearchLevel, a_uMinSearchLevel );
}

MIBool mCVertexMatcher::CPoint::ExpandGroup( mCVertexMatcher * a_pMatcher )
{
    MIFloat fMinDistance = m_pNearest ? m_fDistance : g_fPosInfinity;
    CPoint * pNearest = m_pNearest;
    for ( CPoint * pPoint = m_pNextG; pPoint != this; pPoint = pPoint->m_pNextG )
        if ( ( pPoint->m_fDistance < fMinDistance ) && pPoint->m_pNearest && ( pNearest = pPoint->m_pNearest ) )
            fMinDistance = pPoint->m_fDistance;
    if ( !pNearest )
        return MIFalse;
    if ( pNearest->m_uGroupID == MI_DW_INVALID )
        for ( CPoint * pPoint = this; pPoint->m_uGroupID != MI_DW_INVALID; pPoint = pPoint->m_pNextG )
            pPoint->UnRegisterE( a_pMatcher );
    else
        for ( CPoint * pPoint = pNearest; pPoint->m_uGroupID != m_uGroupID; pPoint = pPoint->m_pNextG )
            pPoint->m_uGroupID = m_uGroupID;
    g_swap( m_pNextG, pNearest->m_pNextG );
    return MITrue;
}

MIUInt mCVertexMatcher::CPoint::GetChunkIndex( void ) const
{
    return m_uChunkIndex;
}

MIUInt mCVertexMatcher::CPoint::GetExp( void ) const
{
    return m_uExp;
}

MIUInt mCVertexMatcher::CPoint::GetGroupID( void ) const
{
    return m_uGroupID;
}

MIUInt mCVertexMatcher::CPoint::GetMinSearchLevel( void ) const
{
    return m_uMinSearchLevel;
}

mCVertexMatcher::CPoint const * mCVertexMatcher::CPoint::GetNearestPoint( void ) const
{
    return m_pNearest;
}

mCVertexMatcher::CPoint * mCVertexMatcher::CPoint::GetNextC( void ) const
{
    return m_pNextC;
}

mCVertexMatcher::CPoint * mCVertexMatcher::CPoint::GetNextE( void )
{
    if ( m_pNextE && ( m_pNextE->m_uGroupID == MI_DW_INVALID ) )
        m_pNextE = m_pNextE->GetNextE();
    return m_pNextE;
}

mCVertexMatcher::CPoint const * mCVertexMatcher::CPoint::GetNextG( void ) const
{
    return m_pNextG;
}

mCVec3 const & mCVertexMatcher::CPoint::GetPosition( void ) const
{
    return *m_pPosition;
}

void mCVertexMatcher::CPoint::Init( mCVertexMatcher const * a_pMatcher, mCVec3 const * a_pPosition )
{
    m_pPosition = a_pPosition;
    mCVec3 vecOffset = ( *m_pPosition - a_pMatcher->m_vecOrigin ) / a_pMatcher->m_fEdgeLength;
    mCVec3 vecOffsetInChunk = vecOffset;
    for ( MIUInt u = 3; u--; vecOffsetInChunk[ u ] -= static_cast< MIUInt >( vecOffset[ u ] ) );
    vecOffsetInChunk *= 4.0f;
    m_uExp = 4 * 4 * static_cast< MIUInt >( vecOffsetInChunk[ 0 ] ) +
                 4 * static_cast< MIUInt >( vecOffsetInChunk[ 1 ] ) +
                     static_cast< MIUInt >( vecOffsetInChunk[ 2 ] );
    MIUInt const * const pExtents = a_pMatcher->m_arrExtents;
    m_uChunkIndex = pExtents[ 2 ] * pExtents[ 1 ] * static_cast< MIUInt >( vecOffset[ 0 ] ) +
                                    pExtents[ 2 ] * static_cast< MIUInt >( vecOffset[ 1 ] ) +
                                                    static_cast< MIUInt >( vecOffset[ 2 ] );
}

void mCVertexMatcher::CPoint::RegisterC( mCVertexMatcher * a_pMatcher )
{
    CChunk & Chunk = a_pMatcher->m_arrChunks[ m_uChunkIndex ];
    m_pNextC = Chunk.m_pFirstPoint;
    Chunk.m_pFirstPoint = this;
    Chunk.m_u64Spread |= ( MIU64 ) 1 << m_uExp;
}

void mCVertexMatcher::CPoint::RegisterE( mCVertexMatcher * a_pMatcher, MIUInt a_uGroupID )
{
    m_uGroupID = a_uGroupID;
    m_pNextE = a_pMatcher->m_arrPointsE[ m_uExp ];
    a_pMatcher->m_arrPointsE[ m_uExp ] = this;
    ++a_pMatcher->m_uUnmatchedPointCount;
}

void mCVertexMatcher::CPoint::SetNearestPoint( CPoint * a_pPoint, MIFloat a_fDistance )
{
    m_pNearest = a_pPoint;
    m_fDistance = a_fDistance;
}

void mCVertexMatcher::CPoint::UnRegisterE( mCVertexMatcher * a_pMatcher )
{
    m_uGroupID = MI_DW_INVALID;
    if ( a_pMatcher->m_arrPointsE[ m_uExp ] == this )
        a_pMatcher->m_arrPointsE[ m_uExp ] = GetNextE();
    --a_pMatcher->m_uUnmatchedPointCount;
}

mCVertexMatcher::CChunk::CChunk( void ) :
    m_u64Spread( 0 ),
    m_pFirstPoint( 0 )
{
}

MIBool mCVertexMatcher::CChunk::And( MIU64 a_u64Mask ) const
{
    return ( m_u64Spread & a_u64Mask ) != 0;
}

mCVertexMatcher::CPoint * mCVertexMatcher::CChunk::GetFirstPoint( void ) const
{
    return m_pFirstPoint;
}

mCVertexMatcher::CPointFinder::CPointFinder( MIUInt a_uMaxDistanceSqr ) :
    m_pMatcher( 0 ),
    m_uExp( 0 ),
    m_fRadius( g_fPosInfinity )
{
    g_memset( m_arrMasks, 0, sizeof( m_arrMasks ) );
    for ( MIUInt u = 0; u != 3 * 3 * 3; ++u )
    {
        for ( MIUInt v = 0; v != 4 * 4 * 4; ++v )
        {
            MIInt arrCoords[ 3 ];
            arrCoords[ 0 ] = u / ( 3 * 3 )     * 4 + v / ( 4 * 4 );
            arrCoords[ 1 ] = u / ( 3 )     % 3 * 4 + v / ( 4 ) % 4;
            arrCoords[ 2 ] = u             % 3 * 4 + v         % 4;
            MIUInt uDistanceSqr = 0;
            for ( MIUInt w = 3; w--; uDistanceSqr += arrCoords[ w ] * arrCoords[ w ] )
                arrCoords[ w ] -= arrCoords[ w ] == 4 ? 4 : ( arrCoords[ w ] < 4 ? 3 : 5 );
            if ( uDistanceSqr > a_uMaxDistanceSqr )
                m_fRadius = g_min( m_fRadius, sqrt( static_cast< MIFloat >( uDistanceSqr ) ) );
            else
                m_arrMasks[ u ] |= ( MIU64 ) 1 << v;
        }
    }
}

MIBool mCVertexMatcher::CPointFinder::FindNearestPoint( mCVertexMatcher const * a_pMatcher, CPoint * a_pPoint )
{
    if ( a_pPoint->GetNearestPoint() && ( a_pPoint->GetNearestPoint()->GetGroupID() != a_pPoint->GetGroupID() ) )
        return MITrue;
    Init( a_pMatcher, a_pPoint->GetExp() );
    CChunk const * const pChunks = a_pMatcher->m_arrChunks.GetBuffer();
    CChunk const * const pEndChunks = pChunks + a_pMatcher->m_arrChunks.GetCount();
    CChunk const * const pChunkCenter = pChunks + a_pPoint->GetChunkIndex();
    CPoint * pResult = 0;
    MIFloat fMinDistance = m_fRadius * m_fRadius;
    for ( MIUInt u = 0; u != m_uChunkCount; ++u )
    {
        CChunk const * const pChunk = pChunkCenter + m_arrChunkOffsets[ u ];
        MIU64 const & Mask = m_arrMasks[ m_arrMaskOffsets[ u ] ];
        if ( ( pChunk < pChunks ) || ( pChunk >= pEndChunks ) || !pChunk->And( Mask ) )
            continue;
        for ( CPoint * pPoint = pChunk->GetFirstPoint(); pPoint; pPoint = pPoint->GetNextC() )
        {
            if ( ( pPoint->GetGroupID() == a_pPoint->GetGroupID() ) || !pPoint->And( Mask ) )
                continue;
            MIFloat fDistance = ( pPoint->GetPosition() - a_pPoint->GetPosition() ).CalcMagnitudeSqr();
            if ( fDistance < fMinDistance )
            {
                fMinDistance = fDistance;
                pResult = pPoint;
            }
        }
    }
    a_pPoint->SetNearestPoint( pResult, fMinDistance );
    return pResult != 0;
}

MIU64 mCVertexMatcher::CPointFinder::GetShiftMask( MIUInt a_uAxis, MIUInt a_uDistance )
{
    static MIU64 s_arrMasks[ 3 ][ 3 ];
    static MIBool s_bInit = MIFalse;
    if ( !s_bInit )
    {
        s_arrMasks[ 0 ][ 0 ] = 0xFFFF;
        s_arrMasks[ 0 ][ 1 ] = s_arrMasks[ 0 ][ 0 ] | s_arrMasks[ 0 ][ 0 ] << 16;
        s_arrMasks[ 0 ][ 2 ] = s_arrMasks[ 0 ][ 0 ] | s_arrMasks[ 0 ][ 1 ] << 16;
        s_arrMasks[ 1 ][ 0 ] = ( ( ( MIU64 ) 1 << 4 ) - 1 ) | ( ( ( MIU64 ) 1 << 4 ) - 1 ) << 16 | ( ( ( MIU64 ) 1 << 4 ) - 1 ) << 32 | ( ( ( MIU64 ) 1 << 4 ) - 1 ) << 48;
        s_arrMasks[ 1 ][ 1 ] = s_arrMasks[ 1 ][ 0 ] | s_arrMasks[ 1 ][ 0 ] << 4;
        s_arrMasks[ 1 ][ 2 ] = s_arrMasks[ 1 ][ 0 ] | s_arrMasks[ 1 ][ 1 ] << 4;
        s_arrMasks[ 2 ][ 0 ] = 1 | 1 << 4 | 1 << 8 | 1 << 12;
        s_arrMasks[ 2 ][ 0 ] = s_arrMasks[ 2 ][ 0 ] | s_arrMasks[ 2 ][ 0 ] << 16 | s_arrMasks[ 2 ][ 0 ] << 32 | s_arrMasks[ 2 ][ 0 ] << 48;
        s_arrMasks[ 2 ][ 1 ] = s_arrMasks[ 2 ][ 0 ] | s_arrMasks[ 2 ][ 0 ] << 1;
        s_arrMasks[ 2 ][ 2 ] = s_arrMasks[ 2 ][ 0 ] | s_arrMasks[ 2 ][ 1 ] << 1;
        s_bInit = MITrue;
    }
    return s_arrMasks[ a_uAxis ][ 3 - a_uDistance ];
}

void mCVertexMatcher::CPointFinder::Init( mCVertexMatcher const * a_pMatcher, MIUInt a_uExp )
{
    if ( ( m_pMatcher == a_pMatcher ) && ( m_uExp == a_uExp ) )
        return;
    Shift( 0, static_cast< MIInt >( a_uExp / ( 4 * 4 ) - m_uExp / ( 4 * 4 ) ) );
    Shift( 1, static_cast< MIInt >( a_uExp / ( 4 ) % 4 - m_uExp / ( 4 ) % 4 ) );
    Shift( 2, static_cast< MIInt >( a_uExp / ( 1 ) % 4 - m_uExp / ( 1 ) % 4 ) );
    m_uChunkCount = 0;
    MIUInt const * pExtents = a_pMatcher->m_arrExtents;
    MIInt iChunkOffset = 0 - pExtents[ 2 ] * pExtents[ 1 ] - pExtents[ 2 ] - 1, iMaskOffset = 0;
    for ( MIUInt u = 3; u--; iChunkOffset += pExtents[ 2 ] * pExtents[ 1 ] - 3 * pExtents[ 2 ] )
        for ( MIUInt v = 3; v--; iChunkOffset += pExtents[ 2 ] - 3 )
            for ( MIUInt w = 3; w--; ++iChunkOffset, ++iMaskOffset )
                if ( m_arrMasks[ iMaskOffset ] )
                    m_arrChunkOffsets[ m_uChunkCount ] = iChunkOffset, m_arrMaskOffsets[ m_uChunkCount++ ] = iMaskOffset;
    m_pMatcher = a_pMatcher;
    m_uExp = a_uExp;
}

void mCVertexMatcher::CPointFinder::Shift( MIUInt a_uAxis, MIInt a_iDistance )
{
    if ( a_iDistance == 0 )
        return;
    if ( a_iDistance < 0 )
    {
        ShiftBack( a_uAxis, 0 - a_iDistance );
        return;
    }
    MIUInt const uOffsetC = ( a_uAxis < 2 ? 3 : 1 ) * ( a_uAxis < 1 ? 3 : 1 );
    MIUInt const uOffsetE = ( a_uAxis < 2 ? 4 : 1 ) * ( a_uAxis < 1 ? 4 : 1 );
    MIInt iIndex = 0;
    MIU64 u64Remainder = 0;
    for ( MIUInt u = 27; u--; iIndex += uOffsetC )
    {
        if ( iIndex >= 27 )
            iIndex -= 26;
        MIU64 u64NewMask = ( ( m_arrMasks[ iIndex ] & GetShiftMask( a_uAxis, a_iDistance ) ) << uOffsetE * a_iDistance ) | u64Remainder;
        u64Remainder = ( m_arrMasks[ iIndex ] & ~GetShiftMask( a_uAxis, a_iDistance ) ) >> uOffsetE * ( 4 - a_iDistance );
        m_arrMasks[ iIndex ] = u64NewMask;
    }
}

void mCVertexMatcher::CPointFinder::ShiftBack( MIUInt a_uAxis, MIInt a_iDistance )
{
    MIUInt const uOffsetC = ( a_uAxis < 2 ? 3 : 1 ) * ( a_uAxis < 1 ? 3 : 1 );
    MIUInt const uOffsetE = ( a_uAxis < 2 ? 4 : 1 ) * ( a_uAxis < 1 ? 4 : 1 );
    MIInt iIndex = 26;
    MIU64 u64Remainder = 0;
    for ( MIUInt u = 27; u--; iIndex -= uOffsetC )
    {
        if ( iIndex < 0 )
            iIndex += 26;
        MIU64 u64NewMask = ( ( m_arrMasks[ iIndex ] & ~GetShiftMask( a_uAxis, 4 - a_iDistance ) ) >> uOffsetE * a_iDistance ) | u64Remainder;
        u64Remainder = ( m_arrMasks[ iIndex ] & GetShiftMask( a_uAxis, 4 - a_iDistance ) ) << uOffsetE * ( 4 - a_iDistance );
        m_arrMasks[ iIndex ] = u64NewMask;
    }
}

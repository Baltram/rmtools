#include "mi_include_3d.h"
#include "mi_include_standard.h"

namespace
{
    MIFloat const fSafetyPuffer0 = 0.000001f;
    MIFloat const fSafetyPuffer1 = 1.000001f;
}

MIUInt   mCVertexMatcher::s_uX;
MIUInt   mCVertexMatcher::s_uY;
MIUInt   mCVertexMatcher::s_uZ;
MIUInt   mCVertexMatcher::s_uXMax;
MIUInt   mCVertexMatcher::s_uYMax;
MIUInt   mCVertexMatcher::s_uZMax;
MIU64    mCVertexMatcher::s_u64Temp;
MIU64    mCVertexMatcher::s_u64Save;
MIU64    mCVertexMatcher::s_u64Keep;
MIUInt   mCVertexMatcher::s_uRotate;
MIUInt   mCVertexMatcher::s_uBackrotate;
MIUInt   mCVertexMatcher::s_uNext;
MIU64    mCVertexMatcher::s_arrFirst[ 9 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
MIUInt   mCVertexMatcher::s_arrSystem3[ 3 ] = { 9, 3, 1 };
MIUInt   mCVertexMatcher::s_arrSystem4[ 3 ] = { 16, 4, 1 };
MIUInt   mCVertexMatcher::s_arrSystem5[ 3 ] = { 25, 5, 1 };
MIUInt   mCVertexMatcher::s_arrSystemCur[ 3 ];
MIUInt   mCVertexMatcher::s_arrExtentsCur[ 3 ];
MIU64 *  mCVertexMatcher::s_pBuffer = 0;
MIU64 *  mCVertexMatcher::s_pBuffer2 = 0;
MIU64 *  mCVertexMatcher::s_arrSearchLevels[ 4 ] = {  0, 0, 0, 0  };
MIU64 *  mCVertexMatcher::s_pCurBlock;
MIU64 *  mCVertexMatcher::s_pCurLevel;
MIUInt * mCVertexMatcher::s_pFoundAt;
mCVertexMatcher::SLookUpPoint *         mCVertexMatcher::s_pPoints;
mCVertexMatcher::SLookUpPoint *         mCVertexMatcher::s_pEndPoints;
mCVertexMatcher::SLookUpCube *          mCVertexMatcher::s_pCubes;
mCVertexMatcher::SLookUpCube *          mCVertexMatcher::s_pCurCube;
mTArray< mCVertexMatcher::SFoundPoint > mCVertexMatcher::s_arrFoundPoints;

void mCVertexMatcher::MatchVerts( mCVec3 const * a_pVertsToMatch, mCVec3 const * a_pTargetVerts, MIUInt a_uVertsToMatchCount, MIUInt a_uTargetVertCount, mTArray< MIUInt > & a_arrCorrelationIndicesDest )
{
    if ( ( a_uTargetVertCount == 0 ) || ( a_uVertsToMatchCount == 0 ) )
        return;

    Init();
    MIUInt uPointCount = ( a_uVertsToMatchCount + a_uTargetVertCount );
    mCVec3 vecMin = *a_pTargetVerts;
    mCVec3 vecMax = *a_pTargetVerts;

    const mCVec3 * pVert = a_pVertsToMatch;
    const mCVec3 * pEndVerts = pVert + a_uVertsToMatchCount;
    for ( ; pVert != pEndVerts; ++pVert )
    {
        if ( pVert->GetX() < vecMin.GetX() )
            vecMin.AccessX() = pVert->GetX();
        else if ( pVert->GetX() > vecMax.GetX() )
            vecMax.AccessX() = pVert->GetX();
        if ( pVert->GetY() < vecMin.GetY() )
            vecMin.AccessY() = pVert->GetY();
        else if ( pVert->GetY() > vecMax.GetY() )
            vecMax.AccessY() = pVert->GetY();
        if ( pVert->GetZ() < vecMin.GetZ() )
            vecMin.AccessZ() = pVert->GetZ();
        else if ( pVert->GetZ() > vecMax.GetZ() )
            vecMax.AccessZ() = pVert->GetZ();
    }

    pVert = a_pTargetVerts;
    pEndVerts = pVert + a_uTargetVertCount;
    for ( ; pVert != pEndVerts; ++pVert )
    {
        if ( pVert->GetX() < vecMin.GetX() )
            vecMin.AccessX() = pVert->GetX();
        else if ( pVert->GetX() > vecMax.GetX() )
            vecMax.AccessX() = pVert->GetX();
        if ( pVert->GetY() < vecMin.GetY() )
            vecMin.AccessY() = pVert->GetY();
        else if ( pVert->GetY() > vecMax.GetY() )
            vecMax.AccessY() = pVert->GetY();
        if ( pVert->GetZ() < vecMin.GetZ() )
            vecMin.AccessZ() = pVert->GetZ();
        else if ( pVert->GetZ() > vecMax.GetZ() )
            vecMax.AccessZ() = pVert->GetZ();
    }

    mCVec3 vecExtents = vecMax - vecMin;
    vecExtents *= fSafetyPuffer1;
    vecMin = vecMax - vecExtents;
    vecExtents *= fSafetyPuffer1;
    vecMax = vecMin + vecExtents;

    MIFloat * pHighest, * pMiddle, * pLowest, * pHigherYZ, * pLowerXZ, * pHigherXY;
    pHigherYZ = ( vecExtents.GetY() > vecExtents.GetZ() ) ? &vecExtents.AccessY() : &vecExtents.AccessZ();
    pLowerXZ = ( vecExtents.GetZ() < vecExtents.GetX() ) ? &vecExtents.AccessZ() : &vecExtents.AccessX();
    pHigherXY = ( vecExtents.GetX() > vecExtents.GetY() ) ? &vecExtents.AccessX() : &vecExtents.AccessY();
    pHighest = ( vecExtents.GetX() > *pHigherYZ ) ? &vecExtents.AccessX() : pHigherYZ;
    pMiddle = ( *pHigherYZ < *pHigherXY ) ? pHigherYZ : pHigherXY;
    pLowest = ( vecExtents.GetY() < *pLowerXZ ) ? &vecExtents.AccessY() : pLowerXZ;

    *pHighest = ( *pHighest == 0.0f ) ? 1.0f : *pHighest;
    *pMiddle = g_max( *pMiddle, ( ( *pHighest ) / uPointCount ) );
    *pLowest = g_max( *pLowest, ( ( *pHighest ) / uPointCount ) );
    *pLowest = g_max( *pLowest, static_cast< MIFloat >( pow( static_cast< MIDouble >( ( *pHighest ) * ( *pMiddle ) / uPointCount ), 0.5 ) ) );

    MIFloat fVolumePerVert = ( vecExtents.GetX() * vecExtents.GetY() * vecExtents.GetZ() / static_cast< MIFloat >( ( a_uTargetVertCount + a_uVertsToMatchCount ) / 2 ) ); 
    MIFloat fEdgeLength = pow( fVolumePerVert, static_cast< MIFloat >( 1.0 / 3.0 ) );
    MIFloat fEdgeLength_4 = ( ( fEdgeLength / 4 ) * fSafetyPuffer1 );
    MIFloat fSearchLevelRadiusDelta = ( ( fEdgeLength / 2 ) / fSafetyPuffer1 );

    s_arrExtentsCur[ 0 ] = static_cast< MIUInt >( ( vecExtents.GetX() / fEdgeLength ) + 1.0f ) + 4;
    s_arrExtentsCur[ 1 ] = static_cast< MIUInt >( ( vecExtents.GetY() / fEdgeLength ) + 1.0f ) + 4;
    s_arrExtentsCur[ 2 ] = static_cast< MIUInt >( ( vecExtents.GetZ() / fEdgeLength ) + 1.0f ) + 4;
    s_arrSystemCur[ 2 ] = 1;
    s_arrSystemCur[ 1 ] = s_arrExtentsCur[ 2 ];
    s_arrSystemCur[ 0 ] = s_arrExtentsCur[ 2 ] * s_arrExtentsCur[ 1 ];

    s_pPoints = new SLookUpPoint [ uPointCount ];
    s_pFoundAt = new MIUInt [ uPointCount ];
    SLookUpPoint ** pPointPointers = new SLookUpPoint * [ uPointCount ];
    SLookUpPoint * pPointsToProcess = ( s_pPoints + a_uTargetVertCount );
    s_pEndPoints = s_pPoints + uPointCount;
    g_memset( s_pPoints, 0, ( sizeof( SLookUpPoint ) * uPointCount ) );
    
    for ( SLookUpPoint * pPoint = s_pPoints; pPoint != pPointsToProcess; ++pPoint )
        pPoint->m_pNext = pPoint;

    s_pCubes = new SLookUpCube [ s_arrExtentsCur[ 0 ] * s_arrExtentsCur[ 1 ] * s_arrExtentsCur[ 2 ] ];
    SLookUpCube * pEndCubes = s_pCubes + ( s_arrExtentsCur[ 0 ] * s_arrExtentsCur[ 1 ] * s_arrExtentsCur[ 2 ] );
    g_memset( s_pCubes, 0, ( sizeof( SLookUpCube ) * ( pEndCubes - s_pCubes ) ) );

    mCVec3 vecOffset = mCVec3( 2 * fEdgeLength, 2 * fEdgeLength, 2 * fEdgeLength ) - vecMin;
    pVert = a_pTargetVerts;
    for ( SLookUpPoint * pPoint = s_pPoints; pPoint != s_pEndPoints; ++pPoint )
    {
        if ( pPoint == pPointsToProcess )
            pVert = a_pVertsToMatch;
        mCVec3 vecRelative = *pVert + vecOffset;

        pPoint->m_pPosition = pVert;
        pPoint->m_uCubeIndex += static_cast< MIUInt >( vecRelative.GetX() / fEdgeLength ) * s_arrSystemCur[ 0 ];
        pPoint->m_uCubeIndex += static_cast< MIUInt >( vecRelative.GetY() / fEdgeLength ) * s_arrSystemCur[ 1 ];
        pPoint->m_uCubeIndex += static_cast< MIUInt >( vecRelative.GetZ() / fEdgeLength ) * s_arrSystemCur[ 2 ];
        ++s_pCubes[ pPoint->m_uCubeIndex ].m_uPointCount;

        pPoint->m_arrCoordinatesWithinCube[ 0 ] = static_cast< MIU8 >( fmod( vecRelative.GetX(), fEdgeLength ) / fEdgeLength_4 );
        pPoint->m_arrCoordinatesWithinCube[ 1 ] = static_cast< MIU8 >( fmod( vecRelative.GetY(), fEdgeLength ) / fEdgeLength_4 );
        pPoint->m_arrCoordinatesWithinCube[ 2 ] = static_cast< MIU8 >( fmod( vecRelative.GetZ(), fEdgeLength ) / fEdgeLength_4 );
        
        s_u64Temp = 1;
        s_u64Temp <<= pPoint->m_arrCoordinatesWithinCube[ 0 ] * s_arrSystem4[ 0 ];
        s_u64Temp <<= pPoint->m_arrCoordinatesWithinCube[ 1 ] * s_arrSystem4[ 1 ];
        s_u64Temp <<= pPoint->m_arrCoordinatesWithinCube[ 2 ] * s_arrSystem4[ 2 ];
        pPoint->m_u64RawPosition = s_u64Temp;
        s_pCubes[ pPoint->m_uCubeIndex ].m_u64PointSpread |= s_u64Temp;
        
        ++pVert;
    }

    SLookUpPoint ** pPointsIt = pPointPointers;
    for ( SLookUpCube * pCube = s_pCubes; pCube != pEndCubes; ++pCube )
    {
        pCube->m_pPoints = pPointsIt;
        pPointsIt += pCube->m_uPointCount;
    }

    for ( SLookUpPoint * pPoint = s_pPoints; pPoint != s_pEndPoints; ++pPoint )
        *( s_pCubes[ pPoint->m_uCubeIndex ].m_pPoints++ ) = pPoint;
    for ( SLookUpCube * pCube = s_pCubes; pCube != pEndCubes; ++pCube )
        pCube->m_pPoints -= pCube->m_uPointCount;

    s_arrFoundPoints.Resize( 2 );
    s_arrFoundPoints[ 0 ].m_fDistance = -1.0f;
    s_arrFoundPoints[ 0 ].m_iOffsetToNext = 1;
    s_arrFoundPoints[ 1 ].m_fDistance = g_fPosInfinity;
    s_arrFoundPoints[ 1 ].m_iOffsetToNext = 0;

    SLookUpPoint * pPointIt;
    SLookUpPoint DummyPoint;
    DummyPoint.m_u8SearchedLevelsCount = 10;
    SLookUpPoint * pFirstGroupPoint;
    MIU8 u8SearchedLevelCount;
    for ( SLookUpPoint * pPoint = pPointsToProcess; pPoint != s_pEndPoints; ++pPoint )
    {
        if ( pPoint->m_pNext == 0 )
        {
            g_memset( s_pFoundAt, 255, ( sizeof( MIUInt ) * uPointCount ) );
            s_arrFoundPoints[ 0 ].m_iOffsetToNext = 1;
            s_arrFoundPoints.Resize( 2 );
            pFirstGroupPoint = pPoint;
            pPoint->m_pNext = &DummyPoint;
            s_pFoundAt[ pPoint - s_pPoints ] = 0;
            u8SearchedLevelCount = 0;

            for ( ; ; )
            {
                SFoundPoint & FoundPoint = s_arrFoundPoints[ s_arrFoundPoints[ 0 ].m_iOffsetToNext ];
                if ( ( FoundPoint.m_fDistance > u8SearchedLevelCount * fSearchLevelRadiusDelta ) && ( u8SearchedLevelCount < 5 ) )
                {
                    pPointIt = pFirstGroupPoint;
                    u8SearchedLevelCount = pFirstGroupPoint->m_u8SearchedLevelsCount + 1;
                    while ( pPointIt->m_u8SearchedLevelsCount < u8SearchedLevelCount )
                    {
                        SearchPoints( pPointIt );
                        pPointIt = pPointIt->m_pNext;
                    }
                }
                else
                {
                    pPointIt = FoundPoint.m_pPoint;
                    if ( pPointIt->m_pNext == 0 )
                    {
                        s_pFoundAt[ pPointIt - s_pPoints ] = 0;
                        s_arrFoundPoints[ 0 ].m_iOffsetToNext += FoundPoint.m_iOffsetToNext;
                        pPointIt->m_pNext = pFirstGroupPoint;
                        pFirstGroupPoint = pPointIt;
                        u8SearchedLevelCount = 0;
                    }
                    else
                    {
                        SLookUpPoint * pPointCache;
                        while ( pFirstGroupPoint != &DummyPoint )
                        {
                            pPointCache = pFirstGroupPoint;
                            pFirstGroupPoint = pFirstGroupPoint->m_pNext;
                            pPointCache->m_pNext = pPointIt->m_pNext;
                        }
                        break;
                    }
                }
            }
        }
    }

    a_arrCorrelationIndicesDest.Resize( a_uVertsToMatchCount );
    MIUInt u = 0;
    for ( SLookUpPoint * pPoint = pPointsToProcess; pPoint != s_pEndPoints; ++pPoint )
        a_arrCorrelationIndicesDest[ u++ ] = static_cast< MIUInt >( pPoint->m_pNext - s_pPoints );

    delete [] s_pCubes;
    delete [] s_pPoints;
    delete [] pPointPointers;
    delete [] s_pFoundAt;
}

MIBool mCVertexMatcher::ComparePoints( mCVertexMatcher::SFoundPoint const & a_Point1, mCVertexMatcher::SFoundPoint const & a_Point2 )
{
    return ( a_Point1.m_fDistance < a_Point2.m_fDistance );
}

void mCVertexMatcher::Init( void )
{
    if ( s_pBuffer )
        return;

    s_u64Temp = 1;
    for ( MIUInt u = 15; u--; )
        s_u64Temp = ( ( s_u64Temp << 1 ) | 1 );
    // 1111111111111111000000000000000000000000000000000000000000000000
    s_arrFirst[ 0 ] = s_u64Temp;
    s_arrFirst[ 1 ] = ( ( s_arrFirst[ 0 ] << 16 ) | s_u64Temp );
    s_arrFirst[ 2 ] = ( ( s_arrFirst[ 1 ] << 16 ) | s_u64Temp );

    s_u64Temp >>= 12;
    for ( MIUInt u = 3; u--; )
        s_u64Temp = ( ( s_u64Temp << 16 ) | s_u64Temp );
    // 1111000000000000111100000000000011110000000000001111000000000000
    s_arrFirst[ 3 ] = s_u64Temp;
    s_arrFirst[ 4 ] = ( ( s_arrFirst[ 3 ] << 4 ) | s_u64Temp );
    s_arrFirst[ 5 ] = ( ( s_arrFirst[ 4 ] << 4 ) | s_u64Temp );

    s_u64Temp = 1;
    for ( MIUInt u = 15; u--; )
        s_u64Temp = ( ( s_u64Temp << 4 ) | 1 );
    // 1000100010001000100010001000100010001000100010001000100010001000
    s_arrFirst[ 6 ] = s_u64Temp;
    s_arrFirst[ 7 ] = ( ( s_arrFirst[ 6 ] << 1 ) | s_u64Temp );
    s_arrFirst[ 8 ] = ( ( s_arrFirst[ 7 ] << 1 ) | s_u64Temp );

    MIFloat fMin = -fSafetyPuffer0;
    MIFloat fMax = fMin + 2.0f;

    s_pBuffer = new MIU64 [ 304 ];
    memset( s_pBuffer, 0, 304 * sizeof( MIU64 ) );
    s_pBuffer2 = new MIU64 [ 125 ];
    memset( s_pBuffer2, 0, 125 * sizeof( MIU64 ) );
    MIU64 s_pCurLevel[ 125 ];

    MIU64 * pInBuffer = s_pBuffer;
    MIU64 ** pInSearchLevel = s_arrSearchLevels; 

    s_arrSystemCur[ 0 ] = 25;
    s_arrSystemCur[ 1 ] = 5;
    s_arrSystemCur[ 2 ] = 1;
    s_arrExtentsCur[ 0 ] = 5;
    s_arrExtentsCur[ 1 ] = 5;
    s_arrExtentsCur[ 2 ] = 5;

    for ( MIUInt u = 0; u != 4; ++u )
    {
        memset( s_pCurLevel, 0, 125 * sizeof( MIU64 ) );
        s_pCurBlock = s_pCurLevel;
        Init3dLoop( 2, 2, 2 );

        while ( s_pCurBlock != 0 )
        {
            s_u64Temp = 1;
            for ( MIUInt xx = 0; xx != 4; ++xx )
            {
                for ( MIUInt yy = 0; yy != 4; ++yy ) {
                    for ( MIUInt zz = 0; zz != 4; ++zz )
                    {
                        MIFloat fLenght = mCVec3( static_cast< MIFloat >( 7 - ( 4 * s_uX + xx ) ), static_cast< MIFloat >( 7 - ( 4 * s_uY + yy ) ), static_cast< MIFloat >( 7 - ( 4 * s_uZ + zz ) ) ).CalcMagnitude();
                        if ( ( fLenght >= fMin ) && ( fLenght < fMax ) )
                        {
                            *s_pCurBlock |= s_u64Temp;
                        }
                        s_u64Temp <<= 1;
                    }
                }
            }
            NextBlock();
        }

        MIUInt arrCounts[ 3 ] = { 2, 2, 2 };
        MIUInt * arrPos[ 3 ];
        arrPos[ 0 ] = &s_uX;
        arrPos[ 1 ] = &s_uY;
        arrPos[ 2 ] = &s_uZ;

        for ( MIUInt v = 0; v != 3; ++v )
        {
            MIUInt uOffset = s_arrSystem4[ v ];
            MIU64 u64Filter = s_arrFirst[ 3 * v ];

            s_pCurBlock = s_pCurLevel;
            Init3dLoop( arrCounts[ 0 ], arrCounts[ 1 ], arrCounts[ 2 ] );

            while ( s_pCurBlock )
            {
                MIU64 & u64Block = *s_pCurBlock;
                s_u64Temp = 0;

                s_u64Temp |= ( ( u64Block & u64Filter ) << ( 3 * uOffset ) );
                u64Filter <<= uOffset;
                s_u64Temp |= ( ( u64Block & u64Filter ) << ( 1 * uOffset ) );
                u64Filter <<= uOffset;
                s_u64Temp |= ( ( u64Block & u64Filter ) >> ( 1 * uOffset ) );
                u64Filter <<= uOffset;
                s_u64Temp |= ( ( u64Block & u64Filter ) >> ( 3 * uOffset ) );
                u64Filter >>= ( 3 * uOffset );

                if ( *arrPos[ v ] == 0 )
                    s_pCurBlock[ s_arrSystem5[ v ] * 3 ] = s_u64Temp;
                else
                    s_pCurBlock[ s_arrSystem5[ v ] ] = s_u64Temp;

                NextBlock();
            }
            
            arrCounts[ v ] = 4;
        }

        MIU64 u64Temp2;
        arrCounts[ 0 ] = arrCounts[ 1 ] = arrCounts[ 2 ] = 5;
        for ( MIUInt v = 0; v != 3; ++v )
        {
            MIU64 & u64Filter = s_arrFirst[ 3 * v ];
            arrCounts[ v ] = 3;
            InitShift( s_arrSystem5, v, 1 );
            s_pCurBlock = ( s_pCurLevel + 124 );
            for ( MIUInt x2 = arrCounts[ 0 ]; x2--; )
            {
                for ( MIUInt y2 = arrCounts[ 1 ]; y2--; )
                {
                    for ( MIUInt z2 = arrCounts[ 2 ]; z2--; )
                    {
                        u64Temp2 = ( *s_pCurBlock & u64Filter );
                        Shift();
                        *s_pCurBlock |= u64Temp2;
                        --s_pCurBlock;
                    }
                    if ( v == 2 )
                        s_pCurBlock -= 2;
                }
                if ( v == 1 )
                    s_pCurBlock -= 10;
            }
            arrCounts[ v ] = 5;
        }

        fMin += 2.0f;
        fMax += 2.0f;

        *( pInSearchLevel++ ) = pInBuffer;
        if ( u < 2 )
        {
            s_pCurBlock = s_pCurLevel + 25 + 5 + 1;
            Init3dLoop( 3, 3, 3 );
            while ( s_pCurBlock )
            {
                *( pInBuffer++ ) = *s_pCurBlock;
                NextBlock();
            }
        }
        else
        {
            memcpy( pInBuffer, s_pCurLevel, 125 * sizeof( MIU64 ) );
            pInBuffer += 125;
        }
    }
}

void mCVertexMatcher::Init3dLoop( MIUInt a_uXCount, MIUInt a_uYCount, MIUInt a_uZCount )
{
    s_uX = s_uY = s_uZ = 0;
    s_uXMax = a_uXCount;
    s_uYMax = a_uYCount;
    s_uZMax = a_uZCount;
}

void mCVertexMatcher::InitShift( MIUInt * a_pSystem, MIUInt a_uDirection, MIUInt a_uCount )
{
    s_uNext = a_pSystem[ a_uDirection ];
    s_u64Save = ~s_arrFirst[ 3 * a_uDirection + 3 - a_uCount ];
    s_u64Keep = ~s_arrFirst[ 3 * a_uDirection + a_uCount - 1 ];
    s_uRotate = a_uCount * s_arrSystem4[ a_uDirection ];
    s_uBackrotate = 4 * s_arrSystem4[ a_uDirection ] - s_uRotate;
}

void mCVertexMatcher::NextBlock( void )
{
    ++s_uZ;
    ++s_pCurBlock;
    if ( s_uZ == s_uZMax )
    {
        s_uZ = 0;
        s_pCurBlock += ( s_arrExtentsCur[ 2 ] - s_uZMax );
        ++s_uY;
        if ( s_uY == s_uYMax )
        {
            s_uY = 0;
            s_pCurBlock += ( ( s_arrExtentsCur[ 1 ] - s_uYMax ) * s_arrSystemCur[ 1 ] );
            ++s_uX;
            if ( s_uX == s_uXMax )
                s_pCurBlock = 0;
        }
    }
}

void mCVertexMatcher::NextCube( void )
{
    ++s_uZ;
    ++s_pCurCube;
    if ( s_uZ == s_uZMax )
    {
        s_uZ = 0;
        s_pCurCube += ( s_arrExtentsCur[ 2 ] - s_uZMax );
        ++s_uY;
        if ( s_uY == s_uYMax )
        {
            s_uY = 0;
            s_pCurCube += ( ( s_arrExtentsCur[ 1 ] - s_uYMax ) * s_arrSystemCur[ 1 ] );
            ++s_uX;
            if ( s_uX == s_uXMax )
                s_pCurCube = 0;
        }
    }
}

void mCVertexMatcher::SearchPoints( SLookUpPoint * a_pPoint )
{
    MIUInt uFoundPointCount = 0;
    SFoundPoint FoundPoint;

    if ( a_pPoint->m_u8SearchedLevelsCount < 4 )
    {
        MIUInt * pSystem = ( a_pPoint->m_u8SearchedLevelsCount < 2 ) ? s_arrSystem3 : s_arrSystem5;
        MIUInt edgeLenght = pSystem[ 1 ];
        MIUInt uSize = ( edgeLenght * edgeLenght * edgeLenght );
        MIUInt uOffset = ( s_arrSystemCur[ 0 ] + s_arrSystemCur[ 1 ] + s_arrSystemCur[ 2 ] ) * ( ( edgeLenght == 5 ) ? 2 : 1 );
        s_pCurBlock = s_pBuffer2;

        g_memcpy( s_pBuffer2, s_arrSearchLevels[ a_pPoint->m_u8SearchedLevelsCount ], uSize * sizeof( MIU64 ) );
        for ( MIUInt u = 0; u != 3; ++u )
        {
            if ( a_pPoint->m_arrCoordinatesWithinCube[ u ] )
            {
                InitShift( pSystem, u, a_pPoint->m_arrCoordinatesWithinCube[ u ] );
                for ( s_pCurBlock = s_pBuffer2 + uSize; s_pCurBlock-- != s_pBuffer2; Shift() );
            }
        }

        s_pCurCube = ( s_pCubes + a_pPoint->m_uCubeIndex - uOffset );
        Init3dLoop( edgeLenght, edgeLenght, edgeLenght );
        while ( s_pCurCube != 0 )
        {
            if ( s_pCurCube->m_u64PointSpread & *s_pCurBlock ) {
                for ( SLookUpPoint ** pCurrentPoint = s_pCurCube->m_pPoints + s_pCurCube->m_uPointCount; pCurrentPoint-- != s_pCurCube->m_pPoints; )
                {
                    if ( ( **pCurrentPoint ).m_u64RawPosition & *s_pCurBlock )
                    {
                        if ( *pCurrentPoint != a_pPoint )
                        {
                            FoundPoint.m_pPoint = *pCurrentPoint;
                            FoundPoint.m_fDistance = ( *( ( **pCurrentPoint ).m_pPosition ) - *( a_pPoint->m_pPosition ) ).CalcMagnitude();
                            MIUInt uFoundAt = s_pFoundAt[ FoundPoint.m_pPoint - s_pPoints ];
                            if ( uFoundAt != ( -1 ) )
                            {
                                SFoundPoint * pFoundPoint2 = &s_arrFoundPoints[ uFoundAt ];
                                if ( pFoundPoint2->m_fDistance > FoundPoint.m_fDistance )
                                {
                                    if ( pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_iOffsetToNext ) {
                                        s_pFoundAt[ pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_pPoint - s_pPoints ] = static_cast< MIUInt >( pFoundPoint2 - &s_arrFoundPoints[ 0 ] );
                                        pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_iOffsetToNext += pFoundPoint2->m_iOffsetToNext;
                                    }
                                    *pFoundPoint2 = pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ];
                                    s_arrFoundPoints.Add( FoundPoint );
                                    ++uFoundPointCount;
                                }
                            }
                            else
                            {
                                s_arrFoundPoints.Add( FoundPoint );
                                ++uFoundPointCount;
                            }
                        }
                    }
                }
            }
            ++s_pCurBlock;
            NextCube();
        }
    }
    else
    {
        for ( SLookUpPoint * pPoint2 = s_pPoints; pPoint2 != s_pEndPoints; ++pPoint2 ) {
            if ( pPoint2 != a_pPoint )
            {
                FoundPoint.m_pPoint = pPoint2;
                FoundPoint.m_fDistance = ( *( pPoint2->m_pPosition ) - *( a_pPoint->m_pPosition ) ).CalcMagnitude();
                MIUInt uFoundAt = s_pFoundAt[ FoundPoint.m_pPoint - s_pPoints ];
                if ( uFoundAt != ( -1 ) )
                {
                    SFoundPoint * pFoundPoint2 = &s_arrFoundPoints[ uFoundAt ];
                    if ( pFoundPoint2->m_fDistance > FoundPoint.m_fDistance )
                    {
                        if ( pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_iOffsetToNext ) {
                            s_pFoundAt[ pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_pPoint - s_pPoints ] = static_cast< MIUInt >( pFoundPoint2 - s_arrFoundPoints.AccessBuffer() );
                            pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ].m_iOffsetToNext += pFoundPoint2->m_iOffsetToNext;
                        }
                        *pFoundPoint2 = pFoundPoint2[ pFoundPoint2->m_iOffsetToNext ];
                        s_arrFoundPoints.Add( FoundPoint );
                        ++uFoundPointCount;
                    }
                }
                else
                {
                    s_arrFoundPoints.Add( FoundPoint );
                    ++uFoundPointCount;
                }
            }
        }
    }

    if ( uFoundPointCount )
    {
        MIInt iOffset;
        SFoundPoint * pOldPoint = &s_arrFoundPoints[ 0 ];
        SFoundPoint * pNewPoint = &s_arrFoundPoints[ s_arrFoundPoints.GetCount() - uFoundPointCount ];
        SFoundPoint * pEndPoints = pNewPoint + uFoundPointCount;
        std::sort( ( &s_arrFoundPoints.Back() + 1 - uFoundPointCount ), ( &s_arrFoundPoints.Back() + 1 ), &ComparePoints );
        for ( MIUInt u = static_cast< MIUInt >( pNewPoint - pOldPoint ); pNewPoint != pEndPoints; ++pNewPoint )
        {
            s_pFoundAt[ pNewPoint->m_pPoint - s_pPoints ] = u++;
            while ( pNewPoint->m_fDistance > pOldPoint[ pOldPoint->m_iOffsetToNext ].m_fDistance )
                pOldPoint += pOldPoint->m_iOffsetToNext;
            iOffset = static_cast< MIInt >( pNewPoint - pOldPoint );
            pNewPoint->m_iOffsetToNext = pOldPoint->m_iOffsetToNext - iOffset;
            pOldPoint->m_iOffsetToNext = iOffset;
            pOldPoint = pNewPoint;
        }
    }

    ++( a_pPoint->m_u8SearchedLevelsCount );
}

void mCVertexMatcher::Shift( void )
{
    s_u64Temp = *s_pCurBlock & s_u64Save;
    s_u64Temp >>= s_uBackrotate;
    *s_pCurBlock = ( *s_pCurBlock << s_uRotate ) & s_u64Keep;
    if ( s_u64Temp )
        s_pCurBlock[ s_uNext ] |= s_u64Temp;
}
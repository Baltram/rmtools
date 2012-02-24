#ifndef MI_VERTEXMATCHER_H_INCLUDED
#define MI_VERTEXMATCHER_H_INCLUDED

class mCVertexMatcher
{
public:
    static void MatchVerts( mCVec3 const * a_pVertsToMatch, mCVec3 const * a_pTargetVerts, MIUInt a_uVertsToMatchCount, MIUInt a_uTargetVertCount, mTArray< MIUInt > & a_arrCorrelationIndicesDest );
private:
    struct SLookUpPoint
    {
        MIU64          m_u64RawPosition;
        MIUInt         m_uCubeIndex;
        MIU8           m_arrCoordinatesWithinCube[ 3 ];
        MIU8           m_u8SearchedLevelsCount;
        mCVec3 const * m_pPosition;
        SLookUpPoint * m_pNext;
    };
    struct SLookUpCube
    {
        MIU64           m_u64PointSpread;
        MIUInt          m_uPointCount;
        SLookUpPoint ** m_pPoints;
    };
    struct SFoundPoint
    {
        MIFloat        m_fDistance;
        MIInt          m_iOffsetToNext;
        SLookUpPoint * m_pPoint;
    };
private:
    static MIBool ComparePoints( SFoundPoint const & a_Point1, SFoundPoint const & a_Point2 );
    static void   Init( void );
    static void   Init3dLoop( MIUInt a_uXCount, MIUInt a_uYCount, MIUInt a_uZCount );
    static void   InitShift( MIUInt * a_pSystem, MIUInt a_uDirection, MIUInt a_uCount );
    static void   NextBlock( void );
    static void   NextCube( void );
    static void   SearchPoints( SLookUpPoint * a_pPoint );
    static void   Shift( void );
private:
    static MIUInt   s_uX;
    static MIUInt   s_uY;
    static MIUInt   s_uZ;
    static MIUInt   s_uXMax;
    static MIUInt   s_uYMax;
    static MIUInt   s_uZMax;
    static MIU64    s_u64Temp;
    static MIU64    s_u64Save;
    static MIU64    s_u64Keep;
    static MIUInt   s_uRotate;
    static MIUInt   s_uBackrotate;
    static MIUInt   s_uNext;
    static MIU64    s_arrFirst[ 9 ];
    static MIUInt   s_arrSystem3[ 3 ];
    static MIUInt   s_arrSystem4[ 3 ];
    static MIUInt   s_arrSystem5[ 3 ];
    static MIUInt   s_arrSystemCur[ 3 ];
    static MIUInt   s_arrExtentsCur[ 3 ];
    static MIU64 *  s_pBuffer;
    static MIU64 *  s_pBuffer2;
    static MIU64 *  s_arrSearchLevels[ 4 ];
    static MIU64 *  s_pCurBlock;
    static MIU64 *  s_pCurLevel;
    static MIUInt * s_pFoundAt;
    static SLookUpPoint *         s_pPoints;
    static SLookUpPoint *         s_pEndPoints;
    static SLookUpCube *          s_pCubes;
    static SLookUpCube *          s_pCurCube;
    static mTArray< SFoundPoint > s_arrFoundPoints;
};

#endif
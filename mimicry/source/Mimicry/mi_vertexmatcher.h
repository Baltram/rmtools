#ifndef MI_VERTEXMATCHER_H_INCLUDED
#define MI_VERTEXMATCHER_H_INCLUDED

class mCVertexMatcher
{
public:
    mCVertexMatcher( mCVec3 const * a_pVertsToMatch, mCVec3 const * a_pTargetVerts, MIUInt a_uNumVertsToMatch, MIUInt a_uNumTargetVerts, MIBool a_bIndirectMatch = MIFalse );
public:
    MIUInt operator [] ( MIUInt a_uIndex ) const;
public:
    mTArray< MIUInt >       & AccessResult( void );
    mTArray< MIUInt > const & GetResult( void ) const;
private:
    class CPoint
    {
    public:
        CPoint( void );
    public:
        MIBool         And( MIU64 a_u64Mask ) const;
        void           AssureMinSearchLevel( MIUInt a_uMinSearchLevel );
        MIBool         ExpandGroup( mCVertexMatcher * a_pMatcher );
        MIUInt         GetChunkIndex( void ) const;
        MIUInt         GetExp( void ) const;
        MIUInt         GetGroupID( void ) const;
        MIUInt         GetMinSearchLevel( void ) const;
        CPoint const * GetNearestPoint( void ) const;
        CPoint       * GetNextC( void ) const;
        CPoint       * GetNextE( void );
        CPoint const * GetNextG( void ) const;
        mCVec3 const & GetPosition( void ) const;
        void           Init( mCVertexMatcher const * a_pMatcher, mCVec3 const * a_pPosition );
        void           RegisterC( mCVertexMatcher * a_pMatcher );
        void           RegisterE( mCVertexMatcher * a_pMatcher, MIUInt a_uGroupID );
        void           SetNearestPoint( CPoint * a_pPoint, MIFloat a_fDistance );
    private:
        void UnRegisterE( mCVertexMatcher * a_pMatcher );
    private:
        mCVec3 const * m_pPosition;
        CPoint       * m_pNearest;
        CPoint       * m_pNextC;
        CPoint       * m_pNextE;
        CPoint       * m_pNextG;
        MIFloat        m_fDistance;
        MIUInt         m_uGroupID;
        MIUInt         m_uExp;
        MIUInt         m_uChunkIndex;
        MIUInt         m_uMinSearchLevel;
    };
    class CChunk
    {
    public:
        CChunk( void );
    public:
        MIBool   And( MIU64 a_u64Mask ) const;
        CPoint * GetFirstPoint( void ) const;
    private:
        MIU64    m_u64Spread;
        CPoint * m_pFirstPoint;
    private:
        friend class CPoint;
    };
    class CPointFinder
    {
    public:
        explicit CPointFinder( MIUInt a_uMaxDistanceSqr );
    public:
        MIBool FindNearestPoint( mCVertexMatcher const * a_pMatcher, CPoint * a_pPoint );
    private:
        MIU64 GetShiftMask( MIUInt a_uAxis, MIUInt a_uDistance );
        void  Init( mCVertexMatcher const * a_pMatcher, MIUInt a_uExp );
        void  Shift( MIUInt a_uAxis, MIInt a_uDistance );
        void  ShiftBack( MIUInt a_uAxis, MIInt a_uDistance );
    private:
        MIU64                   m_arrMasks[ 3 * 3 * 3 ];
        MIInt                   m_arrChunkOffsets[ 3 * 3 * 3 ];
        MIInt                   m_arrMaskOffsets[ 3 * 3 * 3 ];
        MIUInt                  m_uChunkCount;
        MIFloat                 m_fRadius;
        mCVertexMatcher const * m_pMatcher;
        MIUInt                  m_uExp;
    };
private:
    void FindNearestPoint( CPoint * a_pPoint, MIBool a_bForceSuccess ) const;
private:
    mTArray< CPoint > m_arrPointsToMatch;
    mTArray< CPoint > m_arrTargetPoints;
    mTArray< CChunk > m_arrChunks;
    mTArray< MIUInt > m_arrResult;
    CPoint *          m_arrPointsE[ 64 ];
    MIUInt            m_arrExtents[ 3 ];
    MIUInt            m_uUnmatchedPointCount;
    mCVec3            m_vecOrigin;
    MIFloat           m_fEdgeLength;
};

#endif

#ifndef MI_SKIN_H_INCLUDED
#define MI_SKIN_H_INCLUDED

class mCScene;

class mCSkin
{
public:
    mCSkin( mCSkin const & a_skinSource );
    mCSkin( void );
   ~mCSkin( void );
public:
    mCSkin & operator = ( mCSkin const & a_skinSource );
public:
    void         Clear( void );
    mCUnique::ID GetBoneIDByIndex( MIUInt uBoneIndex ) const;
    MIUInt       GetBoneIndex( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const;
    MIUInt       GetBoneIndex( MIUInt a_uWeightIndex ) const;
    MIUInt       GetNumBones( void ) const;
    MIUInt       GetNumInfluencingBones( MIUInt a_uVertexIndex ) const;
    MIUInt       GetNumVerts( void ) const;
    MIUInt       GetNumWeights( void ) const;
    MIFloat      GetWeight( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const;
    MIFloat      GetWeight( MIUInt a_uWeightIndex ) const;
    mEResult     InitSwapping( MIUInt a_uVertCount,
                       mTArray< mCUnique::ID > & a_arrBoneIDs,
                       mTArray< MIUInt > & a_arrVertexIndices,
                       mTArray< MIUInt > & a_arrBoneIndices,
                       mTArray< MIFloat > & a_arrWeights );
    void         LimitNumInfluencingBonesPerVert( MIUInt a_uMax );
    void         MigrateBoneIDs( mCScene const & a_sceneNew, mCScene const & a_sceneCurrent );
    void         Swap( mCSkin & a_skinOther );
private:
    mTArray< mCUnique::ID > m_arrBoneIDs;
    mTArray< MIUInt >       m_arrVertexIndices;
    mTArray< MIUInt >       m_arrBoneIndices;
    mTArray< MIFloat >      m_arrWeights;
    mTArray< MIUInt >       m_arrFirstWeightIndexPerVertex;
};

MI_ERROR_DEFINE_DERIVED( mCSkinError, mCError, EInvalidInitArguments )

#endif

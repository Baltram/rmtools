#include "mi_include_scene.h"

mCSkin::mCSkin( mCSkin const & a_skinSource ) :
    m_arrBoneIDs( a_skinSource.m_arrBoneIDs ),
    m_arrVertexIndices( a_skinSource.m_arrVertexIndices ),
    m_arrBoneIndices( a_skinSource.m_arrBoneIndices ),
    m_arrWeights( a_skinSource.m_arrWeights ),
    m_arrFirstWeightIndexPerVertex( a_skinSource.m_arrFirstWeightIndexPerVertex )
{
}

mCSkin::mCSkin( void )
{
}

mCSkin::~mCSkin( void )
{
}

mCSkin & mCSkin::operator = ( mCSkin const & a_skinSource )
{
    if ( this == &a_skinSource )
        return *this;
    m_arrBoneIDs = a_skinSource.m_arrBoneIDs;
    m_arrVertexIndices = a_skinSource.m_arrVertexIndices;
    m_arrBoneIndices = a_skinSource.m_arrBoneIndices;
    m_arrWeights = a_skinSource.m_arrWeights;
    m_arrFirstWeightIndexPerVertex = a_skinSource.m_arrFirstWeightIndexPerVertex;
    return *this;
}

void mCSkin::Clear( void )
{
    m_arrBoneIDs.Clear();
    m_arrVertexIndices.Clear();
    m_arrBoneIndices.Clear();
    m_arrWeights.Clear();
    m_arrFirstWeightIndexPerVertex.Clear();
}

mCUnique::ID mCSkin::GetBoneIDByIndex( MIUInt uBoneIndex ) const
{
    return m_arrBoneIDs[ uBoneIndex ];
}

MIUInt mCSkin::GetBoneIndex( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const
{
    return m_arrBoneIndices[ m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ] + a_uInfluencingBoneIndex ];
}

MIUInt mCSkin::GetNumBones( void ) const
{
    return m_arrBoneIDs.GetCount();
}

MIUInt mCSkin::GetNumInfluencingBones( MIUInt a_uVertexIndex ) const
{
    MIUInt const uNextFirstIndex = ( a_uVertexIndex + 1 < GetNumVerts() ) ? m_arrFirstWeightIndexPerVertex[ a_uVertexIndex + 1 ] : GetNumWeights();
    return uNextFirstIndex - m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ];
}

MIUInt mCSkin::GetNumVerts( void ) const
{
    return m_arrFirstWeightIndexPerVertex.GetCount();
}

MIUInt mCSkin::GetNumWeights( void ) const
{
    return m_arrWeights.GetCount();
}

MIFloat mCSkin::GetWeight( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const
{
    return m_arrWeights[ m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ] + a_uInfluencingBoneIndex ];
}

mEResult mCSkin::InitSwapping( MIUInt a_uVertCount, 
                       mTArray< mCUnique::ID > & a_arrBoneIDs, 
                       mTArray< MIUInt > & a_arrVertexIndices, 
                       mTArray< MIUInt > & a_arrBoneIndices, 
                       mTArray< MIFloat > & a_arrWeights )
{
    MIUInt const uBoneCount = a_arrBoneIDs.GetCount();
    MIUInt const uWeightCount = a_arrWeights.GetCount();
    if ( ( a_arrVertexIndices.GetCount() != uWeightCount ) || 
         ( a_arrBoneIndices.GetCount() != uWeightCount ) )
    {
        MI_ERROR( mCSkinError, EInvalidInitArguments, "Array sizes differ." );
        return mEResult_False;
    }
    for ( MIUInt u = uWeightCount; u--; )
        if ( a_arrBoneIndices[ u ] >= uBoneCount )
        {
            MI_ERROR( mCSkinError, EInvalidInitArguments, "Bone ID array size exceeded." );
            return mEResult_False;
        }
    MIUInt uLastVertexIndex = a_uVertCount - 1;
    for ( MIUInt u = uWeightCount; u--; )
    {
        if ( a_arrVertexIndices[ u ] > uLastVertexIndex )
        {
            MI_ERROR( mCSkinError, EInvalidInitArguments, "Vertex indices unordered or invalid." );
            return mEResult_False;
        }
        uLastVertexIndex = a_arrVertexIndices[ u ];
    }
    m_arrBoneIDs.Swap( a_arrBoneIDs );
    m_arrVertexIndices.Swap( a_arrVertexIndices );
    m_arrBoneIndices.Swap( a_arrBoneIndices );
    m_arrWeights.Swap( a_arrWeights );
    m_arrFirstWeightIndexPerVertex.Resize( a_uVertCount );
    MIUInt uNextVertexIndex = 0;
    for ( MIUInt u = 0, ue = uWeightCount; u != ue; ++u )
        while ( m_arrVertexIndices[ u ] >= uNextVertexIndex )
            m_arrFirstWeightIndexPerVertex[ uNextVertexIndex++ ] = u;
    while ( uNextVertexIndex != a_uVertCount )
        m_arrFirstWeightIndexPerVertex[ uNextVertexIndex++ ] = uWeightCount;
    return mEResult_Ok;
}

void mCSkin::MigrateBoneIDs( mCScene const & a_sceneNew, mCScene const & a_sceneCurrent )
{
    for ( MIUInt u = 0, ue = m_arrBoneIDs.GetCount(); u != ue; ++u )
        m_arrBoneIDs[ u ] = a_sceneNew.GetNodeAt( a_sceneCurrent.GetNodeIndexByID( m_arrBoneIDs[ u ] ) )->GetID();
}

void mCSkin::Swap( mCSkin & a_skinOther )
{
    if ( this == &a_skinOther )
        return;
    m_arrBoneIDs.Swap( a_skinOther.m_arrBoneIDs );
    m_arrVertexIndices.Swap( a_skinOther.m_arrVertexIndices );
    m_arrBoneIndices.Swap( a_skinOther.m_arrBoneIndices );
    m_arrWeights.Swap( a_skinOther.m_arrWeights );
    m_arrFirstWeightIndexPerVertex.Swap( a_skinOther.m_arrFirstWeightIndexPerVertex );
}

#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_scene.h"
#include "mi_include_standard.h"

mCMesh::mCMesh( mCMesh const & a_meshSource ) :
    m_arrVertices( a_meshSource.m_arrVertices ),
    m_arrTextureVertices( a_meshSource.m_arrTextureVertices ),
    m_arrVertexNormals( a_meshSource.m_arrVertexNormals ),
    m_arrVertexTangents( a_meshSource.m_arrVertexTangents ),
    m_arrFaces( a_meshSource.m_arrFaces ),
    m_arrTextureVertexFaces( a_meshSource.m_arrTextureVertexFaces ),
    m_arrVertexNormalFaces( a_meshSource.m_arrVertexNormalFaces ),
    m_arrVertexTangentFaces( a_meshSource.m_arrVertexTangentFaces ),
    m_arrVertexColors( a_meshSource.m_arrVertexColors )
{
}

mCMesh::mCMesh( void )
{
}

mCMesh::~mCMesh( void )
{
}

mCMesh & mCMesh::operator = ( mCMesh const & a_meshSource )
{
    mCMesh meshCopy( a_meshSource );
    Swap( meshCopy );
    return *this;
}

mCMaxFace * mCMesh::AccessFaces( void )
{
    return m_arrFaces.AccessBuffer();
}

mCVec3 * mCMesh::AccessTVerts( void )
{
    return m_arrTextureVertices.AccessBuffer();
}

mCFace * mCMesh::AccessTVFaces( void )
{
    return m_arrTextureVertexFaces.AccessBuffer();
}

mCColor * mCMesh::AccessVertexColors( void )
{
    return m_arrVertexColors.AccessBuffer();
}

mCVec3 * mCMesh::AccessVerts( void )
{
    return m_arrVertices.AccessBuffer();
}

mCFace * mCMesh::AccessVNFaces( void )
{
    return m_arrVertexNormalFaces.AccessBuffer();
}

mCVec3 * mCMesh::AccessVNormals( void )
{
    return m_arrVertexNormals.AccessBuffer();
}

mCVec3 * mCMesh::AccessVTangents( void )
{
    return m_arrVertexTangents.AccessBuffer();
}

mCFace * mCMesh::AccessVTFaces( void )
{
    return m_arrVertexTangentFaces.AccessBuffer();
}

mCBox mCMesh::CalcExtents( void ) const
{
    mCBox boxResult;
    boxResult.Invalidate();
    for ( MIUInt u = m_arrVertices.GetCount(); u--; )
        boxResult |= m_arrVertices[ u ];
    return boxResult;
}

void mCMesh::CalcVNormalsBySGs( void )
{
    CalcVNormals( MIFalse );
}

void mCMesh::CalcVNormalsByAngle( MIFloat a_fMaxAngleDeg )
{
    CalcVNormals( MITrue, cos( a_fMaxAngleDeg * g_fPi / 180.0f ) );
}

void mCMesh::Clear( void )
{
    mCMesh meshTemp;
    Swap( meshTemp );
}

void mCMesh::CopyTVFacesFromFaces( void )
{
    if ( !GetNumTVerts() )
        return;
    for ( MIUInt u = m_arrFaces.GetCount(); u--; m_arrTextureVertexFaces[ u ] = m_arrFaces[ u ] );
}

void mCMesh::CopyVNFacesFromFaces( void )
{
    if ( !GetNumVNormals() )
        return;
    for ( MIUInt u = m_arrFaces.GetCount(); u--; m_arrVertexNormalFaces[ u ] = m_arrFaces[ u ] );
}

void mCMesh::CopyVTFacesFromFaces( void )
{
    if ( !GetNumVTangents() )
        return;
    for ( MIUInt u = m_arrFaces.GetCount(); u--; m_arrVertexTangentFaces[ u ] = m_arrFaces[ u ] );
}

mCMaxFace const * mCMesh::GetFaces( void ) const
{
    return m_arrFaces.GetBuffer();
}

MIUInt mCMesh::GetNumFaces( void ) const
{
    return m_arrFaces.GetCount();
}

MIUInt mCMesh::GetNumTVerts( void ) const
{
    return m_arrTextureVertices.GetCount();
}

MIUInt mCMesh::GetNumVerts( void ) const
{
    return m_arrVertices.GetCount();
}

MIUInt mCMesh::GetNumVNormals( void ) const
{
    return m_arrVertexNormals.GetCount();
}

MIUInt mCMesh::GetNumVTangents( void ) const
{
    return m_arrVertexTangents.GetCount();
}

mCVec3 const * mCMesh::GetTVerts( void ) const
{
    return m_arrTextureVertices.GetBuffer();
}

mCFace const * mCMesh::GetTVFaces( void ) const
{
    return m_arrTextureVertexFaces.GetBuffer();
}

mCColor const * mCMesh::GetVertexColors( void ) const
{
    return m_arrVertexColors.GetBuffer();
}

mCVec3 const * mCMesh::GetVerts( void ) const
{
    return m_arrVertices.GetBuffer();
}

mCFace const * mCMesh::GetVNFaces( void ) const
{
    return m_arrVertexNormalFaces.GetBuffer();
}

mCVec3 const * mCMesh::GetVNormals( void ) const
{
    return m_arrVertexNormals.GetBuffer();
}

mCVec3 const * mCMesh::GetVTangents( void ) const
{
    return m_arrVertexTangents.GetBuffer();
}

mCFace const * mCMesh::GetVTFaces( void ) const
{
    return m_arrVertexTangentFaces.GetBuffer();
}

MIBool mCMesh::HasTVFaces( void ) const
{
    return ( GetNumTVerts() != 0 );
}

MIBool mCMesh::HasVertexColors( void ) const
{
    return ( m_arrVertexColors.GetCount() != 0 );
}

MIBool mCMesh::HasVNFaces( void ) const
{
    return ( GetNumVNormals() != 0 );
}

MIBool mCMesh::HasVTFaces( void ) const
{
    return ( GetNumVTangents() != 0 );
}

void mCMesh::SetHasVertexColors( MIBool a_bHasVertexColors )
{
    m_arrVertexColors.Resize( a_bHasVertexColors ? GetNumVerts() : 0 );
}

void mCMesh::SetNumFaces( MIUInt a_uCount )
{
    m_arrFaces.Resize( a_uCount );
    if ( HasTVFaces() )
        m_arrTextureVertexFaces.Resize( a_uCount );
    if ( HasVNFaces() )
        m_arrVertexNormalFaces.Resize( a_uCount );
    if ( HasVTFaces() )
        m_arrVertexTangentFaces.Resize( a_uCount );
}

void mCMesh::SetNumTVerts( MIUInt a_uCount )
{
    m_arrTextureVertices.Resize( a_uCount );
    m_arrTextureVertexFaces.Resize( a_uCount ? GetNumFaces() : 0 );
}

void mCMesh::SetNumVerts( MIUInt a_uCount )
{
    m_arrVertices.Resize( a_uCount );
    if ( HasVertexColors() )
        m_arrVertexColors.Resize( a_uCount );
}

void mCMesh::SetNumVNormals( MIUInt a_uCount )
{
    m_arrVertexNormals.Resize( a_uCount );
    m_arrVertexNormalFaces.Resize( a_uCount ? GetNumFaces() : 0 );
}

void mCMesh::SetNumVTangents( MIUInt a_uCount )
{
    m_arrVertexTangents.Resize( a_uCount );
    m_arrVertexTangentFaces.Resize( a_uCount ? GetNumFaces() : 0 );
}

namespace
{
    MIBool CompareMaxFacesByMatID( mCMaxFace const & a_faceFace1, mCMaxFace const & a_faceFace2 )
    {
        return ( a_faceFace1.GetMatID() < a_faceFace2.GetMatID() );
    };
};

void mCMesh::SortFacesByMatID( void )
{
    std::sort( &m_arrFaces.Front(), &m_arrFaces.Back(), &CompareMaxFacesByMatID );
}

void mCMesh::Swap( mCMesh & a_meshOther )
{
    m_arrVertices.Swap( a_meshOther.m_arrVertices );
    m_arrTextureVertices.Swap( a_meshOther.m_arrTextureVertices );
    m_arrVertexNormals.Swap( a_meshOther.m_arrVertexNormals );
    m_arrVertexTangents.Swap( a_meshOther.m_arrVertexTangents );
    m_arrFaces.Swap( a_meshOther.m_arrFaces );
    m_arrTextureVertexFaces.Swap( a_meshOther.m_arrTextureVertexFaces );
    m_arrVertexNormalFaces.Swap( a_meshOther.m_arrVertexNormalFaces );
    m_arrVertexTangentFaces.Swap( a_meshOther.m_arrVertexTangentFaces );
    m_arrVertexColors.Swap( a_meshOther.m_arrVertexColors );
}

void mCMesh::CalcIndicesPerVert( mTArray< MIUInt > & a_arrFirstIndexPerVert, mTArray< MIUInt > & a_arrNextIndexPerIndex )
{
    MIUInt const uVertexCount = GetNumVerts();
    MIUInt const uIndexCount = GetNumFaces() * 3;
    a_arrFirstIndexPerVert.Resize( uVertexCount );
    a_arrNextIndexPerIndex.Resize( uIndexCount );
    for ( MIUInt u = uVertexCount; u--; a_arrFirstIndexPerVert[ u ] = uIndexCount );
    for ( MIUInt u = uIndexCount; u--; )
    {
        MIUInt const uVertIndex = m_arrFaces[ u / 3 ][ u % 3 ];
        a_arrNextIndexPerIndex[ u ] = a_arrFirstIndexPerVert[ uVertIndex ];
        a_arrFirstIndexPerVert[ uVertIndex ] = u;
    }
}

void mCMesh::CalcVNormals( MIBool const a_bUseAnglesNotSGs, MIFloat const a_fMinCosine )
{
    struct SFaceAngles
    {
        MIFloat m_arrFaceAngles[ 3 ];
    };
    MIUInt const           uFaceCount = GetNumFaces();
    MIUInt const           uIndexCount = uFaceCount * 3;
    mTArray< mCVec3 >      arrVertexNormals( uIndexCount );
    mTArray< MIUInt >      arrFirstIndexPerVert, arrNextIndexPerIndex;
    mTArray< SFaceAngles > arrFaceAnglesPerFace( uFaceCount );
    mTArray< mCVec3 >      arrFaceNormals( uFaceCount );

    for ( MIUInt u = uFaceCount; u--; arrFaceNormals[ u ] = m_arrFaces[ u ].CalcNormal( m_arrVertices, arrFaceAnglesPerFace[ u ].m_arrFaceAngles ) );
    CalcIndicesPerVert( arrFirstIndexPerVert, arrNextIndexPerIndex );
    m_arrVertexNormalFaces.Resize( uFaceCount );

    for ( MIUInt u = GetNumVerts(); u--; )
    {
        for ( MIUInt v = arrFirstIndexPerVert[ u ]; v != uIndexCount; v = arrNextIndexPerIndex[ v ] )
        {
            MIUInt const uFaceIndex = v / 3;
            MIUInt const uComposedNormalIndex = arrVertexNormals.GetCount();
            mCVec3 & vecComposedNormal = arrVertexNormals.AddNew() = mCVec3( 0.0f, 0.0f, 0.0f );
            for ( MIUInt w = v, * pLast = 0; w != uIndexCount; pLast = &arrNextIndexPerIndex[ w ], w = *pLast )
            {
                MIUInt const uFaceIndex2 = w / 3;
                if ( uFaceIndex != uFaceIndex2 )
                {
                    if ( a_bUseAnglesNotSGs )
                    {
                        if ( arrFaceNormals[ uFaceIndex ].CalcDotProduct( arrFaceNormals[ uFaceIndex2 ] ) < a_fMinCosine )
                            continue;
                    }
                    else if ( !( m_arrFaces[ uFaceIndex ].GetSGs() & m_arrFaces[ uFaceIndex2 ].GetSGs() ) )
                    {
                        continue;
                    }
                }
                vecComposedNormal += arrFaceNormals[ uFaceIndex2 ] * arrFaceAnglesPerFace[ uFaceIndex2 ].m_arrFaceAngles[ w % 3 ];
                m_arrVertexNormalFaces[ uFaceIndex2 ][ w % 3 ] = uComposedNormalIndex;
                if ( pLast )
                    *pLast = arrNextIndexPerIndex[ w ];
            }
        }
    }

    for ( MIUInt u = arrVertexNormals.GetCount(); u--; arrVertexNormals[ u ].Normalize() );
    arrVertexNormals.UnReserve();
    m_arrVertexNormals.Swap( arrVertexNormals );
}

#endif
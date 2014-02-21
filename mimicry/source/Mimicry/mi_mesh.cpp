#include "mi_include_scene.h"
#include "mi_include_standard.h"

mCMesh::mCMesh( mCMesh const & a_meshSource ) :
    m_arrVertices( a_meshSource.m_arrVertices ),
    m_arrTextureVertices( a_meshSource.m_arrTextureVertices ),
    m_arrVertexNormals( a_meshSource.m_arrVertexNormals ),
    m_arrVertexTangents( a_meshSource.m_arrVertexTangents ),
    m_arrVTHandiness( a_meshSource.m_arrVTHandiness ),
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
    if ( this == &a_meshSource )
        return *this;
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

MIFloat * mCMesh::AccessVTHandiness( void )
{
    return m_arrVTHandiness.AccessBuffer();
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

void mCMesh::CalcFakeTexturing( void )
{
    SetNumTVerts( 3 );
    mCVec3 * const pTVerts = AccessTVerts();
    mCFace * const pTVFaces = AccessTVFaces();
    pTVerts[ 0 ] = mCVec3( 0.0f, 0.0f, 0.0f );
    pTVerts[ 1 ] = mCVec3( 1.0f, 0.0f, 0.0f );
    pTVerts[ 2 ] = mCVec3( 0.0f, 1.0f, 0.0f );
    mCFace const faceTemp( 0, 1, 2 );
    for ( MIUInt u = GetNumFaces(); u--; pTVFaces[ u ] = faceTemp );
}

mEResult mCMesh::CalcFTangents( mTArray< mCVec3 > & a_arrDest, mTArray<MIFloat> &a_arrHandiness )
{
    if ( !HasTVFaces() )
        return mEResult_False;
    MIUInt const uFaceCount = GetNumFaces();
    MIUInt const uVertCount = GetNumVerts();
    mCVec3 const * pVerts = GetVerts();
    mCMaxFace const * pFaces = GetFaces();
    mCVec3 const * pTVerts = GetTVerts();
    mCFace const * pTVFaces = GetTVFaces();

    a_arrDest.Resize( uFaceCount );
    a_arrHandiness.Resize( uFaceCount );
    mCVec3 * pFTangents = a_arrDest.AccessBuffer();
    MIFloat * pHandiness = a_arrHandiness.AccessBuffer();
    for ( MIUInt u = uFaceCount; u--; )
    {
        mCVec3 const & vecVert0 = pVerts[ pFaces[ u ][ 0 ] ];
        mCVec3 const & vecVert1 = pVerts[ pFaces[ u ][ 1 ] ];
        mCVec3 const & vecVert2 = pVerts[ pFaces[ u ][ 2 ] ];
        mCVec3 const & vecTVert0 = pTVerts[ pTVFaces[ u ][ 0 ] ];
        mCVec3 const & vecTVert1 = pTVerts[ pTVFaces[ u ][ 1 ] ];
        mCVec3 const & vecTVert2 = pTVerts[ pTVFaces[ u ][ 2 ] ];
        mCVec3 const vecEdge1 = vecVert1 - vecVert0;
        mCVec3 const vecEdge2 = vecVert2 - vecVert0;
        MIFloat const fDeltaU1 = vecTVert1.GetX() - vecTVert0.GetX();
        MIFloat const fDeltaU2 = vecTVert2.GetX() - vecTVert0.GetX();
        MIFloat const fDeltaV1 = vecTVert1.GetY() - vecTVert0.GetY();
        MIFloat const fDeltaV2 = vecTVert2.GetY() - vecTVert0.GetY();
        MIFloat const fR = fDeltaU1 * fDeltaV2 - fDeltaU2 * fDeltaV1;
        pFTangents[ u ] = ( ( vecEdge1 * fDeltaV2 ) - ( vecEdge2 * fDeltaV1 ) ) / fR;
        mCVec3 vecBitangent = ( ( vecEdge2 * fDeltaU1 ) - ( vecEdge1 * fDeltaU2 ) ) / fR;
        pHandiness[ u ] = pFaces[ u ].CalcNormal( pVerts, uVertCount ).CalcCrossProduct( pFTangents[ u ] ).CalcDotProduct( vecBitangent ) < 0.0f ? -1.0f : 1.0f;
    }
    return mEResult_Ok;
}

void mCMesh::CalcUniVertMesh( mTArray< SUniVert > & a_arrUniVertsDest, mTArray< mCFace > & a_arrUVFacesDest, MIBool a_bIgnoreVTangents ) const
{
    MIUInt const uVertCount = GetNumVerts();
    MIUInt const uFaceCount = GetNumFaces();
    MIUInt const uIndexCount = uFaceCount * 3;
    mCVec3 const * const pVerts = GetVerts();
    mCMaxFace const * const pFaces = GetFaces();
    mCVec3 const * const pTVerts = GetTVerts();
    mCFace const * const pTVFaces = GetTVFaces();
    mCVec3 const * const pVNormals = GetVNormals();
    mCVec3 const * const pVTangents = GetVTangents();
    MIFloat const * const pVTHandiness = GetVTHandiness();
    mCFace const * const pVNFaces = GetVNFaces();
    mCFace const * const pVTFaces = GetVTFaces();
    mCColor const * const pVColors = GetVertexColors();
    MIBool const bHasTVFaces = HasTVFaces();
    MIBool const bHasVNFaces = HasVNFaces();
    MIBool const bHasVTFaces = HasVTFaces();
    MIBool const bHasVColors = HasVertexColors();
    mTArray< MIUInt > arrFirstIndexPerVert, arrNextIndexPerIndex;

    CalcIndicesPerVert( arrFirstIndexPerVert, arrNextIndexPerIndex );
    a_arrUVFacesDest.Resize( uFaceCount );
    a_arrUniVertsDest.Resize( uIndexCount );
    g_memset( a_arrUniVertsDest.AccessBuffer(), 0, sizeof( SUniVert ) * uIndexCount );

    MIUInt uUniVertCount = 0;
    for ( MIUInt u = 0; u != uVertCount; ++u )
    {
        for ( MIUInt v = arrFirstIndexPerVert[ u ]; v != uIndexCount; v = arrNextIndexPerIndex[ v ] )
        {
            MIUInt const v_3 = v / 3, v_m_3 = v % 3;
            MIUInt const uMatIdA = pFaces[ v_3 ].GetMatID();
            mCVec3 const * const pTVertA = bHasTVFaces ? pTVerts + pTVFaces[ v_3 ][ v_m_3 ] : 0;
            mCVec3 const * const pVNormalA = bHasVNFaces ? pVNormals + pVNFaces[ v_3 ][ v_m_3 ] : 0;
            mCVec3 const * const pVTangentA = bHasVTFaces ? pVTangents + pVTFaces[ v_3 ][ v_m_3 ] : 0;
            MIFloat const fVTHandinessA = bHasVTFaces ? pVTHandiness[ pVTFaces[ v_3 ][ v_m_3 ] ] : 0.0f;
            for ( MIUInt w = v, * pLast = &arrNextIndexPerIndex[ v ]; w != uIndexCount; w = *pLast )
            {
                MIUInt const w_3 = w / 3, w_m_3 = w % 3;
                if ( w != v )
                {
                    MIUInt const uMatIdB = pFaces[ w_3 ].GetMatID();
                    mCVec3 const * const pTVertB = bHasTVFaces ? pTVerts + pTVFaces[ w_3 ][ w_m_3 ] : 0;
                    mCVec3 const * const pVNormalB = bHasVNFaces ? pVNormals + pVNFaces[ w_3 ][ w_m_3 ] : 0;
                    mCVec3 const * const pVTangentB = bHasVTFaces ? pVTangents + pVTFaces[ w_3 ][ w_m_3 ] : 0;
                    MIFloat const fVTHandinessB = bHasVTFaces ? pVTHandiness[ pVTFaces[ w_3 ][ w_m_3 ] ] : 0.0f;
                    if ( ( uMatIdB != uMatIdA ) || 
                         ( bHasTVFaces && !pTVertB->IsSimilar( *pTVertA, 0.001f, MITrue ) ) || 
                         ( bHasVNFaces && !pVNormalB->IsSimilar( *pVNormalA, 0.001f ) ) ||
                         ( !a_bIgnoreVTangents && bHasVTFaces && !pVTangentB->IsSimilar( *pVTangentA, 0.001f ) ) ||
                         ( !a_bIgnoreVTangents && ( fVTHandinessA != fVTHandinessB ) ) )
                    {
                        pLast = &arrNextIndexPerIndex[ w ];
                        continue;
                    }
                    *pLast = arrNextIndexPerIndex[ w ];
                }
                a_arrUVFacesDest[ w_3 ][ w_m_3 ] = uUniVertCount;
            }
            SUniVert & UVert = a_arrUniVertsDest[ uUniVertCount++ ];
            UVert.m_uMatID = pFaces[ v_3 ].GetMatID();
            UVert.m_uBaseVertIndex = pFaces[ v_3 ][ v_m_3 ];
            UVert.m_pVert = pVerts + UVert.m_uBaseVertIndex;
            if ( bHasVColors )
                UVert.m_pVColor = pVColors + UVert.m_uBaseVertIndex;
            if ( bHasTVFaces )
                UVert.m_pTVert = pTVerts + pTVFaces[ v_3 ][ v_m_3 ];
            if ( bHasVNFaces )
                UVert.m_pVNormal = pVNormals + pVNFaces[ v_3 ][ v_m_3 ];
            if ( bHasVTFaces )
                UVert.m_pVTangent = pVTangents + pVTFaces[ v_3 ][ v_m_3 ];
            UVert.m_fVTHandiness = fVTHandinessA;
        }
    }
    mTArray< MIUInt > arrNewUVertIndices( MI_DW_INVALID, uUniVertCount );
    mTArray< MIUInt > arrReorderPattern( 0, uUniVertCount );
    for ( MIUInt u = 0, uNewUVertIndex = 0; u != uFaceCount; ++u )
    {
        for ( MIUInt v = 0; v != 3; ++v )
        {
            MIUInt & uUVertIndex = a_arrUVFacesDest[ u ][ v ];
            if ( arrNewUVertIndices[ uUVertIndex ] == MI_DW_INVALID )
            {
                arrReorderPattern[ uNewUVertIndex ] = uUVertIndex;
                arrNewUVertIndices[ uUVertIndex ] = uNewUVertIndex++;
            }
            uUVertIndex = arrNewUVertIndices[ uUVertIndex ];
        }
    }
    g_reorder( a_arrUniVertsDest.AccessBuffer(), arrReorderPattern.GetBuffer(), uUniVertCount );
    a_arrUniVertsDest.Resize( uUniVertCount );
    a_arrUniVertsDest.UnReserve();
}

void mCMesh::CalcVNormalsBySGs( void )
{
    CalcVNormals( MIFalse );
}

void mCMesh::CalcVNormalsByAngle( MIFloat a_fMaxAngleDeg )
{
    CalcVNormals( MITrue, cos( a_fMaxAngleDeg * g_fPi / 180.0f ) );
}

mEResult mCMesh::CalcVTangents( void )
{
    mTArray< mCVec3 > arrFTangents;
    mTArray< MIFloat > arrFTHandiness;
    if ( !HasVNFaces() || ( CalcFTangents( arrFTangents, arrFTHandiness ) == mEResult_False ) )
        return mEResult_False;
    MIUInt const uFaceCount = GetNumFaces();
    MIUInt const uIndexCount = uFaceCount * 3;
    mCVec3 const * pTVerts = GetTVerts();
    mCFace const * pTVFaces = GetTVFaces();
    mCVec3 const * pVNormals = GetVNormals();
    mCFace const * pVNFaces = GetVNFaces();

    MIUInt uVTangentCount = 0;
    mTArray< mCVec3 > arrVTangents( mCVec3( 0.0f, 0.0f, 0.0f ), uIndexCount );
    mTArray< MIFloat > arrVTHandiness( 0.0f, uIndexCount );
    mTArray< mCVec3 > arrVNormals = arrVTangents;
    mTArray< MIUInt > arrFirstIndexPerVert, arrNextIndexPerIndex;
    CalcIndicesPerVert( arrFirstIndexPerVert, arrNextIndexPerIndex );

    m_arrVertexTangentFaces.Resize( uFaceCount );
    for ( MIUInt u = GetNumVerts(); u--; )
    {
        for ( MIUInt v = arrFirstIndexPerVert[ u ]; v != uIndexCount; v = arrNextIndexPerIndex[ v ] )
        {
            MIUInt const v_3 = v / 3, v_m_3 = v % 3;
            arrVTHandiness[ uVTangentCount ] = arrFTHandiness[ v_3 ];
            mCVec3 & vecVTangent = arrVTangents[ uVTangentCount ];
            mCVec3 & vecVNormal = arrVNormals[ uVTangentCount++ ];
            for ( MIUInt w = v, * pLast = &arrNextIndexPerIndex[ v ]; w != uIndexCount; w = *pLast )
            {
                MIUInt const w_3 = w / 3, w_m_3 = w % 3;
                if ( w != v )
                {
                    if ( ( !pTVerts[ pTVFaces[ v_3 ][ v_m_3 ] ].IsSimilar( pTVerts[ pTVFaces[ w_3 ][ w_m_3 ] ], 0.001f, MITrue ) ) ||
                         ( arrFTHandiness[ v_3 ] != arrFTHandiness[ w_3 ] ) ||
                         ( arrFTangents[ v_3 ].CalcAngleDeg( arrFTangents[ w_3 ] ) > 90.0f ) )
                    {
                        pLast = &arrNextIndexPerIndex[ w ];
                        continue;
                    }
                    *pLast = arrNextIndexPerIndex[ w ];
                }
                vecVNormal += pVNormals[ pVNFaces[ w_3 ][ w_m_3 ] ];
                vecVTangent += arrFTangents[ w_3 ];
                m_arrVertexTangentFaces[ w_3 ][ w_m_3 ] = uVTangentCount - 1;
            }
            vecVNormal.Normalize();
            vecVTangent = ( vecVTangent - vecVNormal * vecVNormal.CalcDotProduct( vecVTangent ) ).Normalize();
        }
    }

    arrVTangents.Resize( uVTangentCount );
    arrVTHandiness.Resize( uVTangentCount );
    arrVTangents.UnReserve();
    arrVTHandiness.UnReserve();
    m_arrVertexTangents.Swap( arrVTangents );
    m_arrVTHandiness.Swap( arrVTHandiness );
    return mEResult_Ok;
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

namespace
{
    template< class C, class D >
    void Extract( mTArray< C > const & a_arrFaces, mTArray< C > & a_arrFacesDest, mTArray< D > const & a_arrVerts, mTArray< D > & a_arrVertsDest, mTArray< MIBool > const & a_arrFaceMask )
    {
        MIUInt const uFaceCount = a_arrFaces.GetCount(), uVertCount = a_arrVerts.GetCount();
        a_arrFacesDest.Clear();
        a_arrVertsDest.Clear();
        if ( !uFaceCount || !uVertCount )
            return;
        a_arrFacesDest.Reserve( uFaceCount );
        a_arrVertsDest.Reserve( uVertCount );
        mTArray< MIUInt > arrNewVertIndices( MI_DW_INVALID, uVertCount );
        for ( MIUInt u = 0; u != uFaceCount; ++u )
        {
            if ( !a_arrFaceMask[ u ] )
                continue;
            C & faceDest = a_arrFacesDest.AddNew() = a_arrFaces[ u ];
            for ( MIUInt v = 0; v != 3; ++v )
            {
                MIUInt & uVert = faceDest[ v ];
                if ( arrNewVertIndices[ uVert ] == MI_DW_INVALID )
                {
                    arrNewVertIndices[ uVert ] = a_arrVertsDest.GetCount();
                    a_arrVertsDest.Add( a_arrVerts[ uVert ] );
                }
                uVert = arrNewVertIndices[ uVert ];
            }
        }
        a_arrFacesDest.UnReserve();
        a_arrVertsDest.UnReserve();
    }
}

void mCMesh::ExtractByID( MIUInt a_uMatID, mCMesh & a_meshDest ) const
{
    mCMesh meshDest;
    mTArray< MIBool > arrFaceMask( MIFalse, m_arrFaces.GetCount() );
    for ( MIUInt u = 0, ue = GetNumFaces(); u != ue; ++u )
        arrFaceMask[ u ] = m_arrFaces[ u ].GetMatID() == a_uMatID;
    Extract( m_arrFaces, meshDest.m_arrFaces, m_arrVertices, meshDest.m_arrVertices, arrFaceMask );
    Extract( m_arrTextureVertexFaces, meshDest.m_arrTextureVertexFaces, m_arrTextureVertices, meshDest.m_arrTextureVertices, arrFaceMask );
    Extract( m_arrVertexNormalFaces, meshDest.m_arrVertexNormalFaces, m_arrVertexNormals, meshDest.m_arrVertexNormals, arrFaceMask );
    Extract( m_arrVertexTangentFaces, meshDest.m_arrVertexTangentFaces, m_arrVertexTangents, meshDest.m_arrVertexTangents, arrFaceMask );
    Extract( m_arrFaces, meshDest.m_arrFaces, m_arrVertexColors, meshDest.m_arrVertexColors, arrFaceMask );
    a_meshDest.Swap( meshDest );
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

MIFloat const * mCMesh::GetVTHandiness( void ) const
{
    return m_arrVTHandiness.GetBuffer();
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

namespace
{
    template< class C >
    void ShiftFaces( mTArray< C > & a_arrFaces, MIUInt a_uCount )
    {
        for ( MIUInt u = 0, ue = a_arrFaces.GetCount(); u != ue; ++u )
            for ( MIUInt v = 0; v != 3; ++v )
                a_arrFaces[ u ][ v ] += a_uCount;
    }
}

void mCMesh::Merge( mCMesh a_meshOther, mCMaterialBase const * a_pMaterial, mCMaterialBase const * a_pOtherMaterial, mCMultiMaterial * a_pComposedMaterialDest )
{
    mCMesh * arrMeshes[] = { this, &a_meshOther };
    mTArray< mCMaxFace > * arrFaceArrays[] = { &m_arrFaces, &a_meshOther.m_arrFaces };
    mCMaterialBase const * arrMats[] = { a_pMaterial, a_pOtherMaterial };
    mCMultiMaterial arrMultiMats[ 2 ];
    for ( MIUInt u = 0; u != 2; ++u )
    {
        mCMultiMaterial const * pMultiMaterial = dynamic_cast< mCMultiMaterial const * >( arrMats[ u ] );
        mCMaterial const * pMaterial = dynamic_cast< mCMaterial const * >( arrMats[ u ] );
        if ( pMultiMaterial )
            arrMultiMats[ u ] = *pMultiMaterial;
        else if ( pMaterial )
        {
            arrMultiMats[ u ].AccessName() = pMaterial->GetName();
            arrMultiMats[ u ].AccessSubMaterials().Add( *pMaterial );
        }
        MIUInt uSubMaterialCount = g_max( 1U, arrMultiMats[ u ].GetSubMaterials().GetCount() );
        for ( MIUInt v = 0, ve = arrFaceArrays[ u ]->GetCount(); v != ve; ++v )
            ( *arrFaceArrays[ u ] )[ v ].AccessMatID() %= uSubMaterialCount;
        if ( !arrMeshes[ u ]->HasTVFaces() && arrMeshes[ 1 - u ]->HasTVFaces() )
            arrMeshes[ u ]->CalcFakeTexturing();
        if ( !arrMeshes[ u ]->HasVNFaces() && arrMeshes[ 1 - u ]->HasVNFaces() )
            arrMeshes[ u ]->CalcVNormalsBySGs();
        if ( !arrMeshes[ u ]->HasVTFaces() && arrMeshes[ 1 - u ]->HasVTFaces() )
            arrMeshes[ u ]->CalcVTangents();
        if ( !arrMeshes[ u ]->HasVertexColors() && arrMeshes[ 1 - u ]->HasVertexColors() )
            arrMeshes[ u ]->SetHasVertexColors( MITrue );
    }
    MIUInt uPrimarySubMaterialCount = arrMultiMats[ 0 ].GetSubMaterials().GetCount();
    mTArray< MIUInt > arrNewMatIDs( MI_DW_INVALID, arrMultiMats[ 1 ].GetSubMaterials().GetCount() );
    for ( MIUInt u = 0, ue = arrNewMatIDs.GetCount(), uNewID = uPrimarySubMaterialCount; u != ue; ++u )
    {
        for ( MIUInt v = 0; v != uPrimarySubMaterialCount; ++v )
            if ( arrMultiMats[ 1 ].GetSubMaterials()[ u ] == arrMultiMats[ 0 ].GetSubMaterials()[ v ] )
                arrNewMatIDs[ u ] = v;
        if ( arrNewMatIDs[ u ] == MI_DW_INVALID )
            arrNewMatIDs[ u ] = uNewID++;
    }
    if ( arrNewMatIDs.GetCount() )
        for ( MIUInt u = 0, ue = arrFaceArrays[ 1 ]->GetCount(); u != ue; ++u )
            ( *arrFaceArrays[ 1 ] )[ u ].AccessMatID() = arrNewMatIDs[ ( *arrFaceArrays[ 1 ] )[ u ].GetMatID() ];
    ShiftFaces( a_meshOther.m_arrFaces, GetNumVerts() );
    ShiftFaces( a_meshOther.m_arrTextureVertexFaces, GetNumTVerts() );
    ShiftFaces( a_meshOther.m_arrVertexNormalFaces, GetNumVNormals() );
    ShiftFaces( a_meshOther.m_arrVertexTangentFaces, GetNumVTangents() );
    m_arrVertices.Add( a_meshOther.m_arrVertices );
    m_arrTextureVertices.Add( a_meshOther.m_arrTextureVertices );
    m_arrVertexNormals.Add( a_meshOther.m_arrVertexNormals );
    m_arrVertexTangents.Add( a_meshOther.m_arrVertexTangents );
    m_arrVTHandiness.Add( a_meshOther.m_arrVTHandiness );
    m_arrFaces.Add( a_meshOther.m_arrFaces );
    m_arrTextureVertexFaces.Add( a_meshOther.m_arrTextureVertexFaces );
    m_arrVertexNormalFaces.Add( a_meshOther.m_arrVertexNormalFaces );
    m_arrVertexTangentFaces.Add( a_meshOther.m_arrVertexTangentFaces );
    m_arrVertexColors.Add( a_meshOther.m_arrVertexColors );
    if ( !a_pComposedMaterialDest )
        return;
    a_pComposedMaterialDest->AccessSubMaterials().Swap( arrMultiMats[ 0 ].AccessSubMaterials() );
    for ( MIUInt u = 0, ue = arrNewMatIDs.GetCount(); u != ue; ++u )
        if ( arrNewMatIDs[ u ] >= uPrimarySubMaterialCount )
            a_pComposedMaterialDest->AccessSubMaterials().Add( arrMultiMats[ 1 ].GetSubMaterials()[ u ] );
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
    m_arrVTHandiness.Resize( a_uCount );
    m_arrVertexTangentFaces.Resize( a_uCount ? GetNumFaces() : 0 );
}

namespace
{
    mCMaxFace const *& AccessStaticFacesPointer( void )
    {
        static mCMaxFace const * s_pFaces = 0;
        return s_pFaces;
    }

    MIBool CompareFacePositionsByMatID( MIUInt a_uPos1, MIUInt a_uPos2 )
    {
        static mCMaxFace const *& s_pFaces = AccessStaticFacesPointer();
        return ( s_pFaces[ a_uPos1 ].GetMatID() < s_pFaces[ a_uPos2 ].GetMatID() );
    };
};

void mCMesh::SortFacesByMatID( void )
{
    MIUInt const uFaceCount = GetNumFaces();
    mTArray< MIUInt > arrFaceIndices;
    arrFaceIndices.Resize( uFaceCount );
    for ( MIUInt u = uFaceCount; u--; arrFaceIndices[ u ] = u );
    AccessStaticFacesPointer() = GetFaces();
    std::sort( &arrFaceIndices.Front(), &arrFaceIndices.Back() + 1, &CompareFacePositionsByMatID );
    g_reorder( AccessFaces(), arrFaceIndices.GetBuffer(), uFaceCount );
    if ( HasTVFaces() )
        g_reorder( AccessTVFaces(), arrFaceIndices.GetBuffer(), uFaceCount );
    if ( HasVNFaces() )
        g_reorder( AccessVNFaces(), arrFaceIndices.GetBuffer(), uFaceCount );
    if ( HasVTFaces() )
        g_reorder( AccessVTFaces(), arrFaceIndices.GetBuffer(), uFaceCount );
}

void mCMesh::Swap( mCMesh & a_meshOther )
{
    if ( this == &a_meshOther )
        return;
    m_arrVertices.Swap( a_meshOther.m_arrVertices );
    m_arrTextureVertices.Swap( a_meshOther.m_arrTextureVertices );
    m_arrVertexNormals.Swap( a_meshOther.m_arrVertexNormals );
    m_arrVertexTangents.Swap( a_meshOther.m_arrVertexTangents );
    m_arrVTHandiness.Swap( a_meshOther.m_arrVTHandiness );
    m_arrFaces.Swap( a_meshOther.m_arrFaces );
    m_arrTextureVertexFaces.Swap( a_meshOther.m_arrTextureVertexFaces );
    m_arrVertexNormalFaces.Swap( a_meshOther.m_arrVertexNormalFaces );
    m_arrVertexTangentFaces.Swap( a_meshOther.m_arrVertexTangentFaces );
    m_arrVertexColors.Swap( a_meshOther.m_arrVertexColors );
}

void mCMesh::WeldVertices( void )
{
    struct SVertexWithColor
    {
        mCVec3  m_vec3Position;
        mCColor m_colColor;
        MIBool operator == ( SVertexWithColor const & a_Other ) const { return m_vec3Position == a_Other.m_vec3Position && m_colColor == a_Other.m_colColor; }
    } Vert;
    mTArray< MIUInt > arrNewVertexIndices;
    mTMap< SVertexWithColor, MIUInt > mapVertexIndices;
    arrNewVertexIndices.Resize( GetNumVerts() );
    mapVertexIndices.Reserve( GetNumVerts() );
    for ( MIUInt u = 0, ue = GetNumVerts(), v, * pNewIndex; u != ue; ++u )
    {
        Vert.m_vec3Position = m_arrVertices[ u ];
        if ( HasVertexColors() )
            Vert.m_colColor = m_arrVertexColors[ u ];
        if ( v = mapVertexIndices.GetCount(), pNewIndex = &mapVertexIndices[ Vert ], v != mapVertexIndices.GetCount() )
            arrNewVertexIndices[ u ] = *pNewIndex = v;
        else
            arrNewVertexIndices[ u ] = *pNewIndex;
    }
    mTArray< mCVec3 > arrVertices( mCVec3(), mapVertexIndices.GetCount() );
    mTArray< mCColor > arrVertexColors( mCColor(), HasVertexColors() ? mapVertexIndices.GetCount() : 0 );
    for ( MIUInt u = 0, ue = GetNumVerts(); u != ue; ++u )
        arrVertices[ arrNewVertexIndices[ u ] ] = m_arrVertices[ u ];
    if ( HasVertexColors() )
        for ( MIUInt u = 0, ue = GetNumVerts(); u != ue; ++u )
            arrVertexColors[ arrNewVertexIndices[ u ] ] = m_arrVertexColors[ u ];
    for ( MIUInt u = GetNumFaces(); u--; )
        for ( MIUInt v = 3; v--; m_arrFaces[ u ][ v ] = arrNewVertexIndices[ m_arrFaces[ u ][ v ] ] );
    m_arrVertices.Swap( arrVertices );
    m_arrVertexColors.Swap( arrVertexColors );
}

void mCMesh::CalcIndicesPerVert( mTArray< MIUInt > & a_arrFirstIndexPerVert, mTArray< MIUInt > & a_arrNextIndexPerIndex ) const
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
    mTArray< SFaceAngles > arrFaceAnglesPerFace( SFaceAngles(), uFaceCount );
    mTArray< mCVec3 >      arrFaceNormals( mCVec3(), uFaceCount );

    for ( MIUInt u = uFaceCount; u--; arrFaceNormals[ u ] = m_arrFaces[ u ].CalcNormal( m_arrVertices.GetBuffer(), m_arrVertices.GetCount(), arrFaceAnglesPerFace[ u ].m_arrFaceAngles ) );
    CalcIndicesPerVert( arrFirstIndexPerVert, arrNextIndexPerIndex );
    m_arrVertexNormalFaces.Resize( uFaceCount );

    for ( MIUInt u = GetNumVerts(); u--; )
    {
        for ( MIUInt v = arrFirstIndexPerVert[ u ]; v != uIndexCount; v = arrNextIndexPerIndex[ v ] )
        {
            MIUInt const uFaceIndex = v / 3;
            MIUInt const uComposedNormalIndex = arrVertexNormals.GetCount();
            mCVec3 & vecComposedNormal = arrVertexNormals.AddNew();
            for ( MIUInt w = v, * pLast = &arrNextIndexPerIndex[ v ]; w != uIndexCount; w = *pLast )
            {
                MIUInt const uFaceIndex2 = w / 3;
                if ( w != v )
                {
                    if ( a_bUseAnglesNotSGs )
                    {
                        if ( arrFaceNormals[ uFaceIndex ].CalcDotProduct( arrFaceNormals[ uFaceIndex2 ] ) < a_fMinCosine )
                        {
                            pLast = &arrNextIndexPerIndex[ w ];
                            continue;
                        }
                    }
                    else if ( !( m_arrFaces[ uFaceIndex ].GetSGs() & m_arrFaces[ uFaceIndex2 ].GetSGs() ) )
                    {
                        pLast = &arrNextIndexPerIndex[ w ];
                        continue;
                    }
                    *pLast = arrNextIndexPerIndex[ w ];
                }
                vecComposedNormal += arrFaceNormals[ uFaceIndex2 ] * arrFaceAnglesPerFace[ uFaceIndex2 ].m_arrFaceAngles[ w % 3 ];
                m_arrVertexNormalFaces[ uFaceIndex2 ][ w % 3 ] = uComposedNormalIndex;
            }
        }
    }

    for ( MIUInt u = arrVertexNormals.GetCount(); u--; arrVertexNormals[ u ].Normalize() );
    arrVertexNormals.UnReserve();
    m_arrVertexNormals.Swap( arrVertexNormals );
}

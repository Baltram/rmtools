#include "mi_include_converters.h"
#include "mi_include_standard.h"

mCObjWriter::SOptions::SOptions( void ) :
    m_bWriteMtlFile( MIFalse )
{ 
}

namespace
{
    mCString GetCoherentName( mCString a_strName )
    {
        static MIUInt uId = 0;
        a_strName.Replace( ' ', '_' );
        a_strName.Replace( '\t', '_' );
        if ( a_strName == "" )
            a_strName.Format( "object_%u", uId++ );
        return a_strName;
    }

    MILPCChar EndLine( void )
    {
        return "\r\n";
    }

    void WriteMaterial( mCMaterial const & a_mtlSource, mCIOStreamBinary & a_streamDest )
    {
        a_streamDest << "newmtl " << GetCoherentName( a_mtlSource.GetName() ) << EndLine();
        if ( a_mtlSource.GetTexMap( mCMaterial::EMapType_Diffuse ) )
            a_streamDest << "map_Kd " << g_GetFileName( a_mtlSource.GetTexMap( mCMaterial::EMapType_Diffuse )->GetTextureFilePath() ) << EndLine();
        if ( a_mtlSource.GetTexMap( mCMaterial::EMapType_Normal ) )
            a_streamDest << "map_bump " << g_GetFileName( a_mtlSource.GetTexMap( mCMaterial::EMapType_Normal )->GetTextureFilePath() ) << EndLine();
        if ( a_mtlSource.GetTexMap( mCMaterial::EMapType_Specular ) )
            a_streamDest << "map_Ks " << g_GetFileName( a_mtlSource.GetTexMap( mCMaterial::EMapType_Specular )->GetTextureFilePath() ) << EndLine();
        a_streamDest << EndLine();
    }
}

mEResult mCObjWriter::WriteObjFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    mCStringStream streamDest;
    if ( a_Options.m_bWriteMtlFile )
    {
        mCFileStream streamMtl;
        if ( streamMtl.Open( a_Options.m_strMtlFilePath, mEFileOpenMode_Write ) == mEResult_False )
            return mEResult_False;
        for ( MIUInt u = 0, ue = a_sceneSource.GetNumMaterials(); u != ue; ++u )
        {
            if ( dynamic_cast< mCMaterial const * >( a_sceneSource.GetMaterialAt( u ) ) )
            {
                WriteMaterial( *dynamic_cast< mCMaterial const * >( a_sceneSource.GetMaterialAt( u ) ), streamMtl );
                continue;
            }
            mCMultiMaterial const * pMultiMat = dynamic_cast< mCMultiMaterial const * >( a_sceneSource.GetMaterialAt( u ) );
            for ( MIUInt v = 0, ve = pMultiMat->GetSubMaterials().GetCount(); v != ve; ++v )
                WriteMaterial( pMultiMat->GetSubMaterials()[ v ], streamMtl );
        }
        streamMtl.Close();
        streamDest << "mtllib " << g_GetFileName( a_Options.m_strMtlFilePath ) << EndLine() << EndLine();
    }

    MIUInt uVertCountTotal = 1;
    MIUInt uTVertCountTotal = 1;
    MIUInt uVNormalCountTotal = 1;
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumNodes(); u != ue; ++u )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( u );
        if ( !nodeSource.HasMesh() )
            continue;
        streamDest << "o " << GetCoherentName( nodeSource.GetName() ) << EndLine();
        streamDest << EndLine();


        mCMesh meshSource = *nodeSource.GetMesh();
        MIBool bHasVNormals = meshSource.GetNumVNormals() != 0;
        MIBool bHasTVerts = meshSource.GetNumTVerts() != 0;
        if ( ( a_Options.m_bRecalculateVertexNormals ) || ( !a_Options.m_bDropVertexNormals && !bHasVNormals ) )
        {
            if ( a_Options.m_bUseAnglesInsteadOfSGs )
                meshSource.CalcVNormalsByAngle( a_Options.m_fMaxAngleInDegrees );
            else
                meshSource.CalcVNormalsBySGs();
            bHasVNormals = MITrue;
        }
        meshSource.SortFacesByMatID();
        mCMaxToObjCoordShifter::GetInstance().ShiftMeshCoords( meshSource );
        
        mCVec3 const * pVert = meshSource.GetVerts();
        mCMaxFace const * pFace = meshSource.GetFaces();
        mCVec3 const * pTVert = meshSource.GetTVerts();
        mCFace const * pTVFace = meshSource.GetTVFaces();
        mCVec3 const * pVNormal = meshSource.GetVNormals();
        mCFace const * pVNFace = meshSource.GetVNFaces();

        for ( MIUInt u = meshSource.GetNumVerts(); u--; ++pVert )
            streamDest << mCString().Format( "v %f %f %f", pVert->GetX(), pVert->GetY(), pVert->GetZ() ) << EndLine();
        streamDest << EndLine();

        if ( bHasTVerts )
        {
            for ( MIUInt u = meshSource.GetNumTVerts(); u--; ++pTVert )
                streamDest << mCString().Format( "vt %f %f %f", pTVert->GetX(), pTVert->GetY(), pTVert->GetZ() ) << EndLine();
            streamDest << EndLine();
        }

        if ( bHasVNormals )
        {
            for ( MIUInt u = meshSource.GetNumVNormals(); u--; ++pVNormal )
                streamDest << mCString().Format( "vn %f %f %f", pVNormal->GetX(), pVNormal->GetY(), pVNormal->GetZ() ) << EndLine();
            streamDest << EndLine();
        }

        mTArray< mCString > arrSubMaterialNames;
        if ( a_Options.m_bWriteMtlFile )
        {
            MIUInt uMaterialIndex = a_sceneSource.GetMaterialIndexByName( nodeSource.GetMaterialName() );
            if ( uMaterialIndex != MI_DW_INVALID )
            {
                mCMaterialBase const * pMaterial = a_sceneSource.GetMaterialAt( uMaterialIndex );
                mCMaterial const * pStdMat = dynamic_cast< mCMaterial const * >( pMaterial );
                mCMultiMaterial const * pMultiMat = dynamic_cast< mCMultiMaterial const * >( pMaterial );
                if ( pStdMat )
                    arrSubMaterialNames.Add( GetCoherentName( pStdMat->GetName() ) );
                else
                    for ( MIUInt u = 0, ue = pMultiMat->GetSubMaterials().GetCount(); u != ue; ++u )
                        arrSubMaterialNames.Add( GetCoherentName( pMultiMat->GetSubMaterials()[ u ].GetName() ) );
            }
        }
        for ( MIUInt u = 0, ue = meshSource.GetNumFaces(), uMatID = MI_DW_INVALID; u != ue; ++u )
        {
            if ( arrSubMaterialNames.GetCount() && ( uMatID != pFace->GetMatID() ) )
            {
                uMatID = pFace->GetMatID();
                streamDest << "usemtl " << arrSubMaterialNames[ uMatID % arrSubMaterialNames.GetCount() ] << EndLine();
            }
            streamDest << "f";
            for ( MIUInt v = 0; v != 3; ++v )
            {
                streamDest << " " << ( ( *pFace )[ v ] + uVertCountTotal );
                if ( bHasVNormals || bHasTVerts )
                    streamDest << "/";
                if ( bHasTVerts )
                    streamDest << ( ( *pTVFace )[ v ] + uTVertCountTotal );
                if ( bHasVNormals )
                    streamDest << "/" << ( ( *pVNFace )[ v ] + uVNormalCountTotal );
            }
            streamDest << EndLine();
            ++pFace; ++pTVFace; ++pVNFace;
        }
        streamDest << EndLine();

        uVertCountTotal += meshSource.GetNumVerts();
        uTVertCountTotal += meshSource.GetNumTVerts();
        uVNormalCountTotal += meshSource.GetNumVNormals();
    }

    a_streamDest << streamDest;
    return mEResult_Ok;
}

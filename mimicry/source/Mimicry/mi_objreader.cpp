#include "mi_include_converters.h"
#include "mi_include_map.h"

namespace
{
    void ReadMtlFile( mCScene & a_sceneDest, mCString const & a_strFilePath )
    {
        mCFileStream streamSourceBin;
        if ( streamSourceBin.Open( a_strFilePath, mEFileOpenMode_Read ) == mEResult_False )
            return;
        mCStringStream streamSource;
        streamSource << streamSourceBin;
        streamSource.Seek( 0 );

        mCMultiMaterial & matMultiDest = *dynamic_cast< mCMultiMaterial * >( a_sceneDest.AccessMaterialAt( 0 ) );
        matMultiDest.SetName( g_GetFileNameNoExt( a_strFilePath ) );

        MILPChar pcBuffer = mCString::AccessStaticBuffer();
        MIUInt uOffsetEnd = streamSource.GetSize();
        while ( streamSource.Tell() != uOffsetEnd )
        {
            mCString strLine = streamSource.ReadLine();
            if ( strLine.Scan( "%s", pcBuffer ) != 1 )
                continue;
            mCString strToken = pcBuffer;
            if ( strToken == "" )
                continue;
            strLine.TrimLeft( strToken.GetLength() );
            strLine.TrimLeft( " \t" );
            if ( !strLine.TrimRight( " \t" ).GetLength() )
                continue;
            strToken.ToLower();
            if ( strToken == "newmtl" )
            {
                matMultiDest.AccessSubMaterials().AddNew().SetName( strLine );
            }
            if ( matMultiDest.GetSubMaterials().GetCount() == 0 )
                continue;
            mCMaterial & matDest = matMultiDest.AccessSubMaterials().Back();
            if ( strToken == "map_kd" )
            {
                mCTexMap tmapDiffuse( "", strLine );
                matDest.SetTextureMapAt( mCMaterial::EMapType_Diffuse, &tmapDiffuse );
            }
            else if ( ( strToken == "map_bump" ) || ( strToken == "bump" ) )
            {
                mCTexMap tmapBump( "", strLine );
                matDest.SetTextureMapAt( mCMaterial::EMapType_Normal, &tmapBump );
            }
            else if ( strToken == "map_ks" )
            {
                mCTexMap tmapSpecular( "", strLine );
                matDest.SetTextureMapAt( mCMaterial::EMapType_Specular, &tmapSpecular );
            }
        }
    }
}

mEResult mCObjReader::ReadObjFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, MILPCChar a_pcFolderPath )
{
    a_sceneDest.Clear();
    mCString strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileName( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() : "";
    mCMultiMaterial & matMultiDest = a_sceneDest.AddNewMultiMaterial();
    matMultiDest.SetName( "mtl_default" );

    mCStringStream streamSource;
    streamSource << a_streamSource;
    streamSource.Seek( 0 );

    MIUInt uVertCountTotal = 1;
    MIUInt uTVertCountTotal = 1;
    MIUInt uVNormalCountTotal = 1;
    MIUInt uCurrentMatID = 0;
    mCNode * pCurrentNode = &a_sceneDest.AddNewNode();
    pCurrentNode->AccessName() = "obj_default";
    mTArray< mCVec3 > arrVerts;
    mTArray< mCMaxFace > arrFaces;
    mTArray< mCVec3 > arrTVerts;
    mTArray< mCFace > arrTVFaces;
    mTArray< mCVec3 > arrVNormals;
    mTArray< mCFace > arrVNFaces;

    MILPChar pcBuffer = mCString::AccessStaticBuffer();
    MIUInt uOffsetEnd = streamSource.GetSize();
    while ( streamSource.Tell() != uOffsetEnd )
    {
        mCString strLine = streamSource.ReadLine();
        if ( strLine.Scan( "%s", pcBuffer ) != 1 )
            *pcBuffer = 0;
        mCString strToken = pcBuffer;
        strToken.ToLower();
        strLine.TrimLeft( strToken.GetLength() + 1 );
        if ( strToken == "f" )
        {
            MIUInt uSlashCount;
            strLine.Replace( '/', ' ', uSlashCount );
            MIUInt arrIndices[ 18 ];
            MIUInt uIndexCount = strLine.Scan( " %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u", arrIndices, arrIndices + 1, arrIndices + 2,
                                               arrIndices + 3, arrIndices + 4, arrIndices + 5, arrIndices + 6, arrIndices + 7, arrIndices + 8,
                                               arrIndices + 9, arrIndices + 10, arrIndices + 11, arrIndices + 12, arrIndices + 13, arrIndices + 14,
                                               arrIndices + 15, arrIndices + 16, arrIndices + 17 );
            MIUInt uRatio_2 = uSlashCount ? ( 2 * uIndexCount ) / uSlashCount : 0;
            MIUInt uHasTIndices = ( uRatio_2 / 3 ), uHasNIndices = ( uRatio_2 / 2 % 2 );
            MIUInt uPackSize = 1 + uHasTIndices + uHasNIndices;
            mCMaxFace faceDest( arrIndices[ 0 ] - uVertCountTotal, 0, 0, uCurrentMatID );
            mCFace faceTDest( arrIndices[ 1 ] - uTVertCountTotal, 0, 0 );
            mCFace faceNDest( arrIndices[ 1 + uHasTIndices ] - uVNormalCountTotal, 0, 0 );
            for ( MIUInt u = uPackSize, u2 = u + uPackSize, ue = uIndexCount - uPackSize; u != ue; u = u2, u2 += uPackSize )
            {
                faceDest[ 1 ] = arrIndices[ u ] - uVertCountTotal;
                faceDest[ 2 ] = arrIndices[ u2 ] - uVertCountTotal;
                arrFaces.Add( faceDest );
                if ( uHasTIndices )
                {
                    faceTDest[ 1 ] = arrIndices[ 1 + u ] - uTVertCountTotal;
                    faceTDest[ 2 ] = arrIndices[ 1 + u2 ] - uTVertCountTotal;
                    arrTVFaces.Add( faceTDest );
                }
                if ( uHasNIndices )
                {
                    faceNDest[ 1 ] = arrIndices[ uHasTIndices + 1 + u ] - uVNormalCountTotal;
                    faceNDest[ 2 ] = arrIndices[ uHasTIndices + 1 + u2 ] - uVNormalCountTotal;
                    arrVNFaces.Add( faceNDest );
                }
            }
        }
        else if ( strToken == "v" )
        {
            mCVec3 & vecDest = arrVerts.AddNew();
            strLine.Scan( " %f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "vt" )
        {
            mCVec3 & vecDest = arrTVerts.AddNew();
            strLine.Scan( " %f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "vn" )
        {
            mCVec3 & vecDest = arrVNormals.AddNew();
            strLine.Scan( " %f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "usemtl" )
        {
            strLine.TrimLeft( " \t" ).TrimRight( " \t" );
            uCurrentMatID = 0;
            for ( MIUInt ue = matMultiDest.GetSubMaterials().GetCount(); uCurrentMatID != ue; ++uCurrentMatID )
                if ( matMultiDest.GetSubMaterials()[ uCurrentMatID ].GetName() == strLine )
                    break;
            if ( uCurrentMatID == matMultiDest.GetSubMaterials().GetCount() )
                matMultiDest.AccessSubMaterials().AddNew().SetName( strLine );
        }
        else if ( ( strToken == "mtllib" ) && ( matMultiDest.GetSubMaterials().GetCount() == 0 ) && g_strlen( a_pcFolderPath ) )
        {
            mCString strMtlPath = a_pcFolderPath;
            strMtlPath.TrimRight( "/\\" );
            strMtlPath += "\\";
            strMtlPath += strLine.TrimLeft( " \t" ).TrimRight( " \t" );
            ReadMtlFile( a_sceneDest, strMtlPath );
        }
        if ( ( strToken == "o" ) || ( streamSource.Tell() == uOffsetEnd ) )
        {
            mCMesh meshDest;
            if ( arrVerts.GetCount() )
            {
                uVertCountTotal += arrVerts.GetCount();
                meshDest.SetNumVerts( arrVerts.GetCount() );
                g_memcpy( meshDest.AccessVerts(), arrVerts.AccessBuffer(), sizeof( mCVec3 ) * arrVerts.GetCount() );
                arrVerts.Clear();
                meshDest.SetNumFaces( arrFaces.GetCount() );
                g_memcpy( meshDest.AccessFaces(), arrFaces.AccessBuffer(), sizeof( mCMaxFace ) * arrFaces.GetCount() );
                arrFaces.Clear();

                if ( arrTVFaces.GetCount() && arrTVerts.GetCount() )
                {
                    uTVertCountTotal += arrTVerts.GetCount();
                    meshDest.SetNumTVerts( arrTVerts.GetCount() );
                    g_memcpy( meshDest.AccessTVerts(), arrTVerts.AccessBuffer(), sizeof( mCVec3 ) * arrTVerts.GetCount() );
                    arrTVerts.Clear();
                    g_memcpy( meshDest.AccessTVFaces(), arrTVFaces.AccessBuffer(), sizeof( mCFace ) * arrTVFaces.GetCount() );
                    arrTVFaces.Clear();
                }

                if ( arrVNFaces.GetCount() && arrVNormals.GetCount() )
                {
                    uVNormalCountTotal += arrVNormals.GetCount();
                    meshDest.SetNumVNormals( arrVNormals.GetCount() );
                    g_memcpy( meshDest.AccessVNormals(), arrVNormals.AccessBuffer(), sizeof( mCVec3 ) * arrVNormals.GetCount() );
                    arrVNormals.Clear();
                    g_memcpy( meshDest.AccessVNFaces(), arrVNFaces.AccessBuffer(), sizeof( mCFace ) * arrVNFaces.GetCount() );
                    arrVNFaces.Clear();
                }

                mCObjToMaxCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
                pCurrentNode->SwapMesh( meshDest );
            }
            if ( strToken == "o" )
            {
                if ( pCurrentNode->HasMesh() || ( pCurrentNode->AccessName() != "obj_default" ) )
                    pCurrentNode = &a_sceneDest.AddNewNode();
                pCurrentNode->AccessName() = strLine.TrimLeft( " \t" ).TrimRight( " \t" );
            }
        }
    }
    if ( matMultiDest.GetSubMaterials().GetCount() == 0 )
        matMultiDest.AccessSubMaterials().AddNew().SetName( "mtl_sub_default" );
    for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; a_sceneDest.AccessNodeAt( u )->AccessMaterialName() = matMultiDest.GetName() );
    a_sceneDest.SetName( strSceneName );

    return mEResult_Ok;
}

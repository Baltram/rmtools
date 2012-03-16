#include "mi_include_converters.h"
#include "mi_include_map.h"

namespace
{
    void ReadMtlFile( mCScene & a_sceneDest, mCString a_strFilePath )
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
    mCMultiMaterial matMultiDest = a_sceneDest.AddNewMultiMaterial();
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
        strLine.TrimLeft( strToken.GetLength() );
        strLine.TrimLeft( " \t" );
        strLine.TrimRight( " \t" );
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
                pCurrentNode->AccessName() = strLine;
            }
        }

        if ( ( strToken == "" ) || ( strLine.GetLength() == 0 ) )
            continue;
        else if ( strToken == "f" )
        {
            mCMaxFace & faceDest = arrFaces.AddNew();
            faceDest.AccessMatID() = uCurrentMatID;
            mCFace faceTDest, faceNDest;
            MIBool bHasTIndex = MIFalse, bHasNIndex = MIFalse;
            MILPCChar pcNumbers = "0123456789";
            for ( MIUInt u = 0; u != 3; ++u )
            {
                strLine.TrimLeft( " \t" );
                strLine.Scan( "%u", &( faceDest[ u ] ) );
                strLine.TrimLeft( pcNumbers );
                if ( strLine[ 0 ] == '/' )
                {
                    strLine.TrimLeft( ( MIUInt ) 1 );
                    if ( strLine[ 0 ] != '/' )
                    {
                        bHasTIndex = ( strLine.Scan( "%u", &( faceTDest[ u ] ) ) == 1 );
                        strLine.TrimLeft( pcNumbers );
                    }
                }
                if ( strLine[ 0 ] == '/' )
                {
                    strLine.TrimLeft( ( MIUInt ) 1 );
                    bHasNIndex = ( strLine.Scan( "%u", &( faceNDest[ u ] ) ) == 1 );
                    strLine.TrimLeft( pcNumbers );
                }
            }
            faceDest[ 0 ] -= uVertCountTotal;
            faceDest[ 1 ] -= uVertCountTotal;
            faceDest[ 2 ] -= uVertCountTotal;
            if ( bHasTIndex )
            {
                faceTDest[ 0 ] -= uTVertCountTotal;
                faceTDest[ 1 ] -= uTVertCountTotal;
                faceTDest[ 2 ] -= uTVertCountTotal;
                arrTVFaces.Add( faceTDest );
            }
            if ( bHasNIndex )
            {
                faceNDest[ 0 ] -= uVNormalCountTotal;
                faceNDest[ 1 ] -= uVNormalCountTotal;
                faceNDest[ 2 ] -= uVNormalCountTotal;
                arrVNFaces.Add( faceNDest );
            }
        }
        else if ( strToken == "v" )
        {
            mCVec3 & vecDest = arrVerts.AddNew();
            strLine.Scan( "%f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "vt" )
        {
            mCVec3 & vecDest = arrTVerts.AddNew();
            strLine.Scan( "%f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "vn" )
        {
            mCVec3 & vecDest = arrVNormals.AddNew();
            strLine.Scan( "%f %f %f", &vecDest.AccessX(), &vecDest.AccessY(), &vecDest.AccessZ() );
        }
        else if ( strToken == "usemtl" )
        {
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
            strMtlPath += strLine;
            ReadMtlFile( a_sceneDest, strMtlPath );
        }
    }
    if ( matMultiDest.GetSubMaterials().GetCount() == 0 )
        matMultiDest.AccessSubMaterials().AddNew().SetName( "mtl_sub_default" );

    return mEResult_Ok;
}
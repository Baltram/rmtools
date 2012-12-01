#include "mi_include_converters.h"
#include "mi_include_map.h"

namespace
{
    mCStringStream * AccessStream( mCStringStream * a_pStream = 0 )
    {
        static mCStringStream * s_pStream = 0;
        if ( a_pStream )
            s_pStream = a_pStream;
        return s_pStream;
    }

    MIBool & AccessIntegrityState( void )
    {
        static MIBool s_bState = MITrue;
        return s_bState;
    }

    MIInt GetNumArgs( MILPCChar a_pcFormat )
    {
        MIInt iResult = 0;
        while ( *a_pcFormat )
        {
            if ( *a_pcFormat == '%' )
                if ( ( *++a_pcFormat != '%' ) && ( *a_pcFormat != '*' ) )
                    ++iResult;
            ++a_pcFormat;
        }
        return iResult;
    }

    MIBool ReadTokenLine( mCString const & a_strToken, MILPCChar a_pcFormat, ... )
    {
        va_list argp;
        va_start( argp, a_pcFormat );
        if ( !AccessIntegrityState() )
            return MIFalse;
        MIBool bResult = MIFalse;
        mCError const * const pLastError = mCError::GetLastError< mCError >();
        MIUInt const uOffset = AccessStream()->Tell();
        mCString strSource;
        for ( ; ; )
        {
            strSource = AccessStream()->ReadLine();
            if ( ( strSource.Contains( '}' ) && !a_strToken.Contains( '}' ) ) ||
                 ( mCError::GetLastError< mCError >() != pLastError ) )
                break;
            if ( strSource.Contains( '{' ) )
            {
                if ( !mCString( a_pcFormat ).Contains( '{' ) )
                {
                    if ( !ReadTokenLine( "}", "" ) )
                        break;
                    continue;
                }
            }
            else if ( mCString( a_pcFormat ).Contains( '{' ) )
                continue;
            strSource.TrimLeft( " \t*" );
            if ( ( a_strToken[ 0 ] == 0 ) || ( strSource.FirstOf( a_strToken.GetText() ) == 0 ) )
            {
                if ( GetNumArgs( a_pcFormat ) == strSource.VScan( ( a_strToken + " " + a_pcFormat ).GetText(), argp ) )
                    bResult = MITrue;
                break;
            }
        }
        if ( !bResult )
        {
            AccessStream()->Seek( uOffset );
            AccessIntegrityState() = MIFalse;
        }
        while ( mCError::GetLastError< mCError >() != pLastError )
            mCError::ClearError( mCError::GetLastError< mCError >() );
        va_end( argp );
        return bResult;
    }

    MIBool ReadStringTokenLine( mCString const & a_strToken, mCString & a_strDest )
    {
        mCStringStream & streamSource = *AccessStream();
        MIUInt uOffset = streamSource.Tell();
        MIBool bResult = ReadTokenLine( a_strToken, "\" %s", mCString::AccessStaticBuffer() );
        MIUInt uSize = streamSource.Tell() - uOffset;
        if ( bResult )
        {
            if ( mCString::GetStaticBufferSize() < uSize )
            {
                uSize = mCString::GetStaticBufferSize();
                uOffset = streamSource.Tell() - uSize;
            }
            streamSource.Seek( uOffset );
            MILPChar const pcBuffer = mCString::AccessStaticBuffer();
            streamSource.Read( pcBuffer, uSize );
            MIUInt u, v;
            for ( u = uSize; ( u-- ) && ( pcBuffer[ u ] != '\"' ); );
            for ( v = u; ( v-- ) && ( pcBuffer[ v ] != '\"' ); );
            a_strDest.SetText( pcBuffer + v + 1, u - v - 1 );
        }
        return bResult;
    };

    MIBool EnterBlock( mCString const & a_strToken )
    {
        return ReadTokenLine( a_strToken, "{" );
    }

    MIBool EnterBlock( mCString * a_pNameDest )
    {
        MIBool bResult = ReadTokenLine( "", "%s {", mCString::AccessStaticBuffer() );
        if ( bResult )
            a_pNameDest->SetText( mCString::AccessStaticBuffer() );
        return bResult;
    }

    MIBool LeaveBlock( void )
    {
        return ReadTokenLine( "}", "" );
    }

    void ReadMaterialBlock( mCScene & a_sceneDest )
    {
        mCString strMaterialName;
        ReadStringTokenLine( "MATERIAL_NAME", strMaterialName );
        ReadTokenLine( "MATERIAL_CLASS", "\" %s", mCString::AccessStaticBuffer() );
        if ( *mCString::AccessStaticBuffer() == 'M' )
        {
            mCMultiMaterial matDest( strMaterialName );
            MIUInt uSubMaterialCount = 0;
            ReadTokenLine( "NUMSUBMTLS", "%u", &uSubMaterialCount );
            for ( MIUInt u = 0; u != uSubMaterialCount; ++u )
            {
                EnterBlock( mCString().Format( "SUBMATERIAL %u", u ) );
                ReadMaterialBlock( a_sceneDest );
                mCMaterialBase * pSubMaterial = a_sceneDest.AccessMaterialAt( a_sceneDest.GetNumMaterials() - 1 );
                if ( dynamic_cast< mCMaterial * >( pSubMaterial ) )
                    dynamic_cast< mCMaterial * >( pSubMaterial )->Swap( matDest.AccessSubMaterials().AddNew() );
                else
                    matDest.AccessSubMaterials().AddNew().SetName( pSubMaterial->GetName() );
                a_sceneDest.RemoveMaterial( pSubMaterial );
                LeaveBlock();
            }
            a_sceneDest.AddNewMultiMaterial().Swap( matDest );
        }
        else
        {
            mCMaterial matDest( strMaterialName );
            if ( AccessIntegrityState() )
            {
                for ( mCString strBlockName; EnterBlock( &strBlockName ); LeaveBlock() )
                {
                    mCMaterial::EMapType enuMapType = mCMaterial::EMapType_Normal;
                    if ( strBlockName == "MAP_DIFFUSE" )
                        enuMapType = mCMaterial::EMapType_Diffuse;
                    else if ( strBlockName == "MAP_SPECULAR" )
                        enuMapType = mCMaterial::EMapType_Specular;
                    else if ( strBlockName != "MAP_BUMP" )
                        continue;
                    mCString strName, strPath;
                    ReadStringTokenLine( "MAP_NAME", strName );
                    ReadStringTokenLine( "BITMAP", strPath );
                    mCTexMap mapDest( strName, strPath );
                    matDest.SetTextureMapAt( enuMapType, &mapDest );
                }
                AccessIntegrityState() = MITrue;
            }
            a_sceneDest.AddNewMaterial().Swap( matDest );
        }
    }

    void ReadMeshBlock( mCMesh & a_meshDest )
    {
        MIUInt uVertCount = 0;
        MIUInt uFaceCount = 0;
        ReadTokenLine( "MESH_NUMVERTEX", "%u", &uVertCount );
        ReadTokenLine( "MESH_NUMFACES", "%u", &uFaceCount );
        if ( uVertCount && EnterBlock( "MESH_VERTEX_LIST" ) )
        {
            a_meshDest.SetNumVerts( uVertCount );
            mCVec3 * pVert = a_meshDest.AccessVerts();
            for ( MIUInt u = 0; u != uVertCount; ++u, ++pVert )
                ReadTokenLine( "MESH_VERTEX", "%*u %f %f %f", &pVert->AccessX(), &pVert->AccessY(), &pVert->AccessZ() );
            LeaveBlock();
        }
        if ( uFaceCount && EnterBlock( "MESH_FACE_LIST" ) )
        {
            a_meshDest.SetNumFaces( uFaceCount );
            mCMaxFace * pFace = a_meshDest.AccessFaces();
            for ( MIUInt u = 0; u != uFaceCount; ++u, ++pFace )
            {
                mCString strLine = AccessStream()->ReadLine();
                strLine.Scan( " *MESH_FACE %*u: A: %u B: %u C: %u", &pFace->AccessA(), &pFace->AccessB(), &pFace->AccessC() );
                MIUInt uSmoothingOffset = strLine.FirstOf( "MESH_SMOOTHING" );
                if ( uSmoothingOffset != MI_DW_INVALID )
                {
                    strLine.TrimLeft( uSmoothingOffset + 14 );
                    for ( MIUInt u; ( u = atoi( strLine.TrimLeft( ", \t" ).GetText() ) ) != 0; strLine.TrimLeft( "0123456789" ) )
                        pFace->AccessSGs() |= ( 1 << ( u - 1 ) );
                    strLine.Scan( "*MESH_MTLID %u", &pFace->AccessMatID() );
                }
            }
            LeaveBlock();
        }
        if ( !AccessIntegrityState() )
            return;
        MIUInt uOffset;
        while ( AccessIntegrityState() )
        {
            uOffset = AccessStream()->Tell();
            MIUInt uCount = 0;
            ReadTokenLine( "", "%*s %u", &uCount );
            mCString strBlockName;
            EnterBlock( &strBlockName );
            if ( strBlockName == "MESH_TVERTLIST" )
            {
                a_meshDest.SetNumTVerts( uCount );
                mCVec3 * pTVert = a_meshDest.AccessTVerts();
                mCFace * pTVFace = a_meshDest.AccessTVFaces();
                for ( MIUInt u = uCount; u--; ++pTVert )
                    ReadTokenLine( "MESH_TVERT", "%*u %f %f %f", &pTVert->AccessX(), &pTVert->AccessY(), &pTVert->AccessZ() );
                LeaveBlock();
                if ( uFaceCount )
                {
                    EnterBlock( "MESH_TFACELIST" );
                    for ( MIUInt u = uFaceCount; u--; ++pTVFace )
                        ReadTokenLine( "MESH_TFACE", "%*u %u %u %u", &pTVFace->AccessA(), &pTVFace->AccessB(), &pTVFace->AccessC() );
                    LeaveBlock();
                }
            }
            else if ( strBlockName == "MESH_CVERTLIST" )
            {
                a_meshDest.SetHasVertexColors( MITrue );
                mCColor * pColors = a_meshDest.AccessVertexColors();
                mCMaxFace * pFaces = a_meshDest.AccessFaces();
                mTArray< mCVec3 > arrVertColors( mCVec3(), uCount );
                mTArray< mCFace > arrVCFaces( mCFace(), uFaceCount );
                mCVec3 * pColor = arrVertColors.AccessBuffer();
                mCFace * pVCFace = arrVCFaces.AccessBuffer();
                for ( MIUInt u = uCount; u--; ++pColor )
                    ReadTokenLine( "MESH_VERTCOL", "%*u %f %f %f", &pColor->AccessX(), &pColor->AccessY(), &pColor->AccessZ() );
                LeaveBlock();
                if ( uFaceCount )
                {
                    EnterBlock( "MESH_CFACELIST" );
                    for ( MIUInt u = uFaceCount; u--; ++pVCFace )
                        ReadTokenLine( "MESH_CFACE", "%*u %u %u %u", &pVCFace->AccessA(), &pVCFace->AccessB(), &pVCFace->AccessC() );
                    LeaveBlock();
                }
                for ( MIUInt u = uFaceCount; u--; )
                {
                    for ( MIUInt v = 3; v--; )
                    {

                        pColor = arrVertColors.AccessBuffer() + arrVCFaces[ u ][ v ];
                        mCColor * pColorDest = pColors + pFaces[ u ][ v ];
                        pColorDest->AccessRed() = static_cast< MIU8 >( pColor->GetX() * 255.0f );
                        pColorDest->AccessGreen() = static_cast< MIU8 >( pColor->GetY() * 255.0f );
                        pColorDest->AccessBlue() = static_cast< MIU8 >( pColor->GetZ() * 255.0f );
                    }
                }
            }
        }
        AccessIntegrityState() = MITrue;
        AccessStream()->Seek( uOffset );
        if ( EnterBlock( "MESH_NORMALS" ) )
        {
            a_meshDest.SetNumVNormals( uFaceCount * 3 );
            mCVec3 * pVNormal = a_meshDest.AccessVNormals();
            mCFace * pVNFace = a_meshDest.AccessVNFaces();
            for ( MIUInt u = uFaceCount, v = 0; u--; ++pVNFace )
                for ( MIUInt w = 0; w != 3; ( *pVNFace )[ w++ ] = v++ );
            for ( MIUInt u = uFaceCount; u--; )
            {
                ReadTokenLine( "MESH_FACENORMAL", "" );
                for ( MIUInt v = 3; v--; ++pVNormal )
                    ReadTokenLine( "MESH_VERTEXNORMAL", "%*u %f %f %f", &pVNormal->AccessX(), &pVNormal->AccessY(), &pVNormal->AccessZ() );
            }
            LeaveBlock();
        }
        AccessIntegrityState() = MITrue;
    }

    void ReadNodeBlock( mCScene & a_sceneDest, mTStringMap< mCString > & a_arrNodeParents )
    {
        mCNode & nodeDest = a_sceneDest.AddNewNode();
        ReadStringTokenLine( "NODE_NAME", nodeDest.AccessName() );
        if ( nodeDest.GetName().Left( 5 ).ToLower() == "bip01" )
            nodeDest.AccessIsBone() = MITrue;
        if ( AccessIntegrityState() )
        {
            ReadStringTokenLine( "NODE_PARENT", a_arrNodeParents[ nodeDest.GetName() ] );
            AccessIntegrityState() = MITrue;
        }
        if ( !EnterBlock( "NODE_TM" ) )
            return;
        mCMatrix4 & matTransform = nodeDest.AccessTransform();
        ReadTokenLine( "TM_ROW0", "%f %f %f", &matTransform.AccessElement( 0 ), &matTransform.AccessElement( 1 ), &matTransform.AccessElement( 2 ) );
        ReadTokenLine( "TM_ROW1", "%f %f %f", &matTransform.AccessElement( 4 ), &matTransform.AccessElement( 5 ), &matTransform.AccessElement( 6 ) );
        ReadTokenLine( "TM_ROW2", "%f %f %f", &matTransform.AccessElement( 8 ), &matTransform.AccessElement( 9 ), &matTransform.AccessElement( 10 ) );
        ReadTokenLine( "TM_ROW3", "%f %f %f", &matTransform.AccessElement( 12 ), &matTransform.AccessElement( 13 ), &matTransform.AccessElement( 14 ) );
        LeaveBlock();
        if ( !AccessIntegrityState() )
            return;
        else if ( !EnterBlock( "MESH" ) )
        {
            AccessIntegrityState() = MITrue;
            return;
        }
        mCMesh meshDest;
        ReadMeshBlock( meshDest );
        nodeDest.SwapMesh( meshDest );
        LeaveBlock();
        if ( AccessIntegrityState() )
        {
            MIUInt uMaterialIndex = MI_DW_INVALID;
            ReadTokenLine( "MATERIAL_REF", "%u", &uMaterialIndex );
            if ( uMaterialIndex != MI_DW_INVALID )
                nodeDest.AccessMaterialName() = a_sceneDest.GetMaterialAt( uMaterialIndex )->GetName();
            AccessIntegrityState() = MITrue;
        }
    }

    void ReadSkinBlock( mCScene & a_sceneDest )
    {
        mCStringStream & streamSource = *AccessStream();
        mCString strNodeName = streamSource.ReadLine();
        strNodeName.TrimLeft( " \t" );
        MIUInt uNodeIndex = a_sceneDest.GetNodeIndexByName( strNodeName );
        if ( uNodeIndex == MI_DW_INVALID )
        {
            AccessIntegrityState() = MIFalse;
            return;
        }
        mCNode & nodeDest = *a_sceneDest.AccessNodeAt( uNodeIndex );
        mCSkin skinDest;
        MIUInt a_uVertCount;
        mTArray< mCUnique::ID > arrBoneIDs;
        mTArray< MIUInt > arrVertexIndices;
        mTArray< MIUInt > arrBoneIndices;
        mTArray< MIFloat > arrWeights;
        MIUInt uBoneIndexIt = 0;
        mTStringMap< MIUInt > mapBoneIndexPerName;
        for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; mapBoneIndexPerName[ a_sceneDest.GetNodeAt( u )->GetName() ] = MI_DW_INVALID );
        streamSource >> a_uVertCount;
        for ( MIUInt u = 0; u != a_uVertCount; ++u )
        {
            MIUInt uNumInfluencingBones;
            streamSource >> uNumInfluencingBones;
            for ( MIUInt v = uNumInfluencingBones; v--; )
            {
                mCString strBoneName;
                MIFloat fWeight;
                streamSource.ReadStringInQuotes( strBoneName );
                streamSource >> fWeight;
                MIUInt & uBoneIndex = mapBoneIndexPerName[ strBoneName ];
                if ( uBoneIndex == MI_DW_INVALID )
                    uBoneIndex = uBoneIndexIt++;
                arrVertexIndices.Add( u );
                arrBoneIndices.Add( uBoneIndex );
                arrWeights.Add( fWeight );
            }
        }
        arrBoneIDs.Resize( uBoneIndexIt );
        for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; )
        {
            MIUInt uBoneIndex = mapBoneIndexPerName[ a_sceneDest.GetNodeAt( u )->GetName() ];
            if ( uBoneIndex != MI_DW_INVALID )
                arrBoneIDs[ uBoneIndex ] = a_sceneDest.GetNodeAt( u )->GetID();
            if ( uBoneIndex >= uBoneIndexIt )
                uBoneIndexIt = uBoneIndexIt;
        }
        if ( skinDest.InitSwapping( a_uVertCount, arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights ) == mEResult_False )
            AccessIntegrityState() = MIFalse;
        nodeDest.SwapSkin( skinDest );
    }
}

mEResult mCAseReader::ReadAseFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
{
    AccessIntegrityState() = MITrue;
    a_sceneDest.Clear();
    mTStringMap< mCString > arrNodeParents;
    mCStringStream streamSource;
    streamSource << a_streamSource;
    streamSource.Seek( 0 );
    AccessStream( &streamSource );

    MIUInt uVersion = 0;
    ReadTokenLine( "3DSMAX_ASCIIEXPORT", "%u", &uVersion );
    if ( ( uVersion < 100 ) || ( uVersion > 250 ) )
    {
        MI_ERROR( mCStreamError, EBadFormat, "Unknown .ase file version." );
        return mEResult_False;
    }

    EnterBlock( "SCENE" );
    mCString strSceneName;
    ReadStringTokenLine( "SCENE_FILENAME", strSceneName );
    LeaveBlock();
    a_sceneDest.SetName( strSceneName );

    for ( mCString strBlockName; EnterBlock( &strBlockName ); LeaveBlock() )
    {
        if ( strBlockName == "MATERIAL_LIST" )
        {
            MIUInt uMaterialCount = 0;
            ReadTokenLine( "MATERIAL_COUNT", "%u", &uMaterialCount );
            for ( MIUInt u = 0; u != uMaterialCount; ++u )
            {
                EnterBlock( mCString().Format( "MATERIAL %u", u ) );
                ReadMaterialBlock( a_sceneDest );
                LeaveBlock();
            }
        }
        else if ( ( strBlockName == "GEOMOBJECT" ) || ( strBlockName == "HELPEROBJECT" ) )
        {
            ReadNodeBlock( a_sceneDest, arrNodeParents );
        }
        else if ( strBlockName == "MESH_SOFTSKINVERTS" )
        {
            if ( a_sceneDest.GetNumMaterials() )
            {
                for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; )
                {
                    mCNode * pNode = a_sceneDest.AccessNodeAt( u );
                    if ( pNode->GetMaterialName() == "" )
                        pNode->AccessMaterialName() = a_sceneDest.GetMaterialAt( 0 )->GetName();
                }
            }
            ReadSkinBlock( a_sceneDest );
        }
    }

    if ( !AccessIntegrityState() && ( streamSource.GetSize() - streamSource.Tell() > 8 ) )
    {
        MI_ERROR( mCStreamError, EBadFormat, "Invalid .ase file." );
        return mEResult_False; 
    }
    for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; )
    {
        mCNode * pNode = a_sceneDest.AccessNodeAt( u );
        mCString const & strParentName = arrNodeParents[ pNode->GetName() ];
        MIUInt uParentIndex = ( strParentName != "" ) ? a_sceneDest.GetNodeIndexByName( strParentName ) : MI_DW_INVALID;
        if ( uParentIndex != MI_DW_INVALID )
            a_sceneDest.SetNodeParent( u, uParentIndex );
    }
    a_sceneDest.IdentifyBones();
    return mEResult_Ok;
}

#include "mi_include_converters.h"
#include "mi_include_variant.h"

mCAseWriter::SOptions::SOptions( void ) :
    m_bGothicAscFormat( MIFalse )
{ 
}

namespace
{
    mCString const & GetIndent( MIInt a_iDifference = 0 )
    {
        static mCString s_strIndent;
        if ( a_iDifference > 0 )
            s_strIndent += '\t';
        else if ( a_iDifference < 0 )
            s_strIndent.TrimRight( g_min< MIUInt >( s_strIndent[ 0 ], 1 ) );
        return s_strIndent;
    }

    mCString GetTokenLine( mCString const & a_strToken, mCString const & a_strParams )
    {
        return GetIndent() + "*" + a_strToken + " " + a_strParams + "\r\n";
    }

    mCString GetLine( mCString const a_strText )
    {
        return GetIndent() + a_strText + "\r\n";
    }

    mCString StartBlock( mCString const & a_strName )
    {
        mCString strResult = GetTokenLine( a_strName, "{" );
        GetIndent( 1 );
        return strResult;
    }

    mCString EndBlock( void )
    {
        return GetIndent( -1 ) + "}\r\n";
    }

    mCString GetSGString( MIU32 a_u32SGs )
    {
        mCString strResult;
        for( MIU32 u = 1, uIt = 1; u != 33; uIt <<= 1, ++u )
            if ( uIt & a_u32SGs )
                strResult += mCString().Format( ",%u", u );
        if ( strResult.GetLength() )
            strResult.TrimLeft( ( MIUInt ) 1 );
        return strResult;
    }

    mTStringMap< mCString > * AccessNodeNameMap( mTStringMap< mCString > * a_mapSource = 0, MIBool a_bDisable = MIFalse )
    {
        static mTStringMap< mCString > * s_pMap = 0;
        if ( a_mapSource )
            s_pMap = a_mapSource;
        if ( a_bDisable )
            s_pMap = 0;
        return s_pMap;
    }

    mCString GetNodeName( mCString a_strPlainNodeName )
    {
        mTStringMap< mCString > * pMap = AccessNodeNameMap();
        if ( !pMap )
            return a_strPlainNodeName;
        mCString strResult = ( *pMap )[ a_strPlainNodeName ];
        if ( strResult == "" )
            return a_strPlainNodeName;
        return strResult;
    }

    void WriteMap( mCTexMap const & a_mapSource, MIUInt uSubNo, mCIOStreamBinary & a_streamDest )
    {
        a_streamDest.Write( GetTokenLine( "MAP_NAME", "\""+ a_mapSource.GetName() + "\"" ) );
        a_streamDest.Write( GetTokenLine( "MAP_CLASS", "\"Bitmap\"" ) );
        a_streamDest.Write( GetTokenLine( "MAP_SUBNO", mCString().Format( "%u", uSubNo ) ) );
        a_streamDest.Write( GetTokenLine( "MAP_AMOUNT", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "BITMAP", "\"" + a_mapSource.GetTextureFilePath() + "\"" ) );
        a_streamDest.Write( GetTokenLine( "MAP_TYPE", "Screen" ) );
        a_streamDest.Write( GetTokenLine( "UVW_U_OFFSET", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_V_OFFSET", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_U_TILING", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_V_TILING", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_ANGLE", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_BLUR", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_BLUR_OFFSET", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_NOUSE_AMT", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_NOISE_SIZE", "1.0000" ) );
        a_streamDest.Write( GetTokenLine( "UVW_NOISE_LEVEL", "1" ) );
        a_streamDest.Write( GetTokenLine( "UVW_NOISE_PHASE", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "BITMAP_FILTER", "Pyramidal" ) );
    }

    void WriteMaterial( mCMaterialBase const * a_pMaterial, mCIOStreamBinary & a_streamDest )
    {
        mCMaterial const * pMaterial = dynamic_cast< mCMaterial const * >( a_pMaterial );
        mCMultiMaterial const * pMultiMaterial = dynamic_cast< mCMultiMaterial const * >( a_pMaterial );
        a_streamDest.Write( GetTokenLine( "MATERIAL_NAME", mCString().Format( "\"%s\"", a_pMaterial->GetName() ) ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_CLASS", mCString().Format( "\"%s\"", ( pMultiMaterial ? "Multi/Sub-Object" : "Standard" ) ) ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_AMBIENT", "0.5000\t0.5000\t0.5000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_DIFFUSE", "0.5000\t0.5000\t0.5000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_SPECULAR", "0.9000\t0.9000\t0.9000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_SHINE", "0.1000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_SHINESTRENGTH", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_TRANSPARENCY", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "MATERIAL_WIRESIZE", "1.0000" ) );
        if ( pMaterial )
        {
            a_streamDest.Write( GetTokenLine( "MATERIAL_SHADING", "Blinn" ) );
            a_streamDest.Write( GetTokenLine( "MATERIAL_XP_FALLOFF", "0.0000" ) );
            a_streamDest.Write( GetTokenLine( "MATERIAL_SELFILLUM", "0.0000" ) );
            a_streamDest.Write( GetTokenLine( "MATERIAL_FALLOFF", "In" ) );
            a_streamDest.Write( GetTokenLine( "MATERIAL_XP_TYPE", "Filter" ) );
            mCTexMap const * arrMaps[ 3 ];
            MILPCChar arrTokens[ 3 ] = { "MAP_DIFFUSE", "MAP_SPECULAR", "MAP_BUMP" };
            MIUInt arrSubNos[ 3 ] = { 1, 2, 8 };
            arrMaps[ 0 ] = pMaterial->GetTextureMapAt( mCMaterial::EMapType_Diffuse );
            arrMaps[ 1 ] = pMaterial->GetTextureMapAt( mCMaterial::EMapType_Specular );
            arrMaps[ 2 ] = pMaterial->GetTextureMapAt( mCMaterial::EMapType_Normal );
            for ( MIUInt u = 0; u != 3; ++u )
            {
                if ( !arrMaps[ u ] )
                    continue;
                a_streamDest.Write( StartBlock( arrTokens[ u ] ) );
                WriteMap( *arrMaps[ u ], arrSubNos[ u ], a_streamDest );
                a_streamDest.Write( EndBlock() );
            }
                
        }
        else if ( pMultiMaterial )
        {
            mTArray< mCMaterial > const & arrSubs = pMultiMaterial->GetSubMaterials();
            a_streamDest.Write( GetTokenLine( "NUMSUBMTLS", mCString().Format( "%u", arrSubs.GetCount() ) ) );
            for ( MIUInt u = 0, ue = arrSubs.GetCount(); u != ue; ++u )
            {
                a_streamDest.Write( StartBlock( mCString().Format( "SUBMATERIAL %u", u ) ) );
                WriteMaterial( &arrSubs[ u ], a_streamDest );
                a_streamDest.Write( EndBlock() );
            }
        }
    }

    MIFloat ColCompAsFloat( MIU8 a_uValue )
    {
        return static_cast< MIFloat >( a_uValue ) / 255.0f;
    }

    void WriteMesh( mCMesh const & a_meshSource, mCIOStreamBinary & a_streamDest, mCAseWriter::SOptions const & a_Options )
    {
        mCVec3 const * pVerts = a_meshSource.GetVerts();
        mCMaxFace const * pFaces = a_meshSource.GetFaces();
        MIUInt const uVertexCount = a_meshSource.GetNumVerts();
        MIUInt const uFaceCount = a_meshSource.GetNumFaces();
        a_streamDest.Write( GetTokenLine( "TIMEVALUE", "0" ) );
        a_streamDest.Write( GetTokenLine( "MESH_NUMVERTEX", mCString().Format( "%u", uVertexCount ) ) );
        a_streamDest.Write( GetTokenLine( "MESH_NUMFACES", mCString().Format( "%u", uFaceCount ) ) );
        a_streamDest.Write( StartBlock( "MESH_VERTEX_LIST" ) );
        for ( MIUInt u = 0; u != uVertexCount; ++u )
            a_streamDest.Write( GetTokenLine( "MESH_VERTEX", mCString().Format( "%u\t%.4f\t%.4f\t%.4f", u, pVerts[ u ].GetX(), pVerts[ u ].GetY(), pVerts[ u ].GetZ() ) ) );
        a_streamDest.Write( EndBlock() );
        a_streamDest.Write( StartBlock( "MESH_FACE_LIST" ) );
        for ( MIUInt u = 0; u != uFaceCount; ++u )
            a_streamDest.Write( GetTokenLine( "MESH_FACE", mCString().Format( "%u:\tA: %u\tB: %u\tC: %u\tAB: 1\tBC: 1\tCA: 0\t*MESH_SMOOTHING %s\t*MESH_MTLID %u",
                                                                              u, pFaces[ u ].GetA(), pFaces[ u ].GetB(), pFaces[ u ].GetC(), 
                                                                              GetSGString( pFaces[ u ].GetSGs() ).GetText(), pFaces[ u ].GetMatID()) ) );
        a_streamDest.Write( EndBlock() );
        if ( a_meshSource.HasTVFaces() )
        {
            mCVec3 const * pTVerts = a_meshSource.GetTVerts();
            mCFace const * pTVFaces = a_meshSource.GetTVFaces();
            MIUInt const uTVertCount = a_meshSource.GetNumTVerts();
            a_streamDest.Write( GetTokenLine( "MESH_NUMTVERTEX", mCString().Format( "%u", uTVertCount ) ) );
            a_streamDest.Write( StartBlock( "MESH_TVERTLIST" ) );
            for ( MIUInt u = 0; u != uTVertCount; ++u )
                a_streamDest.Write( GetTokenLine( "MESH_TVERT", mCString().Format( "%u\t%.4f\t%.4f\t%.4f", u, pTVerts[ u ].GetX(), pTVerts[ u ].GetY(), pTVerts[ u ].GetZ() ) ) );
            a_streamDest.Write( EndBlock() );
            a_streamDest.Write( GetTokenLine( "MESH_NUMTVFACES", mCString().Format( "%u", uFaceCount ) ) );
            a_streamDest.Write( StartBlock( "MESH_TFACELIST" ) );
            for ( MIUInt u = 0; u != uFaceCount; ++u )
                a_streamDest.Write( GetTokenLine( "MESH_TFACE", mCString().Format( "%u\t%u\t%u\t%u", u, pTVFaces[ u ].GetA(), pTVFaces[ u ].GetB(), pTVFaces[ u ].GetC() ) ) );
            a_streamDest.Write( EndBlock() );
        }
        if ( a_meshSource.HasVertexColors() && !a_Options.m_bDropVertexColors && !a_Options.m_bGothicAscFormat )
        {
            mCColor const * pVColors = a_meshSource.GetVertexColors();
            a_streamDest.Write( GetTokenLine( "MESH_NUMCVERTEX", mCString().Format( "%u", uVertexCount ) ) );
            a_streamDest.Write( StartBlock( "MESH_CVERTLIST" ) );
            for ( MIUInt u = 0; u != uVertexCount; ++u )
                a_streamDest.Write( GetTokenLine( "MESH_VERTCOL", mCString().Format( "%u\t%.4f\t%.4f\t%.4f", u, ColCompAsFloat( pVColors[ u ].GetRed() ), 
                                                                                     ColCompAsFloat( pVColors[ u ].GetGreen() ), ColCompAsFloat( pVColors[ u ].GetBlue() ) ) ) );
            a_streamDest.Write( EndBlock() );
            a_streamDest.Write( GetTokenLine( "MESH_NUMCVFACES", mCString().Format( "%u", uFaceCount ) ) );
            a_streamDest.Write( StartBlock( "MESH_CFACELIST" ) );
            for ( MIUInt u = 0; u != uFaceCount; ++u )
                a_streamDest.Write( GetTokenLine( "MESH_CFACE", mCString().Format( "%u\t%u\t%u\t%u", u, pFaces[ u ].GetA(), pFaces[ u ].GetB(), pFaces[ u ].GetC() ) ) );
            a_streamDest.Write( EndBlock() );
        }
        if ( !a_Options.m_bDropVertexNormals && !a_Options.m_bGothicAscFormat )
        {
            mCMesh meshClone;
            mCMesh const * pMesh = &a_meshSource;
            if ( !a_meshSource.HasVNFaces() || a_Options.m_bRecalculateVertexNormals )
            {
                pMesh = &meshClone;
                meshClone = a_meshSource;
                if ( a_Options.m_bUseAnglesInsteadOfSGs )
                    meshClone.CalcVNormalsByAngle( a_Options.m_fMaxAngleInDegrees );
                else
                    meshClone.CalcVNormalsBySGs();
            }
            mCVec3 const * pVNormals = pMesh->GetVNormals();
            mCFace const * pVNFaces = pMesh->GetVNFaces();
            a_streamDest.Write( StartBlock( "MESH_NORMALS" ) );
            for ( MIUInt u = 0; u != uFaceCount; ++u )
            {
                mCVec3 vecFaceNormal = pFaces[ u ].CalcNormal( pVerts, uVertexCount );
                a_streamDest.Write( GetTokenLine( "MESH_FACENORMAL", mCString().Format( "%u\t%.4f\t%.4f\t%.4f", u, vecFaceNormal.GetX(), vecFaceNormal.GetY(), vecFaceNormal.GetZ() ) ) );
                GetIndent( 1 );
                for ( MIUInt v = 0; v != 3; ++v )
                {
                    mCVec3 const & vecVNormal = pVNormals[ pVNFaces[ u ][ v ] ];
                    a_streamDest.Write( GetTokenLine( "MESH_VERTEXNORMAL", mCString().Format( "%u\t%.4f\t%.4f\t%.4f", pFaces[ u ][ v ], vecVNormal.GetX(), vecVNormal.GetY(), vecVNormal.GetZ() ) ) );
                }
                GetIndent( -1 );
            }
            a_streamDest.Write( EndBlock() );
        }
    }

    void WriteNode( mCScene const & a_sceneSource, MIUInt a_uNodeIndex, mCIOStreamBinary & a_streamDest, mCAseWriter::SOptions const & a_Options )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( a_uNodeIndex );
        MIUInt const uParentIndex = a_sceneSource.GetNodeParentIndex( a_uNodeIndex );
        mCNode const * pParent = uParentIndex != MI_DW_INVALID ? a_sceneSource.GetNodeAt( uParentIndex ) : 0;
        mCString strNodeName = GetNodeName( nodeSource.GetName() );
        mCVec3 const & vecPos = nodeSource.GetPosition();
        mCMatrix4 const & matTransform = nodeSource.GetTransform();
        mCString strRow0 = mCString().Format( "%.4f\t%.4f\t%.4f", matTransform.GetElement( 0, 0 ), matTransform.GetElement( 0, 1 ), matTransform.GetElement( 0, 2 ) );
        mCString strRow1 = mCString().Format( "%.4f\t%.4f\t%.4f", matTransform.GetElement( 1, 0 ), matTransform.GetElement( 1, 1 ), matTransform.GetElement( 1, 2 ) );
        mCString strRow2 = mCString().Format( "%.4f\t%.4f\t%.4f", matTransform.GetElement( 2, 0 ), matTransform.GetElement( 2, 1 ), matTransform.GetElement( 2, 2 ) );
        mCString strPos = mCString().Format( "%.4f\t%.4f\t%.4f", vecPos.GetX(), vecPos.GetY(), vecPos.GetZ() );
        MIUInt uMaterialIndex = a_sceneSource.GetMaterialIndexByName( nodeSource.GetMaterialName() );
        a_streamDest.Write( GetTokenLine( "NODE_NAME", "\"" + strNodeName + "\"" ) );
        if ( pParent )
            a_streamDest.Write( GetTokenLine( "NODE_PARENT", "\"" + GetNodeName( pParent->GetName() ) + "\"" ) );
        a_streamDest.Write( StartBlock( "NODE_TM" ) );
        a_streamDest.Write( GetTokenLine( "NODE_NAME", "\"" + strNodeName + "\"" ) );
        a_streamDest.Write( GetTokenLine( "INHERIT_POS", "0 0 0" ) );
        a_streamDest.Write( GetTokenLine( "INHERIT_ROT", "0 0 0" ) );
        a_streamDest.Write( GetTokenLine( "INHERIT_SCL", "0 0 0" ) );
        a_streamDest.Write( GetTokenLine( "TM_ROW0", strRow0 ) );
        a_streamDest.Write( GetTokenLine( "TM_ROW1", strRow1 ) );
        a_streamDest.Write( GetTokenLine( "TM_ROW2", strRow2 ) );
        a_streamDest.Write( GetTokenLine( "TM_ROW3", strPos ) );
        a_streamDest.Write( GetTokenLine( "TM_POS", strPos ) );
        a_streamDest.Write( GetTokenLine( "TM_ROTAXIS", "0.0000\t0.0000\t0.0000" ) );
        a_streamDest.Write( GetTokenLine( "TM_ROTANGLE", "0.0000" ) );
        a_streamDest.Write( GetTokenLine( "TM_SCALE", "1.0000\t1.0000\t1.0000" ) );
        a_streamDest.Write( GetTokenLine( "TM_SCALEAXIS", "0.0000\t0.0000\t0.0000" ) );
        a_streamDest.Write( GetTokenLine( "TM_SCALEAXISANG", "0.0000" ) );
        a_streamDest.Write( EndBlock() );
        if ( nodeSource.GetMesh() )
        {
            a_streamDest.Write( StartBlock( "MESH" ) );
            WriteMesh( *nodeSource.GetMesh(), a_streamDest, a_Options );
            a_streamDest.Write( EndBlock() );
        }
        if ( a_Options.m_bGothicAscFormat )
            return;
        a_streamDest.Write( GetTokenLine( "PROP_MOTIONBLUR", "0" ) );
        a_streamDest.Write( GetTokenLine( "PROP_CASTSHADOW", "1" ) );
        a_streamDest.Write( GetTokenLine( "PROP_RECVSHADOW", "1" ) );
        if ( uMaterialIndex != MI_DW_INVALID )
            a_streamDest.Write( GetTokenLine( "MATERIAL_REF", mCString().Format( "%u", uMaterialIndex ) ) );
    }

    void WriteSkin( mCScene const & a_sceneSource, MIUInt a_uNodeIndex, mCIOStreamBinary & a_streamDest )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( a_uNodeIndex );
        mCSkin const & skinSource = *nodeSource.GetSkin();
        MIUInt uVertCount = skinSource.GetNumVerts();
        if ( uVertCount != nodeSource.GetMesh()->GetNumVerts() )
        {
            MI_ERROR( mCConverterError, EMiscellaneous, ( "Not all vertices of the mesh \"" + nodeSource.GetName() + "\" are connected to bones." ).GetText() );
            return;
        }
        mTArray< mCString > arrBoneNames( mCString(), skinSource.GetNumBones() );
        for ( MIUInt u = skinSource.GetNumBones(); u--; )
            arrBoneNames[ u ] = GetNodeName( a_sceneSource.GetNodeAt( a_sceneSource.GetNodeIndexByID( skinSource.GetBoneIDByIndex( u ) ) )->GetName() );
        a_streamDest.Write( StartBlock( "MESH_SOFTSKINVERTS" ) );
        GetIndent( -1 );
        a_streamDest.Write( GetLine( GetNodeName( nodeSource.GetName() ) ) );
        a_streamDest.Write( GetLine( mCString().Format( "%u", uVertCount ) ) );
        for ( MIUInt u = 0; u != uVertCount; ++u )
        {
            MIUInt uBoneCount = skinSource.GetNumInfluencingBones( u );
            mCString strSkinText = mCString().Format( "%u", uBoneCount );
            for ( MIUInt v = 0; v != uBoneCount; ++v )
                strSkinText += mCString().Format( "\t\"%s\"\t%f", arrBoneNames[ skinSource.GetBoneIndex( u, v ) ], skinSource.GetWeight( u, v ) );
            a_streamDest.Write( GetLine( strSkinText ) );
        }
        GetIndent( 1 );
        a_streamDest.Write( EndBlock() );
    }
}

mEResult mCAseWriter::WriteAseFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions const a_Options )
{
    a_streamDest.Write( GetTokenLine( "3DSMAX_ASCIIEXPORT", ( a_Options.m_bGothicAscFormat ? "110" : "200" ) ) );
    a_streamDest.Write( GetTokenLine( "COMMENT", "\"Created using Baltram's ASC and ASE file writer\"" ) );
    a_streamDest.Write( StartBlock( "SCENE" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_FILENAME", "\"" + g_GetFileName( a_sceneSource.GetName() ) + "\"" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_FIRSTFRAME", "0" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_LASTFRAME", "100" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_FRAMESPEED", "30" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_TICKSPERFRAME", "160" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_BACKGROUND_STATIC", "0.0000\t0.0000\t0.0000" ) );
    a_streamDest.Write( GetTokenLine( "SCENE_AMBIENT_STATIC", "0.0000\t0.0000\t0.0000" ) );
    a_streamDest.Write( EndBlock() );
    a_streamDest.Write( StartBlock( "MATERIAL_LIST" ) );
    mTStringMap< mCString > mapFullNodeNames;
    mTArray< mCNode const * > arrNodes;
    a_sceneSource.GetNodesSortedByLinks( arrNodes );
    mCScene sceneTemp;
    mCScene const & sceneMaterials = a_Options.m_bGothicAscFormat ? sceneTemp : a_sceneSource;
    AccessNodeNameMap( 0, MITrue );
    if ( a_Options.m_bGothicAscFormat )
    {
        for ( MIUInt u = a_sceneSource.GetNumNodes(); u--; )
        {
            mCNode const & nodeNode = *a_sceneSource.GetNodeAt( u );
            MIUInt uMaterialIndex = a_sceneSource.GetMaterialIndexByName( nodeNode.GetMaterialName() );
            if ( ( uMaterialIndex != MI_DW_INVALID ) && ( sceneTemp.GetNumMaterials() == 0 ) )
                sceneTemp.AddMaterial( *a_sceneSource.GetMaterialAt( uMaterialIndex ) );
            mCString strFullNodeName = ( nodeNode.HasMesh() ? "ZM_" : "Bip01 " ) + nodeNode.GetName();
            if ( mCString( strFullNodeName ).ToLower().FirstOf( "zm_zm_" ) == 0 )
                strFullNodeName.TrimLeft( ( MIUInt ) 3 );
            if ( mCString( strFullNodeName ).ToLower().FirstOf( "bip01 bip01" ) == 0 )
                strFullNodeName.TrimLeft( ( MIUInt ) 6 );
            mapFullNodeNames[ nodeNode.GetName() ] = strFullNodeName;
        }
        AccessNodeNameMap( &mapFullNodeNames );
    }
    MIUInt const uMaterialCount = sceneMaterials.GetNumMaterials();
    a_streamDest.Write( GetTokenLine( "MATERIAL_COUNT", mCString().Format( "%u", uMaterialCount ) ) );
    for ( MIUInt u = 0; u != uMaterialCount; ++u )
    {
        a_streamDest.Write( StartBlock( mCString().Format( "MATERIAL %u", u ) ) );
        WriteMaterial( sceneMaterials.GetMaterialAt( u ), a_streamDest );
        a_streamDest.Write( EndBlock() );
    }
    a_streamDest.Write( EndBlock() );
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumNodes(); u != ue; ++u )
    {
        a_streamDest.Write( StartBlock( "GEOMOBJECT" ) );
        WriteNode( a_sceneSource, a_sceneSource.GetNodeIndexByID( arrNodes[ u ]->GetID() ), a_streamDest, a_Options );
        a_streamDest.Write( EndBlock() );
    }
    if ( a_Options.m_bGothicAscFormat )
    {
        for ( MIUInt u = 0; u != a_sceneSource.GetNumNodes(); ++u )
            if ( a_sceneSource.GetNodeAt( u )->HasSkin() && a_sceneSource.GetNodeAt( u )->HasMesh() )
                WriteSkin( a_sceneSource, u, a_streamDest );
    }
    return mEResult_Ok;
}
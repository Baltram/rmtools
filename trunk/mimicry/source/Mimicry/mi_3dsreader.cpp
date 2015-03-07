#include "mi_include_converters.h"

namespace
{
    enum EChunkID
    {
        EChunkID_NONE          = MI_DW_INVALID,
        EChunkID_M3DMAGIC      = 0x4D4D,
        EChunkID_MDATA         = 0x3D3D,
        EChunkID_MAT_ENTRY     = 0xAFFF,
        EChunkID_MAT_NAME      = 0xA000,
        EChunkID_MAT_TEXMAP    = 0xA200,
        EChunkID_MAT_BUMPMAP   = 0xA230,
        EChunkID_MAT_SPECMAP   = 0xA204,
        EChunkID_MAT_MAPNAME   = 0xA300,
        EChunkID_NAMED_OBJECT  = 0x4000,
        EChunkID_N_TRI_OBJECT  = 0x4100,
        EChunkID_POINT_ARRAY   = 0x4110,
        EChunkID_FACE_ARRAY    = 0x4120,
        EChunkID_MSH_MAT_GROUP = 0x4130,
        EChunkID_SMOOTH_GROUP  = 0x4150,
        EChunkID_TEX_VERTS     = 0x4140,
        EChunkID_MESH_MATRIX   = 0x4160
    };
}

namespace
{
    MIBool ReadChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, EChunkID a_enuParentChunk = EChunkID_NONE )
    {
        if ( a_streamSource.IsAtEnd() )
            return MIFalse;
        MIUInt uOffset = a_streamSource.Tell();
        MIUInt uChunkID = a_streamSource.ReadU16();
        MIUInt uOffsetEnd = uOffset + a_streamSource.ReadU32();
        static mTStringMap< MIUInt > s_mapMatIDs;
        static mCMultiMaterial s_matMulti;
        static mCMaterial * s_pMaterial = 0;
        static mCNode * s_pNode = 0;
        s_matMulti.AccessName() = "mats_" + a_sceneDest.GetName();
        switch ( uChunkID )
        {
        case EChunkID_M3DMAGIC:
        {
            if ( a_enuParentChunk != EChunkID_NONE )
                return MIFalse;
            s_matMulti.Clear();
            break;
        }
        case EChunkID_MDATA:
        {
            if ( a_enuParentChunk != EChunkID_M3DMAGIC )
                return MIFalse;
            break;
        }
        case EChunkID_MAT_ENTRY:
        {
            if ( a_enuParentChunk != EChunkID_MDATA )
                return MIFalse;
            s_pMaterial = &s_matMulti.AccessSubMaterials().AddNew();
            break;
        }
        case EChunkID_MAT_NAME:
        {
            if ( a_enuParentChunk != EChunkID_MAT_ENTRY )
                return MIFalse;
            if ( s_pMaterial )
            {
                s_pMaterial->AccessName() = a_streamSource.ReadString();
                s_mapMatIDs[ s_pMaterial->GetName() ] = s_matMulti.GetSubMaterials().GetCount() - 1;
            }
            break;
        }
        case EChunkID_MAT_TEXMAP:
        case EChunkID_MAT_BUMPMAP:
        case EChunkID_MAT_SPECMAP:
        {
            if ( a_enuParentChunk != EChunkID_MAT_ENTRY )
                return MIFalse;
            break;
        }
        case EChunkID_MAT_MAPNAME:
        {
            mCMaterial::EMapType enuMapType;
            if ( a_enuParentChunk == EChunkID_MAT_TEXMAP )
                enuMapType = mCMaterial::EMapType_Diffuse;
            else if ( a_enuParentChunk == EChunkID_MAT_BUMPMAP )
                enuMapType = mCMaterial::EMapType_Normal;
            else if ( a_enuParentChunk == EChunkID_MAT_SPECMAP )
                enuMapType = mCMaterial::EMapType_Specular;
            else
                return MIFalse;
            if ( s_pMaterial )
                s_pMaterial->AccessTexMap( enuMapType ).AccessTextureFilePath() = a_streamSource.ReadString();
            break;
        }
        case EChunkID_NAMED_OBJECT:
        {
            if ( a_enuParentChunk != EChunkID_MDATA )
                return MIFalse;
            s_pNode = &a_sceneDest.AddNewNode();
            s_pNode->AccessName() = a_streamSource.ReadString();
            break;
        }
        case EChunkID_N_TRI_OBJECT:
        {
            if ( a_enuParentChunk != EChunkID_NAMED_OBJECT ||
                 !s_pNode )
                return MIFalse;
            mCMesh meshEmpty;
            s_pNode->SetMesh( &meshEmpty );
            break;
        }
        case EChunkID_POINT_ARRAY:
        {
            if ( a_enuParentChunk != EChunkID_N_TRI_OBJECT ||
                 !s_pNode ||
                 !s_pNode->GetMesh() )
                return MIFalse;
            MIUInt uVertCount = a_streamSource.ReadU16();
            s_pNode->AccessMesh()->SetNumVerts( uVertCount );
            mCVec3 * pVerts = s_pNode->AccessMesh()->AccessVerts();
            for ( MIUInt u = 0; u != uVertCount; ++u )
                a_streamSource >> pVerts[ u ];
            break;
        }
        case EChunkID_FACE_ARRAY:
        {
            if ( a_enuParentChunk != EChunkID_N_TRI_OBJECT ||
                 !s_pNode ||
                 !s_pNode->GetMesh() )
                return MIFalse;
            MIUInt uFaceCount = a_streamSource.ReadU16();
            s_pNode->AccessMesh()->SetNumFaces( uFaceCount );
            mCMaxFace * pFaces = s_pNode->AccessMesh()->AccessFaces();
            for ( MIUInt u = 0; u != uFaceCount; ++u )
            {
                pFaces[ u ].AccessA() = a_streamSource.ReadU16();
                pFaces[ u ].AccessB() = a_streamSource.ReadU16();
                pFaces[ u ].AccessC() = a_streamSource.ReadU16();
                a_streamSource.ReadU16();
            }
            if ( s_pNode->GetMesh()->HasTVFaces() )
                s_pNode->AccessMesh()->CopyTVFacesFromFaces();
            break;
        }
        case EChunkID_MSH_MAT_GROUP:
        {
            if ( a_enuParentChunk != EChunkID_FACE_ARRAY ||
                 !s_pNode ||
                 !s_pNode->GetMesh() )
                return MIFalse;
            mCString strMaterialName = a_streamSource.ReadString();
            MIUInt uMatID = 0;
            if ( s_mapMatIDs.Contains( strMaterialName ) )
            {
                uMatID = s_mapMatIDs[ strMaterialName ];
                if ( a_sceneDest.GetNumMaterials() == 0 )
                    a_sceneDest.AddMaterial( s_matMulti );
                s_pNode->AccessMaterialName() = s_matMulti.GetName();
            }
            MIUInt uFaceIndexCount = a_streamSource.ReadU16();
            mCMaxFace * pFaces = s_pNode->AccessMesh()->AccessFaces();
            for ( MIUInt u = 0; u != uFaceIndexCount; ++u )
                pFaces[ a_streamSource.ReadU16() ].AccessMatID() = uMatID;
            break;
        }
        case EChunkID_SMOOTH_GROUP:
        {
            if ( a_enuParentChunk != EChunkID_FACE_ARRAY ||
                 !s_pNode ||
                 !s_pNode->GetMesh() ||
                 uOffsetEnd != uOffset + 6 + s_pNode->GetMesh()->GetNumFaces() * 4 )
                return MIFalse;
            mCMaxFace * pFaces = s_pNode->AccessMesh()->AccessFaces();
            for ( MIUInt u = 0, ue = s_pNode->GetMesh()->GetNumFaces(); u != ue; ++u )
                pFaces[ u ].AccessSGs() = a_streamSource.ReadU32();
            break;
        }
        case EChunkID_TEX_VERTS:
        {
            if ( a_enuParentChunk != EChunkID_N_TRI_OBJECT ||
                 !s_pNode ||
                 !s_pNode->GetMesh() ||
                 a_streamSource.ReadU16() != s_pNode->GetMesh()->GetNumVerts() )
                return MIFalse;
            s_pNode->AccessMesh()->SetNumTVerts( s_pNode->GetMesh()->GetNumVerts() );
            s_pNode->AccessMesh()->CopyTVFacesFromFaces();
            mCVec3 * pTVerts = s_pNode->AccessMesh()->AccessTVerts();
            for ( MIUInt u = 0, ue = s_pNode->GetMesh()->GetNumTVerts(); u != ue; ++u )
                a_streamSource >> pTVerts[ u ].AccessX() >> pTVerts[ u ].AccessY();
            break;
        }
        case EChunkID_MESH_MATRIX:
        {
            if ( a_enuParentChunk != EChunkID_N_TRI_OBJECT ||
                 !s_pNode )
                return MIFalse;
            mCMatrix4 & matTransform = s_pNode->AccessTransform();
            for ( MIUInt u = 0; u != 4; ++u )
                for ( MIUInt v = 0; v != 3; ++v )
                    a_streamSource >> matTransform.AccessRow( u )[ v ];
            break;
        }
        default:
        {
            if ( uOffsetEnd > a_streamSource.GetSize() )
                return MIFalse;
            a_streamSource.Seek( uOffsetEnd );
            break;
        }
        }
        while ( a_streamSource.Tell() < uOffsetEnd )
            if ( !ReadChunk( a_sceneDest, a_streamSource, static_cast< EChunkID >( uChunkID ) ) )
                return MIFalse;
        return MITrue;
    }
}

mEResult mC3dsReader::Read3dsFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
{
    a_sceneDest.Clear();
    mCString strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    a_sceneDest.SetName( strSceneName );
    if ( !ReadChunk( a_sceneDest, a_streamSource ) )
    {
        a_sceneDest.SetName( "" );
        MI_ERROR( mCConverterError, EBadFormat, "Invalid .3ds file." );
        return mEResult_False;
    }
    return mEResult_Ok;
}

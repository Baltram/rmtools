#include "mi_include_converters.h"
#include "mi_include_variant.h"

namespace
{
    MILPCChar const s_arrColorSrcCombinerTypeStrings[] =
    {
        "eEColorSrcCombinerType_Add",
        "eEColorSrcCombinerType_Subtract",
        "eEColorSrcCombinerType_Multiply",
        "eEColorSrcCombinerType_Max",
        "eEColorSrcCombinerType_Min"
    };

    MILPCChar const s_arrColorSrcSampleTexRepeatStrings[] =
    {
        "eEColorSrcSampleTexRepeat_Wrap",
        "eEColorSrcSampleTexRepeat_Clamp",
        "eEColorSrcSampleTexRepeat_Mirror"
    };

    MILPCChar const s_arrColorSrcSwitchRepeatStrings[] =
    {
        "eEColorSrcSwitchRepeat_Repeat",
        "eEColorSrcSwitchRepeat_Clamp",
        "eEColorSrcSwitchRepeat_PingPong"
    };

    MILPCChar const s_arrFresnelTermStrings[] =
    {
        "eEFresnelTerm_Simple",
        "eEFresnelTerm_Quadric",
        "eEFresnelTerm_Power"
    };

    MILPCChar const s_arrReflectTypeStrings[] =
    {
        "eEReflectType_Reflect",
        "eEReflectType_WorldEye",
        "eEReflectType_WorldNormal",
        "eEReflectType_TangentNormal",
        "eEReflectType_TangentEye"
    };

    MILPCChar const s_arrShaderMaterialAmbientTypeStrings[] =
    {
        "eEShaderMaterialAmbientType_None",
        "eEShaderMaterialAmbientType_Simple",
        "eEShaderMaterialAmbientType_Complex"
    };

    MILPCChar const s_arrShaderMaterialBRDFTypeStrings[] =
    {
        "eEShaderMaterialBRDFType_Simple",
        "eEShaderMaterialBRDFType_Complex",
        "eEShaderMaterialBRDFType_WrapAround"
    };

    MILPCChar const s_arrShaderMaterialBlendModeStrings[] =
    {
        "eEShaderMaterialBlendMode_Normal",
        "eEShaderMaterialBlendMode_Masked",
        "eEShaderMaterialBlendMode_AlphaBlend",
        "eEShaderMaterialBlendMode_Modulate",
        "eEShaderMaterialBlendMode_AlphaModulate",
        "eEShaderMaterialBlendMode_Translucent",
        "eEShaderMaterialBlendMode_Darken",
        "eEShaderMaterialBlendMode_Brighten",
        "eEShaderMaterialBlendMode_Invisible"
    };

    MILPCChar const s_arrShaderMaterialTransformationStrings[] =
    {
        "eEShaderMaterialTransformation_Default",
        "eEShaderMaterialTransformation_Instanced",
        "eEShaderMaterialTransformation_Skinned",
        "eEShaderMaterialTransformation_Tree_Branches",
        "eEShaderMaterialTransformation_Tree_Fronds",
        "eEShaderMaterialTransformation_Tree_LeafCards",
        "eEShaderMaterialTransformation_Tree_LeafMeshes",
        "eEShaderMaterialTransformation_Billboard",
        "eEShaderMaterialTransformation_Morphed",
        "eEShaderMaterialTransformation_Skinned_Morphed",
        "eEShaderMaterialTransformation_Vegetation",
        "eEShaderMaterialTransformation_Tree_Billboards"
    };

    MILPCChar const s_arrShaderMaterialVersionStrings[] =
    {
        "eEShaderMaterialVersion_1_1",
        "eEShaderMaterialVersion_1_4",
        "eEShaderMaterialVersion_2_0",
        "eEShaderMaterialVersion_3_0"
    };

    MILPCChar const s_arrShapeMaterialStrings[] =
    {
        "eEShapeMaterial_None",
        "eEShapeMaterial_Wood",
        "eEShapeMaterial_Metal",
        "eEShapeMaterial_Water",
        "eEShapeMaterial_Stone",
        "eEShapeMaterial_Earth",
        "eEShapeMaterial_Ice",
        "eEShapeMaterial_Leather",
        "eEShapeMaterial_Clay",
        "eEShapeMaterial_Glass",
        "eEShapeMaterial_Flesh",
        "eEShapeMaterial_Snow",
        "eEShapeMaterial_Debris",
        "eEShapeMaterial_Foliage",
        "eEShapeMaterial_Magic",
        "eEShapeMaterial_Grass",
        "eEShapeMaterial_SpringAndDamper1",
        "eEShapeMaterial_SpringAndDamper2",
        "eEShapeMaterial_SpringAndDamper3"
    };

    MILPCChar const s_arrTexCoordSrcOscillatorTypeStrings[] =
    {
        "eETexCoordSrcOscillatorType_Pan",
        "eETexCoordSrcOscillatorType_Stretch",
        "eETexCoordSrcOscillatorType_StretchRepeat",
        "eETexCoordSrcOscillatorType_Jitter"
    };

    MILPCChar const s_arrTexCoordSrcRotatorTypeStrings[] =
    {
        "eETexCoordSrcRotatorType_Once",
        "eETexCoordSrcRotatorType_Constant",
        "eETexCoordSrcRotatorType_Oscillate"
    };

    MILPCChar const s_arrEnumNames[] =
    {
        "eEColorSrcCombinerType",
        "eEColorSrcSampleTexRepeat",
        "eEColorSrcSwitchRepeat",
        "eEFresnelTerm",
        "eEReflectType",
        "eEShaderMaterialAmbientType",
        "eEShaderMaterialBRDFType",
        "eEShaderMaterialBlendMode",
        "eEShaderMaterialTransformation",
        "eEShaderMaterialVersion",
        "eEShapeMaterial",
        "eETexCoordSrcOscillatorType",
        "eETexCoordSrcRotatorType"
    };

    MILPCChar const * s_arrEnumStringArrays[] =
    {
        s_arrColorSrcCombinerTypeStrings,
        s_arrColorSrcSampleTexRepeatStrings,
        s_arrColorSrcSwitchRepeatStrings,
        s_arrFresnelTermStrings,
        s_arrReflectTypeStrings,
        s_arrShaderMaterialAmbientTypeStrings,
        s_arrShaderMaterialBRDFTypeStrings,
        s_arrShaderMaterialBlendModeStrings,
        s_arrShaderMaterialTransformationStrings,
        s_arrShaderMaterialVersionStrings,
        s_arrShapeMaterialStrings,
        s_arrTexCoordSrcOscillatorTypeStrings,
        s_arrTexCoordSrcRotatorTypeStrings
    };

    MIUInt s_arrEnumCounts[] =
    {
        MI_C_ARRAYSIZE( s_arrColorSrcCombinerTypeStrings ),
        MI_C_ARRAYSIZE( s_arrColorSrcSampleTexRepeatStrings ),
        MI_C_ARRAYSIZE( s_arrColorSrcSwitchRepeatStrings ),
        MI_C_ARRAYSIZE( s_arrFresnelTermStrings ),
        MI_C_ARRAYSIZE( s_arrReflectTypeStrings ),
        MI_C_ARRAYSIZE( s_arrShaderMaterialAmbientTypeStrings ),
        MI_C_ARRAYSIZE( s_arrShaderMaterialBRDFTypeStrings ),
        MI_C_ARRAYSIZE( s_arrShaderMaterialBlendModeStrings ),
        MI_C_ARRAYSIZE( s_arrShaderMaterialTransformationStrings ),
        MI_C_ARRAYSIZE( s_arrShaderMaterialVersionStrings ),
        MI_C_ARRAYSIZE( s_arrShapeMaterialStrings ),
        MI_C_ARRAYSIZE( s_arrTexCoordSrcOscillatorTypeStrings ),
        MI_C_ARRAYSIZE( s_arrTexCoordSrcRotatorTypeStrings )
    };

    struct SGuid
    {
        MIU64  m_u64GuidLow;
        MIU64  m_u64GuidHigh;
        MIBool m_bValid;
        MIChar m_arrPadding[ 3 ];
    };

    SGuid GetShaderElementToken( mCBuffer const & a_bufClassData )
    {
        MILPCByte pShaderElementBaseVersion = a_bufClassData.GetMemory() + a_bufClassData.GetSize() - 38;
        if ( pShaderElementBaseVersion[ 0 ] == 0 && pShaderElementBaseVersion[ -1 ] >= 2 )
            --pShaderElementBaseVersion;
        return *reinterpret_cast< SGuid const * >( pShaderElementBaseVersion + 2 );
    }

    void ReadBuffer( mCGenomeFile & a_File, mCBuffer & a_bufDest, MIUInt a_uSize )
    {
        a_bufDest.Resize( a_uSize );
        a_File().Read( a_bufDest.AccessMemory(), a_uSize );
    }

    void WriteString( mCString const & a_strSource, mCIOStreamBinary & a_streamDest )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() );
        a_streamDest << a_strSource;
    }

    void WriteRelativeOffset( mCIOStreamBinary & a_streamDest, MIUInt a_uBaseOffset )
    {
        MIUInt const uOffset = a_streamDest.Tell();
        a_streamDest.Seek( a_uBaseOffset - 4 );
        a_streamDest << g_32( uOffset - a_uBaseOffset );
        a_streamDest.Seek( uOffset );
    }

    MIBool ReadPropertyObjectHeader( mCGenomeFile & a_File, mCString & a_strClassName, MIUInt & a_uSize, MIUInt & a_uVersion )
    {
        if ( ( a_File().ReadU16() != 1 ) ||
             ( a_File().ReadU8() != 1 ) ||
             ( a_File().ReadU16() != 1 ) ||
             ( a_File().ReadU8() != 1 ) ||
             ( ( a_strClassName = a_File.ReadString() ).GetLength() == 0 ) ||
             ( a_File().ReadU16() != 1 ) ||
             ( a_File().ReadU8() != 0 ) )
            return MIFalse;
        a_uVersion = a_File().ReadU16();
        MIUInt const uPropertySystemVersion = a_File().ReadU16();
        a_File() >> g_32( a_uSize );
        MIUInt uOffset = a_File().Tell();
        if ( uPropertySystemVersion < 81 )
            a_File.ReadString();
        if ( uPropertySystemVersion < 82 )
            a_File().Skip( 20 );
        a_uSize -= ( a_File().Tell() - uOffset );
        return MITrue;
    }

    void WritePropertyObjectHeader( mCIOStreamBinary & a_streamDest, mCString const & a_strClassName, MIUInt a_uSize, MIUInt a_uVersion, MIBool a_bOldVersion )
    {
        a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
        WriteString( a_strClassName, a_streamDest );
        a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 0 ) << ( MIU16 )( a_uVersion ) << ( MIU16 )( a_bOldVersion ? 83 : 201 ) << g_32( a_uSize );
    }

    void ReadPropertyData( mCGenomeFile & a_File, mTArray< mCGenomeMaterial::SProperty > & a_arrPropertiesDest )
    {
        a_arrPropertiesDest.Clear();
        a_File().ReadU16();
        MIUInt uPropertyCount = a_File().ReadU32();
        for ( MIUInt u = 0; u != uPropertyCount; ++u )
        {
            mCGenomeMaterial::SProperty & Property = a_arrPropertiesDest.AddNew();
            a_File >> Property.m_strName >> Property.m_strType;
            a_File().ReadU16();
            MIUInt const uSize = a_File().ReadU32();
            if ( Property.m_strType == "float" )
            {
                Property.m_varValue.SetData( a_File().ReadFloat() );
            }
            else if ( Property.m_strType == "bool" )
            {
                Property.m_varValue.SetData( a_File().ReadBool() );
            }
            else if ( Property.m_strType == "char" )
            {
                Property.m_varValue.SetData( a_File().ReadU8() );
            }
            else if ( Property.m_strType == "int" )
            {
                Property.m_varValue.SetData( a_File().ReadI32() );
            }
            else if ( Property.m_strType == "bCVector2" )
            {
                mCVec3 vecDest;
                a_File() >> vecDest.AccessX() >> vecDest.AccessY();
                Property.m_varValue.SetData( vecDest );
            }
            else if ( Property.m_strType == "bCEulerAngles" )
            {
                mCVec3 vecDest;
                a_File() >> vecDest;
                Property.m_varValue.SetData( vecDest );
            }
            else if ( Property.m_strType == "bCFloatColor" )
            {
                mCVec3 vecColor;
                a_File().ReadU32();
                a_File() >> vecColor;
                Property.m_varValue.SetData( vecColor );
            }
            else if ( Property.m_strType == "bCImageResourceString" || Property.m_strType == "bCImageOrMaterialResourceString" || Property.m_strType == "bCString" )
            {
                Property.m_varValue.SetData( a_File.ReadString() );
            }
            else if ( Property.m_strType.FirstOf( "bTPropertyContainer<enum " ) == 0 )
            {
                mCString const strEnumName = mCString( Property.m_strType ).TrimLeft( 25u ).TrimRight( 1u );
                mCGenomeMaterial::SEnumContainer EnumContainer = { 0, 0, 0 };
                a_File().ReadU16();
                a_File() >> g_32( EnumContainer.m_uValue );
                for ( MIUInt v = 0; v != MI_C_ARRAYSIZE( s_arrEnumNames ); ++v )
                {
                    if ( strEnumName != s_arrEnumNames[ v ] )
                        continue;
                    EnumContainer.m_uCount = s_arrEnumCounts[ v ];
                    EnumContainer.m_pEnumNames = s_arrEnumStringArrays[ v ];
                    break;
                }
                Property.m_varValue.SetData( EnumContainer );
            }
            else
            {
                mCBuffer bufDest;
                Property.m_varValue.SetData( bufDest );
                bufDest.Resize( uSize );
                a_File().Read( bufDest.AccessMemory(), uSize );
                Property.m_varValue.SwapData( bufDest );
            }
        }
    }

    void WritePropertyData( mCIOStreamBinary & a_streamDest, mTArray< mCGenomeMaterial::SProperty > const & a_arrPropertiesSource, MIBool a_bOldVersion )
    {
        a_streamDest << ( MIU16 )( a_bOldVersion ? 30 : 201 );
        a_streamDest << g_32( a_arrPropertiesSource.GetCount() );
        for ( MIUInt u = 0, ue = a_arrPropertiesSource.GetCount(); u != ue; ++u )
        {
            mCGenomeMaterial::SProperty const & Property = a_arrPropertiesSource[ u ];
            WriteString( Property.m_strName, a_streamDest );
            WriteString( Property.m_strType, a_streamDest );
            a_streamDest << ( MIU16 )( 30 );
            if ( Property.m_strType == "float" )
            {
                a_streamDest << ( MIU32 )( 4 ) << Property.m_varValue.GetData< MIFloat >();
            }
            else if ( Property.m_strType == "bool" )
            {
                a_streamDest << ( MIU32 )( 1 ) << Property.m_varValue.GetData< MIBool >();
            }
            else if ( Property.m_strType == "char" )
            {
                a_streamDest << ( MIU32 )( 1 ) << Property.m_varValue.GetData< MIU8 >();
            }
            else if ( Property.m_strType == "int" )
            {
                a_streamDest << ( MIU32 )( 4 ) << g_32( Property.m_varValue.GetData< MII32 >() );
            }
            else if ( Property.m_strType == "bCVector2" )
            {
                mCVec3 vecSource = Property.m_varValue.GetData< mCVec3 >();
                a_streamDest << ( MIU32 )( 8 ) << vecSource.GetX() << vecSource.GetY();
            }
            else if ( Property.m_strType == "bCEulerAngles" )
            {
                a_streamDest << ( MIU32 )( 12 ) << Property.m_varValue.GetData< mCVec3 >();
            }
            else if ( Property.m_strType == "bCFloatColor" )
            {
                mCVec3 vecColor = Property.m_varValue.GetData< mCVec3 >();
                a_streamDest << ( MIU32 )( 16 ) << ( MIU32 )( 0 ) << vecColor;
            }
            else if ( Property.m_strType == "bCImageResourceString" || Property.m_strType == "bCImageOrMaterialResourceString" || Property.m_strType == "bCString" )
            {
                mCString strSource = Property.m_varValue.GetData< mCString >();
                a_streamDest << g_32( 2 + strSource.GetLength() );
                WriteString( strSource, a_streamDest );
            }
            else if ( Property.m_strType.FirstOf( "bTPropertyContainer<enum " ) == 0 )
            {
                a_streamDest << ( MIU32 )( 6 ) << ( MIU16 )( a_bOldVersion ? 1 : 201 ) << g_32( Property.m_varValue.GetData< mCGenomeMaterial::SEnumContainer >().m_uValue );
            }
            else
            {
                mCBuffer bufSource = Property.m_varValue.GetData< mCBuffer >();
                a_streamDest << g_32( bufSource.GetSize() );
                a_streamDest.Write( bufSource.GetMemory(), bufSource.GetSize() );
            }
        }
    }
}

mCGenomeMaterial::MIFMaterialFileFinderFunction mCGenomeMaterial::s_pfuncMaterialFileFinderFunction = 0;

mCGenomeMaterial::mCGenomeMaterial( void ) :
    m_pResourceShaderMaterial( 0 ),
    m_pShaderDefault( 0 )
{
}

mCGenomeMaterial::mCGenomeMaterial( mCIOStreamBinary & a_streamSource ) :
    m_pResourceShaderMaterial( 0 ),
    m_pShaderDefault( 0 )
{
    Load( a_streamSource );
}

mCGenomeMaterial::~mCGenomeMaterial( void )
{
    Invalidate();
}

MIBool & mCGenomeMaterial::AccessMaterialLookupHint( void )
{
    static MIBool s_bMaterialLookupHint = MITrue;
    return s_bMaterialLookupHint;
}

MIBool mCGenomeMaterial::FindGothic3Material( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest )
{
    return FindMaterial( a_strMaterialName, a_matDest, "xshmat" );
}

MIBool mCGenomeMaterial::FindRisenMaterial( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest )
{
    return FindMaterial( a_strMaterialName, a_matDest, "_xmat" );
}

void mCGenomeMaterial::LoadGothic3Materials( mCScene & a_sceneDest )
{
    LoadMaterials( a_sceneDest, "xshmat" );
}

void mCGenomeMaterial::LoadRisenMaterials( mCScene & a_sceneDest )
{
    LoadMaterials( a_sceneDest, "_xmat" );
}

void mCGenomeMaterial::RegisterMaterialFileFinderFunction( MIFMaterialFileFinderFunction a_pfuncMaterialFileFinderFunction )
{
    s_pfuncMaterialFileFinderFunction = a_pfuncMaterialFileFinderFunction;
}

mCGenomeMaterial::EColorSourceType mCGenomeMaterial::GetColorSourceType( MIUInt a_uShaderElementIndex ) const
{
    if ( !IsValid()  )
        return EColorSourceType_Invalid;
    SGuid Token = GetShaderElementToken( GetShaderElement( a_uShaderElementIndex ).m_bufClassData );
    if ( !Token.m_bValid )
        return EColorSourceType_Invalid;
    MILPCByte pColorSrcProxy = m_pShaderDefault->m_bufClassData.GetMemory();
    MIUInt const uVersion = *reinterpret_cast< MIU16 const * >( pColorSrcProxy );
    MIUInt const uColorSrcProxySize = *reinterpret_cast< MIU32 const * >( pColorSrcProxy ) >= 666 ? 32 : 24;
    pColorSrcProxy += uColorSrcProxySize - 18;
    MIUInt const uEnd = EColorSourceType_Count - ( uVersion < 2 ? 2 : ( uVersion < 3 ? 1 : 0 ) );
    for ( MIUInt u = 0; u != uEnd; ++u, pColorSrcProxy += uColorSrcProxySize )
    {
        SGuid const * pProxyToken = reinterpret_cast< SGuid const * >( pColorSrcProxy );
        if ( pProxyToken->m_bValid && pProxyToken->m_u64GuidHigh == Token.m_u64GuidHigh && pProxyToken->m_u64GuidLow == Token.m_u64GuidLow )
            return static_cast< EColorSourceType >( u );
    }
    return EColorSourceType_Invalid;
}

MIUInt mCGenomeMaterial::GetReferencedColorSources( MIUInt a_uShaderElementIndex, MIUInt ( & a_arrShaderElementIndices )[ 3 ] ) const
{
    a_arrShaderElementIndices[ 0 ] = a_arrShaderElementIndices[ 1 ] = a_arrShaderElementIndices[ 2 ] = MI_DW_INVALID;
    if ( !IsValid() )
        return 0;
    SPropertyObject const & ShaderElement = GetShaderElement( a_uShaderElementIndex );
    MILPCChar arrClassNames[ 3 ] = { "eCColorSrcMask", "eCColorSrcCombiner", "eCColorSrcBlend" };
    MIUInt uCount;
    for ( uCount = 3; uCount != 0; --uCount )
        if ( ShaderElement.m_strClassName == arrClassNames[ uCount - 1 ] )
            break;
    if ( !uCount )
        return 0;
    MILPCByte pColorSrcProxy = ShaderElement.m_bufClassData.GetMemory() + 2 + 4;
    MIUInt const uColorSrcProxySize = reinterpret_cast< MIU32 const * >( pColorSrcProxy )[ -1 ] >= 666 ? 32 : 24;
    for ( MIUInt u = uCount, v, ve; u--; )
    {
        SGuid const & ProxyToken = *reinterpret_cast< SGuid const * >( pColorSrcProxy + uColorSrcProxySize * u );
        for ( v = 0, ve = GetShaderElementCount(); ProxyToken.m_bValid && v != ve; ++v )
        {
            SGuid Token = GetShaderElementToken( GetShaderElement( a_uShaderElementIndex ).m_bufClassData );
            if ( Token.m_u64GuidHigh == ProxyToken.m_u64GuidHigh && Token.m_u64GuidLow == ProxyToken.m_u64GuidLow )
            {
                a_arrShaderElementIndices[ u ] = v;
                break;
            }
        }
    }
    return uCount;
}

void mCGenomeMaterial::GetMaterialData( mCMaterial & a_matDest ) const
{
    if ( !IsValid() )
        return;
    for ( MIUInt u = GetShaderElementCount(); u--; )
    {
        EColorSourceType enumType = GetColorSourceType( u );
        mCTexMap mapSource;
        mCVariant varPath;
        if ( !GetPropertyDeep( u, "ImageFilePath", varPath ) )
            continue;
        mapSource.AccessTextureFilePath() = varPath.GetData< mCString >();
        switch ( enumType )
        {
        case EColorSourceType_Diffuse:
            a_matDest.SetTextureMapAt( mCMaterial::EMapType_Diffuse, mapSource.GetTextureFilePath().GetLength() ? &mapSource : 0 );
            break;
        case EColorSourceType_Specular:
            a_matDest.SetTextureMapAt( mCMaterial::EMapType_Specular, mapSource.GetTextureFilePath().GetLength() ? &mapSource : 0 );
            break;
        case EColorSourceType_Normal:
            a_matDest.SetTextureMapAt( mCMaterial::EMapType_Normal, mapSource.GetTextureFilePath().GetLength() ? &mapSource : 0 );
            break;
        }
    }
}

mCGenomeMaterial::SProperty const & mCGenomeMaterial::GetProperty( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties[ a_uPropertyIndex ];
}

MIUInt mCGenomeMaterial::GetPropertyCount( MIUInt a_uShaderElementIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties.GetCount();
}

MIBool mCGenomeMaterial::GetPropertyDeep( MIUInt a_uShaderElementIndex, mCString const & a_strPropertyName, mCVariant & a_varDest ) const
{
    for ( MIUInt u = GetPropertyCount( a_uShaderElementIndex ); u--; )
    {
        SProperty const & Property = GetProperty( a_uShaderElementIndex, u );
        if ( Property.m_strName == a_strPropertyName )
            return a_varDest = Property.m_varValue, MITrue;
    }
    MIUInt arrColorSources[ 3 ];
    MIUInt uColorSrcCount = GetReferencedColorSources( a_uShaderElementIndex, arrColorSources );
    if ( !uColorSrcCount )
        return MIFalse;
    for ( MIUInt u = 0; u != uColorSrcCount; ++u )
        if ( arrColorSources[ u ] != MI_DW_INVALID && GetPropertyDeep( arrColorSources[ u ], a_strPropertyName, a_varDest ) )
            return MITrue;
    return MIFalse;
}

MIUInt mCGenomeMaterial::GetShaderElementCount( void ) const
{
    return m_arrShaderElements.GetCount();
}

mCString mCGenomeMaterial::GetShaderElementType( MIUInt a_uShaderElementIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_strClassName;
}

void mCGenomeMaterial::Invalidate( void )
{
    m_arrShaderElements.Clear();
    m_bufResourceHeader.Resize( 0 );
    delete m_pResourceShaderMaterial;
    m_pResourceShaderMaterial = 0;
    delete m_pShaderDefault;
    m_pShaderDefault = 0;
}

MIBool mCGenomeMaterial::IsReadOnly( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties[ a_uPropertyIndex ].m_varValue.IsOfType< mCBuffer >();
}

MIBool mCGenomeMaterial::IsValid( void ) const
{
    return m_pResourceShaderMaterial && m_pShaderDefault;
}

mEResult mCGenomeMaterial::Load( mCIOStreamBinary & a_streamSource )
{
    Invalidate();
    mCGenomeFile File( &a_streamSource );
    File.StartRead();
    if ( File().GetSize() < 40 )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid genome material file." ), mEResult_False;
    m_pResourceShaderMaterial = new SPropertyObject;
    m_pShaderDefault = new SPropertyObject;
    m_bOldVersion = ( File().ReadString( 8 ) != "GR01SM01" );
    File().Skip( -8 );
    if ( !m_bOldVersion )
    {
        m_bufResourceHeader.Resize( 40 );
        File().Read( m_bufResourceHeader.AccessMemory(), 40 );
    }
    if ( !ReadPropertyObjectHeader( File, m_pResourceShaderMaterial->m_strClassName, m_pResourceShaderMaterial->m_uSize, m_pResourceShaderMaterial->m_uVersion ) ||
         m_pResourceShaderMaterial->m_strClassName != ( m_bOldVersion ? "eCResourceShaderMaterial_PS" : "eCMaterialResource2" ) )
    {
        Invalidate();
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid genome material file." ), mEResult_False;
    }
    ReadPropertyData( File, m_pResourceShaderMaterial->m_arrProperties );
    MIUInt uClassDataSize = 2;
    if ( m_bOldVersion )
    {
        File().ReadU16();
        MIUInt const uVersion = File().ReadU16();
        File().Skip( -4 );
        uClassDataSize = ( uVersion >= 23 ? ( uVersion >= 30 ? 8 : 11 ) : ( uVersion >= 2 ? 7 : 6 ) );
    }
    ReadBuffer( File, m_pResourceShaderMaterial->m_bufClassData, uClassDataSize );
    ReadPropertyObjectHeader( File, m_pShaderDefault->m_strClassName, m_pShaderDefault->m_uSize, m_pShaderDefault->m_uVersion );
    ReadPropertyData( File, m_pShaderDefault->m_arrProperties );
    MIUInt uOffset = File().Tell();
    MIUInt const uVersion = File().ReadU16();
    MIUInt uColorSourceCount = EColorSourceType_Count - ( uVersion < 2 ? 2 : ( uVersion < 3 ? 1 : 0 ) );
    uClassDataSize = 2 + uColorSourceCount * ( File().ReadU32() >= 666 ? 32 : 24 );
    File().Seek( uOffset + uClassDataSize + 2 );
    uClassDataSize += File().ReadU16() >= 2 ? 41 : 40;
    File().Seek( uOffset );
    ReadBuffer( File, m_pShaderDefault->m_bufClassData, uClassDataSize );
    MIUInt const uShaderElementCount = File().ReadU32();
    m_arrShaderElements.Resize( uShaderElementCount );
    for ( MIUInt u = 0; u != uShaderElementCount; ++u )
    {
        SPropertyObject & ShaderElement = m_arrShaderElements[ u ];
        ReadPropertyObjectHeader( File, ShaderElement.m_strClassName, ShaderElement.m_uSize, ShaderElement.m_uVersion );
        uOffset = File().Tell();
        ReadPropertyData( File, ShaderElement.m_arrProperties );
        ReadBuffer( File, ShaderElement.m_bufClassData, uOffset + ShaderElement.m_uSize - File().Tell() );
    }
    return mEResult_Ok;
}

mEResult mCGenomeMaterial::Save( mCIOStreamBinary & a_streamDest )
{
    if ( !IsValid() )
        return mEResult_False;
    if ( !m_bOldVersion )
        a_streamDest.Write( m_bufResourceHeader.GetMemory(), m_bufResourceHeader.GetSize() );
    WritePropertyObjectHeader( a_streamDest, m_pResourceShaderMaterial->m_strClassName, 0, m_pResourceShaderMaterial->m_uVersion, m_bOldVersion );
    MIUInt uResourceShaderMaterialDataOffset = a_streamDest.Tell();
    WritePropertyData( a_streamDest, m_pResourceShaderMaterial->m_arrProperties, m_bOldVersion );
    a_streamDest.Write( m_pResourceShaderMaterial->m_bufClassData.GetMemory(), m_pResourceShaderMaterial->m_bufClassData.GetSize() );
    if ( !m_bOldVersion )
        WriteRelativeOffset( a_streamDest, uResourceShaderMaterialDataOffset );
    WritePropertyObjectHeader( a_streamDest, m_pShaderDefault->m_strClassName, 0, m_pShaderDefault->m_uVersion, m_bOldVersion );
    MIUInt uShaderDefaultDataOffset = a_streamDest.Tell();
    WritePropertyData( a_streamDest, m_pShaderDefault->m_arrProperties, m_bOldVersion );
    a_streamDest.Write( m_pShaderDefault->m_bufClassData.GetMemory(), m_pShaderDefault->m_bufClassData.GetSize() );
    a_streamDest << g_32( m_arrShaderElements.GetCount() );
    for ( MIUInt u = 0, ue = m_arrShaderElements.GetCount(); u != ue; ++u )
    {
        SPropertyObject const & ShaderElement = m_arrShaderElements[ u ];
        WritePropertyObjectHeader( a_streamDest, ShaderElement.m_strClassName, 0, ShaderElement.m_uVersion, m_bOldVersion );
        MIUInt uShaderElementDataOffset = a_streamDest.Tell();
        WritePropertyData( a_streamDest, ShaderElement.m_arrProperties, m_bOldVersion );
        a_streamDest.Write( ShaderElement.m_bufClassData.GetMemory(), ShaderElement.m_bufClassData.GetSize() );
        WriteRelativeOffset( a_streamDest, uShaderElementDataOffset );
    }
    WriteRelativeOffset( a_streamDest, uShaderDefaultDataOffset );
    if ( m_bOldVersion )
        WriteRelativeOffset( a_streamDest, uResourceShaderMaterialDataOffset );
    if ( !m_bOldVersion )
    {
        MIUInt uPropertyObjectSize = uShaderDefaultDataOffset - uResourceShaderMaterialDataOffset + m_pResourceShaderMaterial->m_strClassName.GetLength() - m_pShaderDefault->m_strClassName.GetLength();
        MIUInt uResourceDataSize = a_streamDest.Tell() - m_bufResourceHeader.GetSize() - uPropertyObjectSize;
        a_streamDest.Seek( 12 );
        a_streamDest << g_32( uPropertyObjectSize ) << g_32( uPropertyObjectSize + m_bufResourceHeader.GetSize() ) << g_32( uResourceDataSize ) << g_time();
    }
    return mEResult_Ok;
}

void mCGenomeMaterial::SetMaterialData( mCMaterial const & a_matSource )
{
    if ( !IsValid() )
        return;
    mCVariant varImagePath;
    for ( MIUInt u = GetShaderElementCount(); u--; )
    {
        EColorSourceType enumType = GetColorSourceType( u );
        MIUInt uPropertyIndex = MI_DW_INVALID;
        mCTexMap const * pMapSource = 0;
        for ( MIUInt v = GetPropertyCount( u ); v--; )
        {
            SProperty const & Property = GetProperty( u, v );
            if ( Property.m_strName == "ImageFilePath" && !IsReadOnly( u, v ) )
                uPropertyIndex = v;
        }
        if ( uPropertyIndex == MI_DW_INVALID )
            continue;
        switch ( enumType )
        {
        case EColorSourceType_Diffuse:
            pMapSource = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Diffuse );
            break;
        case EColorSourceType_Specular:
            pMapSource = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Specular );
            break;
        case EColorSourceType_Normal:
            pMapSource = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Normal );
            break;
        }
        varImagePath.SetData< mCString >( pMapSource ? pMapSource->GetTextureFilePath() : "" );
        SetProperty( u, uPropertyIndex, varImagePath );
    }
}

void mCGenomeMaterial::SetProperty( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex, mCVariant a_varValue )
{
    if ( !IsValid() )
        return;
    SProperty & Property = AccessShaderElement( a_uShaderElementIndex ).m_arrProperties[ a_uPropertyIndex ];
    MIInt iDelta = 0;
    if ( !IsReadOnly( a_uShaderElementIndex, a_uPropertyIndex ) && Property.m_varValue.IsOfType( a_varValue ) )
    {
        if ( Property.m_varValue.IsOfType< mCString >() )
            iDelta = a_varValue.GetData< mCString >().GetLength() - Property.m_varValue.GetData< mCString >().GetLength();
        Property.m_varValue.Swap( a_varValue );
    }
    if ( !iDelta )
        return;
    AccessShaderElement( a_uShaderElementIndex ).m_uSize += iDelta;
}

MIBool mCGenomeMaterial::FindMaterial( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest, mCString const & a_strExt )
{
    if ( !s_pfuncMaterialFileFinderFunction )
        return MIFalse;
    mCIOStreamBinary * pStreamIn = 0;
    ( *s_pfuncMaterialFileFinderFunction )( a_strMaterialName.GetText(), a_strExt.GetText(), pStreamIn );
    if ( !pStreamIn )
        return MIFalse;
    mCError const * const pLastError = mCError::GetLastError< mCError >();
    MIBool bResult = ( a_matDest.Load( *pStreamIn ) == mEResult_Ok );
    while ( mCError::GetLastError< mCError >() != pLastError )
        mCError::ClearError( mCError::GetLastError< mCError >() );
    delete pStreamIn;
    return bResult;
}

void mCGenomeMaterial::LoadMaterials( mCScene & a_sceneDest, mCString const & a_strEx )
{
    for ( MIUInt u = a_sceneDest.GetNumMaterials(); u--; )
    {
        mCMaterial * pMaterial = dynamic_cast< mCMaterial * >( a_sceneDest.AccessMaterialAt( u ) );
        mCMultiMaterial * pMultiMaterial = dynamic_cast< mCMultiMaterial * >( a_sceneDest.AccessMaterialAt( u ) );
        if ( pMaterial )
        {
            mCGenomeMaterial matSource;
            if ( !FindMaterial( pMaterial->GetName(), matSource, a_strEx ) )
                continue;
            matSource.GetMaterialData( *pMaterial );
        }
        if ( pMultiMaterial )
        {
            mTArray< mCMaterial > & arrSubMaterials = pMultiMaterial->AccessSubMaterials();
            for ( MIUInt v = arrSubMaterials.GetCount(); v--; )
            {
                mCGenomeMaterial matSource;
                if ( !FindMaterial( arrSubMaterials[ v ].GetName(), matSource, a_strEx ) )
                    continue;
                matSource.GetMaterialData( arrSubMaterials[ v ] );
            }
        }
    }
}

mCGenomeMaterial::SPropertyObject & mCGenomeMaterial::AccessShaderElement( MIUInt a_uShaderElementIndex )
{
    return const_cast< SPropertyObject & >( GetShaderElement( a_uShaderElementIndex ) );
}

mCGenomeMaterial::SPropertyObject const & mCGenomeMaterial::GetShaderElement( MIUInt a_uShaderElementIndex ) const
{
    if ( a_uShaderElementIndex == ESpecialShaderElementIndex_ResourceShaderMaterial )
        return *m_pResourceShaderMaterial;
    else if ( a_uShaderElementIndex == ESpecialShaderElementIndex_ShaderDefault )
        return *m_pShaderDefault;
    return m_arrShaderElements[ a_uShaderElementIndex ];
}

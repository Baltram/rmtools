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

    MILPCChar const s_arrShaderClassDataDescriptors[] =
    {
        "vcccccc2c3c_eCShaderDefault",
        "_eCShaderLeafs",
        "vc_eCShaderLightStreaks",
        "vcccccc2c_eCShaderOcean",
        "vc2c_eCShaderParticle",
        "vcccccc2c_eCShaderRiver",
        "vcc_eCShaderSkyDome",
        "vcc_eCShaderVegetation"
    };

    MILPCChar const s_arrShaderElementClassDataDescriptors[] =
    {
        "vccc_eCColorSrcBlend ",
        "vcc_eCColorSrcCombiner",
        "vc_eCColorSrcMask",
        "vtd_eCColorSrcSampler",
        "vct_eCTexCoordSrcBumpOffset",
        "vcc_eCTexCoordSrcColor",
        "vt_eCTexCoordSrcOscillator",
        "vt_eCTexCoordSrcRotator",
        "vt_eCTexCoordSrcScale",
        "vt_eCTexCoordSrcScroller"
    };

    struct SGuid
    {
        MIU64  m_u64GuidLow;
        MIU64  m_u64GuidHigh;
        MIBool m_bValid;
        MIChar m_arrPadding[ 3 ];
    public:
        MIBool operator == ( SGuid const & a_Other ) { return m_bValid && a_Other.m_bValid && m_u64GuidLow == a_Other.m_u64GuidLow && m_u64GuidHigh == a_Other.m_u64GuidHigh; }
    };

    SGuid GetShaderElementToken( mCMemoryStream const & a_streamClassData )
    {
        MILPCByte pShaderElementBaseVersion = static_cast< MILPCByte >( a_streamClassData.GetBuffer() ) + a_streamClassData.GetSize() - 38;
        if ( pShaderElementBaseVersion[ 0 ] == 0 && pShaderElementBaseVersion[ -1 ] >= 2 )
            --pShaderElementBaseVersion;
        return *reinterpret_cast< SGuid const * >( pShaderElementBaseVersion + 2 );
    }

    void TransferData( mCGenomeFile & a_File, mCMemoryStream & a_stramDest, MIUInt a_uSize )
    {
        mCBuffer bufBuffer( a_uSize );
        a_File().Read( bufBuffer.AccessMemory(), a_uSize );
        a_stramDest.Write( bufBuffer.GetMemory(), a_uSize );
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

    MIBool FindDescriptor( mCString const & a_strClassName, mCString & a_strDest )
    {
        MIBool const bShader = a_strClassName.Contains( "Shader" );
        MILPCChar const * pDescriptors = bShader ? s_arrShaderClassDataDescriptors : s_arrShaderElementClassDataDescriptors;
        MIUInt u = 0, ue = bShader ? MI_C_ARRAYSIZE( s_arrShaderClassDataDescriptors ) : MI_C_ARRAYSIZE( s_arrShaderElementClassDataDescriptors );
        for ( ; u != ue; ++u )
            if ( mCString( pDescriptors[ u ] ).Contains( a_strClassName.GetText() ) )
                break;
        if ( u == ue )
            return false;
        return a_strDest = pDescriptors[ u ], true;
    }

    void FindReferences( mCString a_strDataDescriptor, mCIOStreamBinary & a_streamSource, mTArray< SGuid > * a_pReferences, MIBool a_bResetStreamOffset )
    {
        a_strDataDescriptor += "_";
        if ( a_bResetStreamOffset )
            a_streamSource.Seek( 0 );
        for ( MIUInt u = 0, ue = a_strDataDescriptor.GetLength(), uVersion = 0; u != ue; ++u )
        {
            MIChar cCharacter = a_strDataDescriptor[ u ];
            while ( cCharacter <= '9' && cCharacter >= '0' )
                cCharacter = a_strDataDescriptor[ u += ( uVersion < static_cast< MIUInt >( cCharacter - '0' ) ? 2 : 1 ) ];
            if ( cCharacter  == '_' )
                break;
            SGuid Token;
            switch ( cCharacter )
            {
            case 'b':
                a_streamSource.ReadU8();
                break;
            case 'w':
                a_streamSource.ReadU16();
                break;
            case 'd':
                a_streamSource.ReadU32();
                break;
            case 'v':
                uVersion = a_streamSource.ReadU16();
                break;
            case 'c':
            case 't':
                if ( a_streamSource.ReadU32() >= 666 && cCharacter == 'c'  )
                    a_streamSource.Skip( 8 );
            case 'g':
                a_streamSource.Read( a_pReferences ? &a_pReferences->AddNew() : &Token, 20 );
                break;
            }
        }
        if ( a_bResetStreamOffset )
            a_streamSource.Seek( 0 );
    }

    mTArray< SGuid > FindReferences( mCString const & a_strClassName, mCMemoryStream a_streamSource )
    {
        mCString strDescriptor;
        mTArray< SGuid > arrReferences;
        FindDescriptor( a_strClassName, strDescriptor );
        FindReferences( strDescriptor, a_streamSource, &arrReferences, true );
        return arrReferences;
    }
}

mCGenomeMaterial::MIFMaterialFileFinderFunction mCGenomeMaterial::s_pfuncMaterialFileFinderFunction = 0;

mCGenomeMaterial::mCGenomeMaterial( void ) :
    m_pMaterialResourceObject( 0 ),
    m_pShaderObject( 0 )
{
}

mCGenomeMaterial::mCGenomeMaterial( mCIOStreamBinary & a_streamSource ) :
    m_pMaterialResourceObject( 0 ),
    m_pShaderObject( 0 )
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

void mCGenomeMaterial::LoadGothic3Materials( mCScene & a_sceneDest, mCString const & a_strTextureFileExtension )
{
    LoadMaterials( a_sceneDest, "xshmat", a_strTextureFileExtension );
}

void mCGenomeMaterial::LoadRisenMaterials( mCScene & a_sceneDest, mCString const & a_strTextureFileExtension )
{
    LoadMaterials( a_sceneDest, "_xmat", a_strTextureFileExtension );
}

void mCGenomeMaterial::RegisterMaterialFileFinderFunction( MIFMaterialFileFinderFunction a_pfuncMaterialFileFinderFunction )
{
    s_pfuncMaterialFileFinderFunction = a_pfuncMaterialFileFinderFunction;
}

mCGenomeMaterial::EColorSourceType mCGenomeMaterial::GetColorSourceType( MIUInt a_uShaderElementIndex ) const
{
    EColorSourceType arrShaderDefaultTypes[]      = { EColorSourceType_Diffuse, EColorSourceType_Opacity, EColorSourceType_SelfIllumination, EColorSourceType_Specular, EColorSourceType_SpecularPower, EColorSourceType_Normal, EColorSourceType_Distortion, EColorSourceType_LightEmission };
    EColorSourceType arrShaderLightStreaksTypes[] = { EColorSourceType_Diffuse };
    EColorSourceType arrShaderOceanTypes[]        = { EColorSourceType_Diffuse, EColorSourceType_StaticBump, EColorSourceType_FlowingBump, EColorSourceType_Specular, EColorSourceType_SpecularPower, EColorSourceType_Reflection, EColorSourceType_Distortion };
    EColorSourceType arrShaderParticleTypes[]     = { EColorSourceType_Diffuse, EColorSourceType_Distortion };
    EColorSourceType arrShaderSkyDomeTypes[]      = { EColorSourceType_Clouds, EColorSourceType_Absorbation };
    EColorSourceType arrShaderVegetationTypes[]   = { EColorSourceType_Diffuse, EColorSourceType_AlphaDisolve };
    EColorSourceType const * arrTypeArrays[]      = { arrShaderDefaultTypes, arrShaderLightStreaksTypes, arrShaderOceanTypes, arrShaderOceanTypes, arrShaderParticleTypes, arrShaderSkyDomeTypes, arrShaderVegetationTypes };
    MILPCChar arrClassNames[]                     = { "eCShaderDefault", "eCShaderLightStreaks", "eCShaderOcean", "eCShaderRiver", "eCShaderParticle", "eCShaderSkyDome", "eCShaderVegetation" };
    SGuid Token;
    if ( !IsValid() || !( Token = GetShaderElementToken( GetShaderElement( a_uShaderElementIndex ).m_streamClassData ) ).m_bValid )
        return EColorSourceType_Invalid;
    mTArray< SGuid > arrReferences = FindReferences( m_pShaderObject->m_strClassName, m_pShaderObject->m_streamClassData );
    for ( MIUInt u = 0, ue = arrReferences.GetCount(); u != ue; ++u )
        if ( arrReferences[ u ] == Token )
            for ( MIUInt v = 0, ve = MI_C_ARRAYSIZE( arrClassNames ); v != ve; ++v )
                if ( arrClassNames[ v ] == m_pShaderObject->m_strClassName )
                    return arrTypeArrays[ v ][ u ];
    return EColorSourceType_Invalid;
}

void mCGenomeMaterial::GetReferencedShaderElements( MIUInt a_uShaderElementIndex, mTArray< MIUInt > & a_arrReferencedShaderElementIndices ) const
{
    a_arrReferencedShaderElementIndices.Clear();
    mTArray< SGuid > arrReferences = FindReferences( GetShaderElement( a_uShaderElementIndex ).m_strClassName, GetShaderElement( a_uShaderElementIndex ).m_streamClassData );
    for ( MIUInt u = 0, ue = GetShaderElementCount(); u != ue; ++u )
        for ( MIUInt v = arrReferences.GetCount(); v--; )
            if ( GetShaderElementToken( GetShaderElement( u ).m_streamClassData ) == arrReferences[ v ] )
                a_arrReferencedShaderElementIndices.Add( u );
}

void mCGenomeMaterial::GetMaterialData( mCMaterial & a_matDest, mCString a_strTextureFileExtension ) const
{
    if ( !IsValid() )
        return;
    for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
        a_matDest.RemoveTexMap( i );
    const MIUInt uShaderElementCount = GetShaderElementCount();
    mTArray< MIBool > arrShaderElementIsReferencedDiffuse( MIFalse, uShaderElementCount );
    mTArray< MIBool > arrShaderElementIsReferencedSpecular( MIFalse, uShaderElementCount );
    mTArray< MIBool > arrShaderElementIsReferencedNormal( MIFalse, uShaderElementCount );
    for ( MIUInt u = uShaderElementCount; u--; )
    {
        switch ( GetColorSourceType( u ) )
        {
        case EColorSourceType_Diffuse:
            GetShaderElementIsReferencedDeep( u, arrShaderElementIsReferencedDiffuse );
            break;
        case EColorSourceType_Specular:
            GetShaderElementIsReferencedDeep( u, arrShaderElementIsReferencedSpecular );
            break;
        case EColorSourceType_Normal:
            GetShaderElementIsReferencedDeep( u, arrShaderElementIsReferencedNormal );
            break;
        }
    }
    if ( !a_strTextureFileExtension.StartsWith( "." ) && a_strTextureFileExtension != "" )
        a_strTextureFileExtension = "." + a_strTextureFileExtension;
    mCVariant varPath;
    for ( MIUInt u = uShaderElementCount; u--; )
    {
        if ( !GetProperty( u, "ImageFilePath", varPath ) || varPath.GetData< mCString >() == "" )
            continue;
        if ( arrShaderElementIsReferencedDiffuse[ u ] )
            a_matDest.AccessTexMap( mCMaterial::EMapType_Diffuse ) = mCTexMap( "", varPath.GetData< mCString >() + a_strTextureFileExtension );
        else if ( arrShaderElementIsReferencedSpecular[ u ] )
            a_matDest.AccessTexMap( mCMaterial::EMapType_Specular ) = mCTexMap( "", varPath.GetData< mCString >() + a_strTextureFileExtension );
        else if ( arrShaderElementIsReferencedNormal[ u ] )
            a_matDest.AccessTexMap( mCMaterial::EMapType_Normal ) = mCTexMap( "", varPath.GetData< mCString >() + a_strTextureFileExtension );
    }
}

mCGenomeMaterial::SProperty const & mCGenomeMaterial::GetProperty( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties[ a_uPropertyIndex ];
}

MIBool mCGenomeMaterial::GetProperty( MIUInt a_uShaderElementIndex, mCString const & a_strPropertyName, mCVariant & a_varDest ) const
{
    for ( MIUInt u = GetPropertyCount( a_uShaderElementIndex ); u--; )
    {
        SProperty const & Property = GetProperty( a_uShaderElementIndex, u );
        if ( Property.m_strName == a_strPropertyName )
            return a_varDest = Property.m_varValue, MITrue;
    }
    return MIFalse;
}

MIUInt mCGenomeMaterial::GetPropertyCount( MIUInt a_uShaderElementIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties.GetCount();
}

MIUInt mCGenomeMaterial::GetShaderElementCount( void ) const
{
    return m_arrShaderElements.GetCount();
}

void mCGenomeMaterial::GetShaderElementIsReferencedDeep( MIUInt a_uShaderElementIndex, mTArray< MIBool > & a_arrShaderElementIsReferenced ) const
{
    a_arrShaderElementIsReferenced.Resize( GetShaderElementCount() );
    a_arrShaderElementIsReferenced[ a_uShaderElementIndex ] = MITrue;
    mTArray< MIUInt > arrReferencedIndices;
    GetReferencedShaderElements( a_uShaderElementIndex, arrReferencedIndices );
    for ( MIUInt u = 0, ue = arrReferencedIndices.GetCount(); u != ue; ++u )
    {
        MIUInt uShaderElementIndex = arrReferencedIndices[ u ];
        if ( a_arrShaderElementIsReferenced[ uShaderElementIndex ] == MIFalse )
        {
            a_arrShaderElementIsReferenced[ uShaderElementIndex ] = MITrue;
            GetShaderElementIsReferencedDeep( uShaderElementIndex, a_arrShaderElementIsReferenced );
        }
    }
}

mCString mCGenomeMaterial::GetShaderElementType( MIUInt a_uShaderElementIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_strClassName;
}

void mCGenomeMaterial::Invalidate( void )
{
    m_arrShaderElements.Clear();
    m_bufResourceHeader.Resize( 0 );
    delete m_pMaterialResourceObject;
    m_pMaterialResourceObject = 0;
    delete m_pShaderObject;
    m_pShaderObject = 0;
}

MIBool mCGenomeMaterial::IsReadOnly( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const
{
    return GetShaderElement( a_uShaderElementIndex ).m_arrProperties[ a_uPropertyIndex ].m_varValue.IsOfType< mCBuffer >();
}

MIBool mCGenomeMaterial::IsValid( void ) const
{
    return m_pMaterialResourceObject && m_pShaderObject;
}

mEResult mCGenomeMaterial::Load( mCIOStreamBinary & a_streamSource )
{
    Invalidate();
    mCGenomeFile File( &a_streamSource );
    File.StartRead();
    if ( File().GetSize() < 40 )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid genome material file." ), mEResult_False;
    m_pMaterialResourceObject = new SPropertyObject;
    m_pShaderObject = new SPropertyObject;
    m_bOldVersion = ( File().ReadString( 8 ) != "GR01SM01" );
    File().Skip( -8 );
    if ( !m_bOldVersion )
    {
        m_bufResourceHeader.Resize( 40 );
        File().Read( m_bufResourceHeader.AccessMemory(), 40 );
    }
    if ( !ReadPropertyObjectHeader( File, m_pMaterialResourceObject->m_strClassName, m_pMaterialResourceObject->m_uSize, m_pMaterialResourceObject->m_uVersion ) ||
         m_pMaterialResourceObject->m_strClassName != ( m_bOldVersion ? "eCResourceShaderMaterial_PS" : "eCMaterialResource2" ) )
        return Invalidate(), MI_ERROR( mCStreamError, EBadFormat, "Invalid genome material file." ), mEResult_False;
    ReadPropertyData( File, m_pMaterialResourceObject->m_arrProperties );
    MIUInt uClassDataSize = 2;
    if ( m_bOldVersion )
    {
        File().ReadU16();
        MIUInt const uVersion = File().ReadU16();
        File().Skip( -4 );
        uClassDataSize = ( uVersion >= 23 ? ( uVersion >= 30 ? 8 : 11 ) : ( uVersion >= 2 ? 7 : 6 ) );
    }
    TransferData( File, m_pMaterialResourceObject->m_streamClassData, uClassDataSize );
    ReadPropertyObjectHeader( File, m_pShaderObject->m_strClassName, m_pShaderObject->m_uSize, m_pShaderObject->m_uVersion );
    ReadPropertyData( File, m_pShaderObject->m_arrProperties );
    MIUInt uOffset = File().Tell();
    mCString strShaderObjectClassDataDescriptor;
    if ( !FindDescriptor( m_pShaderObject->m_strClassName, strShaderObjectClassDataDescriptor ) )
        return Invalidate(), MI_ERROR( mCStreamError, EBadFormat, "Unknown genome material file version." ), mEResult_False;
    FindReferences( strShaderObjectClassDataDescriptor, File(), 0, MIFalse );
    FindReferences( "vvgdddd2b", File(), 0, MIFalse );
    uClassDataSize = File().Tell() - uOffset;
    File().Seek( uOffset );
    TransferData( File, m_pShaderObject->m_streamClassData, uClassDataSize );
    MIUInt const uShaderElementCount = File().ReadU32();
    m_arrShaderElements.Resize( uShaderElementCount );
    for ( MIUInt u = 0; u != uShaderElementCount; ++u )
    {
        SPropertyObject & ShaderElement = m_arrShaderElements[ u ];
        ReadPropertyObjectHeader( File, ShaderElement.m_strClassName, ShaderElement.m_uSize, ShaderElement.m_uVersion );
        uOffset = File().Tell();
        ReadPropertyData( File, ShaderElement.m_arrProperties );
        TransferData( File, ShaderElement.m_streamClassData, uOffset + ShaderElement.m_uSize - File().Tell() );
    }
    return mEResult_Ok;
}

mEResult mCGenomeMaterial::Save( mCIOStreamBinary & a_streamDest )
{
    if ( !IsValid() )
        return mEResult_False;
    if ( !m_bOldVersion )
        a_streamDest.Write( m_bufResourceHeader.GetMemory(), m_bufResourceHeader.GetSize() );
    WritePropertyObjectHeader( a_streamDest, m_pMaterialResourceObject->m_strClassName, 0, m_pMaterialResourceObject->m_uVersion, m_bOldVersion );
    MIUInt uMaterialResourceObjectDataOffset = a_streamDest.Tell();
    WritePropertyData( a_streamDest, m_pMaterialResourceObject->m_arrProperties, m_bOldVersion );
    a_streamDest << m_pMaterialResourceObject->m_streamClassData;
    if ( !m_bOldVersion )
        WriteRelativeOffset( a_streamDest, uMaterialResourceObjectDataOffset );
    WritePropertyObjectHeader( a_streamDest, m_pShaderObject->m_strClassName, 0, m_pShaderObject->m_uVersion, m_bOldVersion );
    MIUInt uShaderObjectDataOffset = a_streamDest.Tell();
    WritePropertyData( a_streamDest, m_pShaderObject->m_arrProperties, m_bOldVersion );
    a_streamDest << m_pShaderObject->m_streamClassData;
    a_streamDest << g_32( m_arrShaderElements.GetCount() );
    for ( MIUInt u = 0, ue = m_arrShaderElements.GetCount(); u != ue; ++u )
    {
        SPropertyObject & ShaderElement = m_arrShaderElements[ u ];
        WritePropertyObjectHeader( a_streamDest, ShaderElement.m_strClassName, 0, ShaderElement.m_uVersion, m_bOldVersion );
        MIUInt uShaderElementDataOffset = a_streamDest.Tell();
        WritePropertyData( a_streamDest, ShaderElement.m_arrProperties, m_bOldVersion );
        a_streamDest << ShaderElement.m_streamClassData;
        WriteRelativeOffset( a_streamDest, uShaderElementDataOffset );
    }
    WriteRelativeOffset( a_streamDest, uShaderObjectDataOffset );
    if ( m_bOldVersion )
        WriteRelativeOffset( a_streamDest, uMaterialResourceObjectDataOffset );
    if ( !m_bOldVersion )
    {
        MIUInt uPropertyObjectSize = uShaderObjectDataOffset - uMaterialResourceObjectDataOffset + m_pMaterialResourceObject->m_strClassName.GetLength() - m_pShaderObject->m_strClassName.GetLength();
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
            pMapSource = a_matSource.GetTexMap( mCMaterial::EMapType_Diffuse );
            break;
        case EColorSourceType_Specular:
            pMapSource = a_matSource.GetTexMap( mCMaterial::EMapType_Specular );
            break;
        case EColorSourceType_Normal:
            pMapSource = a_matSource.GetTexMap( mCMaterial::EMapType_Normal );
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
    mCError::CProbe Probe;
    MIBool bResult = ( a_matDest.Load( *pStreamIn ) == mEResult_Ok );
    delete pStreamIn;
    return bResult;
}

void mCGenomeMaterial::LoadMaterials( mCScene & a_sceneDest, mCString const & a_strEx, mCString const & a_strTextureFileExtension )
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
            matSource.GetMaterialData( *pMaterial, a_strTextureFileExtension );
        }
        if ( pMultiMaterial )
        {
            mTArray< mCMaterial > & arrSubMaterials = pMultiMaterial->AccessSubMaterials();
            for ( MIUInt v = arrSubMaterials.GetCount(); v--; )
            {
                mCGenomeMaterial matSource;
                if ( !FindMaterial( arrSubMaterials[ v ].GetName(), matSource, a_strEx ) )
                    continue;
                matSource.GetMaterialData( arrSubMaterials[ v ], a_strTextureFileExtension );
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
    if ( a_uShaderElementIndex == ESpecialShaderElementIndex_MaterialResourceObject )
        return *m_pMaterialResourceObject;
    else if ( a_uShaderElementIndex == ESpecialShaderElementIndex_ShaderObject )
        return *m_pShaderObject;
    return m_arrShaderElements[ a_uShaderElementIndex ];
}

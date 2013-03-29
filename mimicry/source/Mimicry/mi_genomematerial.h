#ifndef MI_GENOMEMATERIAL_H_INCLUDED
#define MI_GENOMEMATERIAL_H_INCLUDED

class mCGenomeMaterial
{
public:
    enum EColorSourceType
    {
        EColorSourceType_Diffuse,
        EColorSourceType_Opacity,
        EColorSourceType_SelfIllumination,
        EColorSourceType_Specular,
        EColorSourceType_SpecularPower,
        EColorSourceType_Normal,
        EColorSourceType_Distortion,
        EColorSourceType_LightEmission,
        EColorSourceType_Count,
        EColorSourceType_Invalid
    };
    enum ESpecialShaderElementIndex
    {
        ESpecialShaderElementIndex_ResourceShaderMaterial = 0xAAAAAAAA,
        ESpecialShaderElementIndex_ShaderDefault          = 0xBBBBBBBB
    };
    typedef void ( * MIFMaterialFileFinderFunction )( MILPCChar a_pcFileName, MILPCChar a_pcFileExt, mCIOStreamBinary *& a_pDest );
    struct SEnumContainer;
    struct SProperty;
public:
    mCGenomeMaterial( void );
    mCGenomeMaterial( mCIOStreamBinary & a_streamSource );
   ~mCGenomeMaterial( void );
public:
    static MIBool & AccessMaterialLookupHint( void );
    static MIBool   FindGothic3Material( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest );
    static MIBool   FindRisenMaterial( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest );
    static void     LoadGothic3Materials( mCScene & a_sceneDest );
    static void     LoadRisenMaterials( mCScene & a_sceneDest );
    static void     RegisterMaterialFileFinderFunction( MIFMaterialFileFinderFunction a_pfuncMaterialFileFinderFunction );
public:
    EColorSourceType  GetColorSourceType( MIUInt a_uShaderElementIndex ) const;
    MIUInt            GetReferencedColorSources( MIUInt a_uShaderElementIndex, MIUInt ( & a_arrShaderElementIndices )[ 3 ] ) const;
    void              GetMaterialData( mCMaterial & a_matDest ) const;
    SProperty const & GetProperty( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const;
    MIUInt            GetPropertyCount( MIUInt a_uShaderElementIndex ) const;
    MIBool            GetPropertyDeep( MIUInt a_uShaderElementIndex, mCString const & a_strPropertyName, mCVariant & a_varDest ) const;
    MIUInt            GetShaderElementCount( void ) const;
    mCString          GetShaderElementType( MIUInt a_uShaderElementIndex ) const;
    void              Invalidate( void );
    MIBool            IsReadOnly( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex ) const;
    MIBool            IsValid( void ) const;
    mEResult          Load( mCIOStreamBinary & a_streamSource );
    mEResult          Save( mCIOStreamBinary & a_streamDest );
    void              SetMaterialData( mCMaterial const & a_matSource );
    void              SetProperty( MIUInt a_uShaderElementIndex, MIUInt a_uPropertyIndex, mCVariant a_varValue );
private:
    struct SPropertyObject;
private:
    mCGenomeMaterial( mCGenomeMaterial const & );
private:
    mCGenomeMaterial & operator = ( mCGenomeMaterial const & );
private:
    static MIBool FindMaterial( mCString const & a_strMaterialName, mCGenomeMaterial & a_matDest, mCString const & a_strExt );
    static void   LoadMaterials( mCScene & a_sceneDest, mCString const & a_strEx );
private:
    SPropertyObject &       AccessShaderElement( MIUInt a_uShaderElementIndex );
    SPropertyObject const & GetShaderElement( MIUInt a_uShaderElementIndex ) const;
private:
    static MIFMaterialFileFinderFunction s_pfuncMaterialFileFinderFunction;
private:
    mCBuffer                   m_bufResourceHeader;
    SPropertyObject *          m_pResourceShaderMaterial;
    SPropertyObject *          m_pShaderDefault;
    mTArray< SPropertyObject > m_arrShaderElements;
    MIBool                     m_bOldVersion;
};

struct mCGenomeMaterial::SEnumContainer
{
    MIUInt            m_uValue;
    MIUInt            m_uCount;
    MILPCChar const * m_pEnumNames;
};

struct mCGenomeMaterial::SProperty
{
    mCString  m_strName;
    mCString  m_strType;
    mCVariant m_varValue;
};

struct mCGenomeMaterial::SPropertyObject
{
    mCString             m_strClassName;
    MIUInt               m_uSize;
    MIUInt               m_uVersion;
    mCBuffer             m_bufClassData;
    mTArray< SProperty > m_arrProperties;
};

#endif

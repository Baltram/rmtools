#ifndef MI_MATERIAL_H_INCLUDED
#define MI_MATERIAL_H_INCLUDED

class mCMaterial :
    public mCMaterialBase
{
public:
    enum EMapType
    {
        EMapType_Diffuse,
        EMapType_Normal,
        EMapType_Specular,
        EMapType_Count,
        EMapType_ForceDWORD = MI_FORCE_DWORD
    };
public:
   virtual ~mCMaterial( void );
public:
    mCMaterial( mCString const & a_strName );
    mCMaterial( mCMaterial const & a_mtlSource );
    mCMaterial( void );
public:
    mCMaterial & operator = ( mCMaterial const & a_mtlSource );
public:
    virtual mCMaterial * Clone( void ) const;
public:
    void             Clear( void );
    mCTexMap const * GetTextureMapAt( EMapType a_enuMapType ) const;
    void             SetTextureMapAt( EMapType a_enuMapType, mCTexMap const * a_pSource );
    void             Swap( mCMaterial & a_mtlOther );
private:
    mCTexMap m_arrMaps[ EMapType_Count ];
    MIU32    m_u32MapStates;
};

#endif
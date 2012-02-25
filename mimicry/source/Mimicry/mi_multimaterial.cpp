#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_scene.h"

mCMultiMaterial::~mCMultiMaterial( void )
{
}

mCMultiMaterial::mCMultiMaterial( mCString const & a_strName ) :
    mCMaterialBase( a_strName )
{
}

mCMultiMaterial::mCMultiMaterial( mCMultiMaterial const & a_mtlSource ) :
    mCMaterialBase( a_mtlSource ),
    m_arrSubMaterials( a_mtlSource.m_arrSubMaterials )
{
}

mCMultiMaterial::mCMultiMaterial( void )
{
}

mCMultiMaterial & mCMultiMaterial::operator = ( mCMultiMaterial const & a_mtlSource )
{
    mCMultiMaterial mtlCopy( a_mtlSource );
    Swap( mtlCopy );
    return *this;
}

mCMaterial & mCMultiMaterial::operator [] ( MIUInt a_uIndex )
{
    return m_arrSubMaterials[ a_uIndex ];
}

mCMaterial const & mCMultiMaterial::operator [] ( MIUInt a_uIndex ) const
{
    return m_arrSubMaterials[ a_uIndex ];
}

mCMultiMaterial * mCMultiMaterial::Clone( void ) const
{
    return new mCMultiMaterial( *this );
}

mTArray< mCMaterial > & mCMultiMaterial::AccessSubMaterials( void )
{
    return m_arrSubMaterials;
}

void mCMultiMaterial::Clear( void )
{
    mCMultiMaterial mtlTemp;
    Swap( mtlTemp );
};

mTArray< mCMaterial > const & mCMultiMaterial::GetSubMaterials( void ) const
{
    return m_arrSubMaterials;
}

void mCMultiMaterial::Swap( mCMultiMaterial & a_mtlOther )
{
    mCMaterialBase::Swap( a_mtlOther );
    m_arrSubMaterials.Swap( a_mtlOther.m_arrSubMaterials );
}


#endif
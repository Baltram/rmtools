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
    if ( this == &a_mtlSource )
        return *this;
    mCMultiMaterial mtlCopy( a_mtlSource );
    Swap( mtlCopy );
    return *this;
}

MIBool mCMultiMaterial::operator == ( mCMultiMaterial const & a_mtlOther ) const
{
    if ( m_arrSubMaterials.GetCount() != m_arrSubMaterials.GetCount() )
        return MIFalse;
    for ( MIUInt u = m_arrSubMaterials.GetCount(); u--; )
        if ( m_arrSubMaterials[ u ] != a_mtlOther.m_arrSubMaterials[ u ] )
            return MIFalse;
    return MITrue;
}

MIBool mCMultiMaterial::operator != ( mCMultiMaterial const & a_mtlOther ) const
{
    return !( *this == a_mtlOther );
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
    if ( this == &a_mtlOther )
        return;
    mCMaterialBase::Swap( a_mtlOther );
    m_arrSubMaterials.Swap( a_mtlOther.m_arrSubMaterials );
}

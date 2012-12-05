#include "mi_include_scene.h"

mCMaterialBase::~mCMaterialBase( void )
{
}

mCMaterialBase::mCMaterialBase( mCString const & a_strName ) :
    m_strName( a_strName )
{
}

mCMaterialBase::mCMaterialBase( mCMaterialBase const & a_mtlSource ) :
    m_strName( a_mtlSource.m_strName )
{
}

mCMaterialBase::mCMaterialBase( void )
{
    static MIUInt s_uCounter = 0;
    m_strName.Format( "Material #%u", ++s_uCounter );
}

mCMaterialBase & mCMaterialBase::operator = ( mCMaterialBase const & a_mtlSource )
{
    if ( this == &a_mtlSource )
        return *this;
    m_strName = a_mtlSource.m_strName;
    return *this;
}

mCString & mCMaterialBase::AccessName( void )
{
    return m_strName;
}

mCString const & mCMaterialBase::GetName( void ) const
{
    return m_strName;
}

void mCMaterialBase::Swap( mCMaterialBase & a_mtlOther )
{
    if ( this == &a_mtlOther )
        return;
    m_strName.Swap( a_mtlOther.m_strName );
}

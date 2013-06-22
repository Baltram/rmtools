#include "mi_include_scene.h"

mCTexMap::mCTexMap( mCString const & a_strName, mCString const & a_strTextureFilePath ) :
    m_strName( a_strName ),
    m_strTextureFilePath( a_strTextureFilePath )
{
}

mCTexMap::mCTexMap( mCTexMap const & a_tmapSource ) :
    m_strName( a_tmapSource.m_strName ),
    m_strTextureFilePath( a_tmapSource.m_strTextureFilePath )
{
}

mCTexMap::mCTexMap( void )
{
}

mCTexMap::~mCTexMap( void )
{
}

mCTexMap & mCTexMap::operator = ( mCTexMap const & a_tmapSource )
{
    if ( this == &a_tmapSource )
        return *this;
    m_strName = a_tmapSource.m_strName;
    m_strTextureFilePath = a_tmapSource.m_strTextureFilePath;
    return *this;
}

mCString & mCTexMap::AccessName( void )
{
    return m_strName;
}

mCString & mCTexMap::AccessTextureFilePath( void )
{
    return m_strTextureFilePath;
}

mCString const & mCTexMap::GetName( void ) const
{
    return m_strName;
}

mCString const & mCTexMap::GetTextureFilePath( void ) const
{
    return m_strTextureFilePath;
}

MIBool mCTexMap::IsEmpty( void ) const
{
    return m_strTextureFilePath.GetLength() == 0;
}

void mCTexMap::Swap( mCTexMap & a_tmapOther )
{
    if ( this == &a_tmapOther )
        return;
    g_swap( m_strName, a_tmapOther.m_strName );
    g_swap( m_strTextureFilePath, a_tmapOther.m_strTextureFilePath );
}

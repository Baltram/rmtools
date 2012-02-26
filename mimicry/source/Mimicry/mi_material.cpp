#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_scene.h"

mCMaterial::~mCMaterial( void )
{
}

mCMaterial::mCMaterial( mCString const & a_strName ) :
    mCMaterialBase( a_strName ),
    m_u32MapStates( 0 )
{
}

mCMaterial::mCMaterial( mCMaterial const & a_mtlSource ) :
    mCMaterialBase( a_mtlSource ),
    m_u32MapStates( a_mtlSource.m_u32MapStates )
{
    for ( MIUInt u = EMapType_Count; u--; m_arrMaps[ u ] = a_mtlSource.m_arrMaps[ u ] );
}

mCMaterial::mCMaterial( void ) :
    m_u32MapStates( 0 )
{
}

mCMaterial & mCMaterial::operator = ( mCMaterial const & a_mtlSource )
{
    if ( this == &a_mtlSource )
        return *this;
    mCMaterial mtlCopy( a_mtlSource );
    Swap( mtlCopy );
    return *this;
}

mCMaterial * mCMaterial::Clone( void ) const
{
    return new mCMaterial( *this );
}

void mCMaterial::Clear( void )
{
    mCMaterial mtlTemp;
    Swap( mtlTemp );
}

mCTexMap const * mCMaterial::GetTextureMapAt( EMapType a_enuMapType ) const
{
    if ( ( a_enuMapType < EMapType_Count ) && ( m_u32MapStates & ( 1 << a_enuMapType ) ) )
        return &m_arrMaps[ a_enuMapType ];
    return 0;
}

void mCMaterial::SetTextureMapAt( EMapType a_enuMapType, mCTexMap const * a_pSource )
{
    if ( ( a_enuMapType < EMapType_Count ) && a_pSource )
    {
        m_u32MapStates |= ( 1 << a_enuMapType );
        m_arrMaps[ a_enuMapType ] = *a_pSource;
    }
    else
    {
        m_u32MapStates &= ~( 1 << a_enuMapType );
    }
}

void mCMaterial::Swap( mCMaterial & a_mtlOther )
{
    if ( this == &a_mtlOther )
        return;
    mCMaterialBase::Swap( a_mtlOther );
    g_swap( m_u32MapStates, a_mtlOther.m_u32MapStates );
    for ( MIUInt u = EMapType_Count; u--; )
        m_arrMaps[ u ].Swap( a_mtlOther.m_arrMaps[ u ] );
}

#endif
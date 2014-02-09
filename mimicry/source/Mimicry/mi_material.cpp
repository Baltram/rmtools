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

MIBool mCMaterial::operator == ( mCMaterial const & a_mtlOther ) const
{
    for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
        if ( HasTexMap( i ) != a_mtlOther.HasTexMap( i ) ||
             HasTexMap( i ) && *GetTexMap( i ) != *a_mtlOther.GetTexMap( i ) )
            return MIFalse;
    return MITrue;
}

MIBool mCMaterial::operator != ( mCMaterial const & a_mtlOther ) const
{
    return !( *this == a_mtlOther );
}

mCTexMap & mCMaterial::AccessTexMap( EMapType a_enuMapType )
{
    m_u32MapStates |= ( 1 << a_enuMapType );
    return m_arrMaps[ a_enuMapType ];
}

void mCMaterial::Clear( void )
{
    mCMaterial mtlTemp;
    Swap( mtlTemp );
}

mCMaterial * mCMaterial::Clone( void ) const
{
    return new mCMaterial( *this );
}

mCTexMap const * mCMaterial::GetTexMap( EMapType a_enuMapType ) const
{
    return HasTexMap( a_enuMapType ) ? &m_arrMaps[ a_enuMapType ] : 0;
}

MIBool mCMaterial::HasTexMap( EMapType a_enuMapType ) const
{
    return ( a_enuMapType < EMapType_Count ) && ( m_u32MapStates & ( 1 << a_enuMapType ) );
}

void mCMaterial::RemoveEmptyTexMaps( void )
{
    for ( EMapType i = EMapType_Diffuse; i != EMapType_Count; ++i )
        if ( AccessTexMap( i ).IsEmpty() )
            RemoveTexMap( i );
}

void mCMaterial::RemoveTexMap( EMapType a_enuMapType )
{
    m_u32MapStates &= ~( 1 << a_enuMapType );
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

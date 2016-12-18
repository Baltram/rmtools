#include "mi_include_3d.h"

mCColor::mCColor( MIU8 a_u8Red, MIU8 a_u8Green, MIU8 a_u8Blue, MIU8 a_u8Alpha ) :
    m_u8Red( a_u8Red ),
    m_u8Green( a_u8Green ),
    m_u8Blue( a_u8Blue ),
    m_u8Alpha( a_u8Alpha )
{
}

mCColor::mCColor( mCColor const & a_colSource ) :
    m_u8Red( a_colSource.m_u8Red ),
    m_u8Green( a_colSource.m_u8Green ),
    m_u8Blue( a_colSource.m_u8Blue ),
    m_u8Alpha( a_colSource.m_u8Alpha )
{
}

mCColor::mCColor( void ) :
    m_u8Red( 255 ),
    m_u8Green( 255 ),
    m_u8Blue( 255 ),
    m_u8Alpha( 255 )
{
}

mCColor::~mCColor( void )
{
}

MIBool mCColor::operator == ( mCColor const & a_colOther ) const
{
    return ( ( m_u8Red == a_colOther.m_u8Red ) &&
             ( m_u8Green == a_colOther.m_u8Green ) &&
             ( m_u8Blue == a_colOther.m_u8Blue ) &&
             ( m_u8Alpha == a_colOther.m_u8Alpha ) );
}

MIBool mCColor::operator != ( mCColor const & a_colOther ) const
{
    return !( *this == a_colOther );
}

mCColor & mCColor::operator = ( mCColor const & a_colSource )
{
    if ( this == &a_colSource )
        return *this;
    m_u8Red = a_colSource.m_u8Red;
    m_u8Green = a_colSource.m_u8Green;
    m_u8Blue = a_colSource.m_u8Blue;
    m_u8Alpha = a_colSource.m_u8Alpha;
    return *this;
}

MIU8 & mCColor::AccessRed( void )
{
    return m_u8Red;
}

MIU8 & mCColor::AccessGreen( void )
{
    return m_u8Green;
}

MIU8 & mCColor::AccessBlue( void )
{
    return m_u8Blue;
}

MIU8 & mCColor::AccessAlpha( void )
{
    return m_u8Alpha;
}

MIU8 mCColor::GetRed( void ) const
{
    return m_u8Red;
}

MIU8 mCColor::GetGreen( void ) const
{
    return m_u8Green;
}

MIU8 mCColor::GetBlue( void ) const
{
    return m_u8Blue;
}

MIU8 mCColor::GetAlpha( void ) const
{
    return m_u8Alpha;
}

void mCColor::Swap( mCColor & a_colOther )
{
    if ( this == &a_colOther )
        return;
    g_swap( m_u8Red, a_colOther.m_u8Red );
    g_swap( m_u8Green, a_colOther.m_u8Green );
    g_swap( m_u8Blue, a_colOther.m_u8Blue );
    g_swap( m_u8Alpha, a_colOther.m_u8Alpha );
}

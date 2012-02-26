#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_3d.h"

mCBox::mCBox( mCVec3 a_vecMin, mCVec3 a_vecMax ) :
    m_vecMin( a_vecMin ),
    m_vecMax( a_vecMax )
{
}

mCBox::mCBox( mCBox const & a_boxSource ) :
    m_vecMin( a_boxSource.m_vecMin ),
    m_vecMax( a_boxSource.m_vecMax )
{
}

mCBox::mCBox( void ) :
    m_vecMin( s_vecPosInfinity ),
    m_vecMax( s_vecNegInfinity )
{
}

mCBox::~mCBox( void )
{
}

MIBool mCBox::operator == ( mCBox const & a_boxOther ) const
{
    return ( ( m_vecMin == a_boxOther.m_vecMin ) && ( m_vecMax == a_boxOther.m_vecMax ) );
}

MIBool mCBox::operator != ( mCBox const & a_boxOther ) const
{
    return !( *this == a_boxOther );
}

mCBox mCBox::operator * ( MIFloat a_fFactor ) const
{
    mCBox boxResult( *this );
    boxResult *= a_fFactor;
    return boxResult;
}

mCBox mCBox::operator - ( mCVec3 const & a_vecVector ) const
{
    mCBox boxResult( *this );
    boxResult -= a_vecVector;
    return boxResult;
}

mCBox mCBox::operator + ( mCVec3 const & a_vecVector ) const
{
    mCBox boxResult( *this );
    boxResult += a_vecVector;
    return boxResult;
}

mCBox mCBox::operator / ( MIFloat a_fFactor ) const
{
    mCBox boxResult( *this );
    boxResult /= a_fFactor;
    return boxResult;
}

mCBox mCBox::operator & ( mCBox const & a_boxBox ) const
{
    mCBox boxResult( *this );
    boxResult &= a_boxBox;
    return boxResult;
}

mCBox mCBox::operator | ( mCBox const & a_boxBox ) const
{
    mCBox boxResult( *this );
    boxResult |= a_boxBox;
    return boxResult;
}

mCBox mCBox::operator | ( mCVec3 const & a_vecVector ) const
{
    mCBox boxResult( *this );
    boxResult |= a_vecVector;
    return boxResult;
}

mCBox & mCBox::operator *= ( MIFloat a_fFactor )
{
    m_vecMin *= a_fFactor;
    m_vecMax *= a_fFactor;
    return *this;
}

mCBox & mCBox::operator += ( mCVec3 const & a_vecVector )
{
    m_vecMin += a_vecVector;
    m_vecMax += a_vecVector;
    return *this;
}

mCBox & mCBox::operator -= ( mCVec3 const & a_vecVector )
{
    m_vecMin -= a_vecVector;
    m_vecMax -= a_vecVector;
    return *this;
}

mCBox & mCBox::operator /= ( MIFloat a_fFactor )
{
    m_vecMin /= a_fFactor;
    m_vecMax /= a_fFactor;
    return *this;
}

mCBox & mCBox::operator &= ( mCBox const & a_boxBox )
{
    if ( IsValid() && a_boxBox.IsValid() )
    {
        m_vecMin[ 0 ] = g_max( m_vecMin[ 0 ], a_boxBox.m_vecMin[ 0 ] );
        m_vecMin[ 1 ] = g_max( m_vecMin[ 1 ], a_boxBox.m_vecMin[ 1 ] );
        m_vecMin[ 2 ] = g_max( m_vecMin[ 2 ], a_boxBox.m_vecMin[ 2 ] );
        m_vecMax[ 0 ] = g_min( m_vecMax[ 0 ], a_boxBox.m_vecMax[ 0 ] );
        m_vecMax[ 1 ] = g_min( m_vecMax[ 1 ], a_boxBox.m_vecMax[ 1 ] );
        m_vecMax[ 2 ] = g_min( m_vecMax[ 2 ], a_boxBox.m_vecMax[ 2 ] );
        if ( ( m_vecMin[ 0 ] > m_vecMax[ 0 ] ) || ( m_vecMin[ 1 ] > m_vecMax[ 1 ] ) || ( m_vecMin[ 2 ] > m_vecMax[ 2 ] ) )
            Invalidate();
    }
    return *this;
}

mCBox & mCBox::operator |= ( mCBox const & a_boxBox )
{
    if ( IsValid() && a_boxBox.IsValid() )
    {
        m_vecMin[ 0 ] = g_min( m_vecMin[ 0 ], a_boxBox.m_vecMin[ 0 ] );
        m_vecMin[ 1 ] = g_min( m_vecMin[ 1 ], a_boxBox.m_vecMin[ 1 ] );
        m_vecMin[ 2 ] = g_min( m_vecMin[ 2 ], a_boxBox.m_vecMin[ 2 ] );
        m_vecMax[ 0 ] = g_max( m_vecMax[ 0 ], a_boxBox.m_vecMax[ 0 ] );
        m_vecMax[ 1 ] = g_max( m_vecMax[ 1 ], a_boxBox.m_vecMax[ 1 ] );
        m_vecMax[ 2 ] = g_max( m_vecMax[ 2 ], a_boxBox.m_vecMax[ 2 ] );
    }
    return *this;
}

mCBox & mCBox::operator |= ( mCVec3 const & a_vecVector )
{
    if ( IsValid() )
    {
        m_vecMin[ 0 ] = g_min( m_vecMin[ 0 ], a_vecVector[ 0 ] );
        m_vecMin[ 1 ] = g_min( m_vecMin[ 1 ], a_vecVector[ 1 ] );
        m_vecMin[ 2 ] = g_min( m_vecMin[ 2 ], a_vecVector[ 2 ] );
        m_vecMax[ 0 ] = g_max( m_vecMax[ 0 ], a_vecVector[ 0 ] );
        m_vecMax[ 1 ] = g_max( m_vecMax[ 1 ], a_vecVector[ 1 ] );
        m_vecMax[ 2 ] = g_max( m_vecMax[ 2 ], a_vecVector[ 2 ] );
    }
    return *this;
}

mCBox & mCBox::operator = ( mCBox const & a_boxSource )
{
    if ( this == &a_boxSource )
        return *this;
    m_vecMin = a_boxSource.m_vecMin;
    m_vecMax = a_boxSource.m_vecMax;
    return *this;
}

mCVec3 & mCBox::AccessMax( void )
{
    return m_vecMax;
}

mCVec3 & mCBox::AccessMin( void )
{
    return m_vecMin;
}

MIBool mCBox::Contains( mCBox const & a_boxBox ) const
{
    return ( IsValid() && ( ( *this | a_boxBox ) == *this ) );
}

MIBool mCBox::Contains( mCVec3 const & a_vecVector ) const
{
    return ( IsValid() && ( ( *this | a_vecVector ) == *this ) );
}

mCVec3 mCBox::GetCenter( void ) const
{
    return mCVec3( ( m_vecMin[ 0 ] + m_vecMax[ 0 ] ) / 2, ( m_vecMin[ 1 ] + m_vecMax[ 1 ] ) / 2, ( m_vecMin[ 2 ] + m_vecMax[ 2 ] ) / 2 );
}

MIFloat mCBox::GetHeight( void ) const
{
    return m_vecMax[ 2 ] - m_vecMin[ 2 ];
}

MIFloat mCBox::GetLength( void ) const
{
    return m_vecMax[ 0 ] - m_vecMin[ 0 ];
}

mCVec3 const & mCBox::GetMax( void ) const
{
    return m_vecMax;
}

mCVec3 const & mCBox::GetMin( void ) const
{
    return m_vecMin;
}

MIFloat mCBox::GetWidth( void ) const
{
    return m_vecMax[ 1 ] - m_vecMin[ 1 ];
}

MIBool mCBox::Intersects( mCBox const & a_boxBox ) const
{
    return ( *this & a_boxBox ).IsValid();
}

void mCBox::Invalidate( void )
{
    m_vecMin = s_vecPosInfinity;
    m_vecMax = s_vecNegInfinity;
}

MIBool mCBox::IsValid( void ) const
{
    return ( ( m_vecMin != s_vecPosInfinity ) || ( m_vecMax != s_vecNegInfinity ) );
}

void mCBox::SetTo( mCVec3 const & a_vecMin, mCVec3 const & a_vecMax )
{
    m_vecMin = a_vecMin;
    m_vecMax = a_vecMax;
}

mCVec3 const mCBox::s_vecPosInfinity( g_fPosInfinity, g_fPosInfinity, g_fPosInfinity );
mCVec3 const mCBox::s_vecNegInfinity( g_fNegInfinity, g_fNegInfinity, g_fNegInfinity );

#endif
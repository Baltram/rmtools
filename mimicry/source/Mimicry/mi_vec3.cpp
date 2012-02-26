#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_standard.h"
#include "mi_include_streams.h"
#include "mi_include_3d.h"

mCVec3::mCVec3( MIFloat a_fX, MIFloat a_fY, MIFloat a_fZ ) :
    m_fX( a_fX ),
    m_fY( a_fY ),
    m_fZ( a_fZ )
{
}

mCVec3::mCVec3( mCVec3 const & a_vecSource ) :
    m_fX( a_vecSource.m_fX ),
    m_fY( a_vecSource.m_fY ),
    m_fZ( a_vecSource.m_fZ )
{
}

mCVec3::mCVec3( void ) :
    m_fX( 0.0f ),
    m_fY( 0.0f ),
    m_fZ( 0.0f )
{
}

mCVec3::~mCVec3( void )
{
}

MIBool mCVec3::operator == ( mCVec3 const & a_vecOther ) const
{
    return ( ( m_fX == a_vecOther.m_fX ) && ( m_fY == a_vecOther.m_fY ) && ( m_fZ == a_vecOther.m_fZ ) );
}

MIBool mCVec3::operator != ( mCVec3 const & a_vecOther ) const
{
    return !( *this == a_vecOther );
}

MIFloat & mCVec3::operator [] ( MIUInt a_uCoordIndex )
{
    return m_arrCoordinates[ a_uCoordIndex ];
}

MIFloat const & mCVec3::operator [] ( MIUInt a_uCoordIndex ) const
{
    return m_arrCoordinates[ a_uCoordIndex ];
}

mCVec3 mCVec3::operator * ( MIFloat a_fFactor ) const
{
    mCVec3 vecResult( *this );
    vecResult *= a_fFactor;
    return vecResult;
}

mCVec3 mCVec3::operator / ( MIFloat a_fFactor ) const
{
    mCVec3 vecResult( *this );
    vecResult /= a_fFactor;
    return vecResult;
}

mCVec3 mCVec3::operator - ( mCVec3 const & a_vecVector ) const
{
    mCVec3 vecResult( *this );
    vecResult -= a_vecVector;
    return vecResult;
}

mCVec3 mCVec3::operator + ( mCVec3 const & a_vecVector ) const
{
    mCVec3 vecResult( *this );
    vecResult += a_vecVector;
    return vecResult;
}

mCVec3 & mCVec3::operator *= ( MIFloat a_fFactor )
{
    m_fX *= a_fFactor;
    m_fY *= a_fFactor;
    m_fZ *= a_fFactor;
    return *this;
}

mCVec3 & mCVec3::operator /= ( MIFloat a_fFactor )
{
    if ( a_fFactor != 0.0f )
    {
        m_fX /= a_fFactor;
        m_fY /= a_fFactor;
        m_fZ /= a_fFactor;
    }
    return *this;
}

mCVec3 & mCVec3::operator += ( mCVec3 const & a_vecVector )
{
    m_fX += a_vecVector.m_fX;
    m_fY += a_vecVector.m_fY;
    m_fZ += a_vecVector.m_fZ;
    return *this;
}

mCVec3 & mCVec3::operator -= ( mCVec3 const & a_vecVector )
{
    m_fX -= a_vecVector.m_fX;
    m_fY -= a_vecVector.m_fY;
    m_fZ -= a_vecVector.m_fZ;
    return *this;
}

mCVec3 & mCVec3::operator = ( mCVec3 const & a_vecSource )
{
    if ( this == &a_vecSource )
        return *this;
    m_fX = a_vecSource.m_fX;
    m_fY = a_vecSource.m_fY;
    m_fZ = a_vecSource.m_fZ;
    return *this;
}

mCVec3 mCVec3::CrossProduct( mCVec3 const & a_vecFirst, mCVec3 const & a_vecSecond )
{
    return mCVec3( ( a_vecFirst.m_fY * a_vecSecond.m_fZ - a_vecFirst.m_fZ * a_vecSecond.m_fY ), 
                   ( a_vecFirst.m_fZ * a_vecSecond.m_fX - a_vecFirst.m_fX * a_vecSecond.m_fZ ), 
                   ( a_vecFirst.m_fX * a_vecSecond.m_fY - a_vecFirst.m_fY * a_vecSecond.m_fX ) );
}

MIFloat mCVec3::DotProduct( mCVec3 const & a_vecFirst, mCVec3 const & a_vecSecond )
{
    return ( a_vecFirst.m_fX * a_vecSecond.m_fX ) + ( a_vecFirst.m_fY * a_vecSecond.m_fY ) + ( a_vecFirst.m_fZ * a_vecSecond.m_fZ );
}

MIFloat & mCVec3::AccessX( void )
{
    return m_fX;
}

MIFloat & mCVec3::AccessY( void )
{
    return m_fY;
}

MIFloat & mCVec3::AccessZ( void )
{
    return m_fZ;
}

MIFloat mCVec3::CalcAngleDeg( mCVec3 const & a_vecOther, MIBool a_bAlreadyNormalized ) const
{
    return CalcAngleRad( a_vecOther, a_bAlreadyNormalized ) * 180.0f / g_fPi;
}

MIFloat mCVec3::CalcAngleRad( mCVec3 const & a_vecOther, MIBool a_bAlreadyNormalized ) const
{
    MIFloat fDotProduct = DotProduct( *this, a_vecOther );
    if ( !a_bAlreadyNormalized )
        fDotProduct /= ( CalcMagnitude() * a_vecOther.CalcMagnitude() );
    return acos( fDotProduct );
}

mCVec3 mCVec3::CalcCrossProduct( mCVec3 const & a_vecSecond ) const
{
    return CrossProduct( *this, a_vecSecond );
}

MIFloat mCVec3::CalcDotProduct( mCVec3 const & a_vecSecond ) const
{
    return DotProduct( *this, a_vecSecond );
}

MIFloat mCVec3::CalcMagnitude( void ) const
{
    return sqrt( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ );
}

mCVec3 mCVec3::CalcNormalized( void ) const
{
    return *this / CalcMagnitude();
}

void mCVec3::Clear( void )
{
    m_fX = 0.0f;
    m_fY = 0.0f;
    m_fZ = 0.0f;
}

MIFloat mCVec3::GetX( void ) const
{
    return m_fX;
}

MIFloat mCVec3::GetY( void ) const
{
    return m_fY;
}

MIFloat mCVec3::GetZ( void ) const
{
    return m_fZ;
}

MIBool mCVec3::IsSimilar( mCVec3 const & a_vecOther, MIFloat a_fCoordTolerance, MIBool a_bIgnoreZ ) const
{
    return ( ( fabs( m_fX - a_vecOther.m_fX ) <= a_fCoordTolerance ) &&
             ( fabs( m_fY - a_vecOther.m_fY ) <= a_fCoordTolerance ) &&
             ( a_bIgnoreZ || ( fabs( m_fZ - a_vecOther.m_fZ ) <= a_fCoordTolerance ) ) );
}

MIBool mCVec3::IsZero( void ) const
{
    return ( ( m_fX == 0.0f ) && ( m_fY == 0.0f ) && ( m_fZ == 0.0f ) );
}

mCVec3 const & mCVec3::Normalize( void )
{
    *this /= CalcMagnitude();
    return *this;
}

void mCVec3::Swap( mCVec3 & a_vecOther )
{
    if ( this == &a_vecOther )
        return;
    g_swap( m_fX, a_vecOther.m_fX );
    g_swap( m_fY, a_vecOther.m_fY );
    g_swap( m_fZ, a_vecOther.m_fZ );
}

mCIOStreamBinary & operator >> ( mCIOStreamBinary & a_streamSource, mCVec3 & a_vecDest )
{
    a_streamSource >> a_vecDest.AccessX();
    a_streamSource >> a_vecDest.AccessY();
    a_streamSource >> a_vecDest.AccessZ();
    return a_streamSource;
}

mCIOStreamBinary & operator << ( mCVec3 & a_vecDest, mCIOStreamBinary & a_streamSource )
{
    return ( a_streamSource >> a_vecDest );
}

mCIOStreamBinary & operator << ( mCIOStreamBinary & a_streamDest, mCVec3 const & a_vecSource )
{
    a_streamDest << a_vecSource.GetX();
    a_streamDest << a_vecSource.GetY();
    a_streamDest << a_vecSource.GetZ();
    return a_streamDest;
}

mCIOStreamBinary & operator >> ( mCVec3 const & a_vecSource, mCIOStreamBinary & a_streamDest )
{
    return ( a_streamDest << a_vecSource );
}

#endif
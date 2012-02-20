#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_3d.h"

mCFace::mCFace( MIUInt a_uA, MIUInt a_uB, MIUInt a_uC ) :
    m_uA( a_uA ),
    m_uB( a_uB ),
    m_uC( a_uC )
{
}

mCFace::mCFace( mCFace const & a_faceSource ) :
    m_uA( a_faceSource.m_uA ),
    m_uB( a_faceSource.m_uB ),
    m_uC( a_faceSource.m_uC )
{
}

mCFace::mCFace( void ) :
    m_uA( 0 ),
    m_uB( 0 ),
    m_uC( 0 )
{
}

mCFace::~mCFace( void )
{
}

MIBool mCFace::operator == ( mCFace const & a_faceOther ) const
{
    return ( ( m_uA == a_faceOther.m_uA ) && ( m_uB == a_faceOther.m_uB ) && ( m_uC == a_faceOther.m_uC ) );
}

MIBool mCFace::operator != ( mCFace const & a_faceOther ) const
{
    return !( *this == a_faceOther );
}

MIUInt & mCFace::operator [] ( MIUInt a_uCoordIndex )
{
    return m_arrIndices[ a_uCoordIndex ];
}

MIUInt const & mCFace::operator [] ( MIUInt a_uCoordIndex ) const
{
    return m_arrIndices[ a_uCoordIndex ];
}

mCFace & mCFace::operator =  ( mCFace const & a_faceSource )
{
    m_uA = a_faceSource.m_uA;
    m_uB = a_faceSource.m_uB;
    m_uC = a_faceSource.m_uC;
    return *this;
}

MIUInt & mCFace::AccessA( void )
{
    return m_uA;
}

MIUInt & mCFace::AccessB( void )
{
    return m_uB;
}

MIUInt & mCFace::AccessC( void )
{
    return m_uC;
}

mCVec3 mCFace::CalcNormal( mCVec3 const * a_pVertices, MIUInt a_uVertexCount, MIFloat a_arrAngles[ 3 ] ) const
{
    if ( ( m_uA >= a_uVertexCount ) || ( m_uB >= a_uVertexCount ) || ( m_uC >= a_uVertexCount ) )
        return mCVec3();
    mCVec3 const vecAB = a_pVertices[ m_uB ] - a_pVertices[ m_uA ];
    mCVec3 const vecAC = a_pVertices[ m_uC ] - a_pVertices[ m_uA ];
    if ( a_arrAngles )
    {
        a_arrAngles[ 0 ] = vecAB.CalcAngleRad( vecAC );
        a_arrAngles[ 1 ] = vecAB.CalcAngleRad( a_pVertices[ m_uB ] - a_pVertices[ m_uC ] );
        a_arrAngles[ 2 ] = g_fPi - a_arrAngles[ 0 ] - a_arrAngles[ 1 ];
    }
    return mCVec3::CrossProduct( vecAB, vecAC ).Normalize();
}

MIUInt mCFace::GetA( void ) const
{
    return m_uA;
}

MIUInt mCFace::GetB( void ) const
{
    return m_uB;
}

MIUInt mCFace::GetC( void ) const
{
    return m_uC;
}

void mCFace::Invert( void )
{
    g_swap( m_uA, m_uC );
}

void mCFace::Swap( mCFace & a_faceOther )
{
    g_swap( m_uA, a_faceOther.m_uA );
    g_swap( m_uB, a_faceOther.m_uB );
    g_swap( m_uC, a_faceOther.m_uC );
}

mCIOStreamBinary & operator >> ( mCIOStreamBinary & a_streamSource, mCFace & a_faceDest )
{
    a_streamSource >> g_32( a_faceDest.AccessA() );
    a_streamSource >> g_32( a_faceDest.AccessB() );
    a_streamSource >> g_32( a_faceDest.AccessC() );
    return a_streamSource;
}

mCIOStreamBinary & operator << ( mCFace & a_faceDest, mCIOStreamBinary & a_streamSource )
{
    return ( a_streamSource >> a_faceDest );
}

mCIOStreamBinary & operator << ( mCIOStreamBinary & a_streamDest, mCFace const & a_faceSource )
{
    a_streamDest << g_32( a_faceSource.GetA() );
    a_streamDest << g_32( a_faceSource.GetB() );
    a_streamDest << g_32( a_faceSource.GetB() );
    return a_streamDest;
}

mCIOStreamBinary & operator >> ( mCFace const & a_faceSource, mCIOStreamBinary & a_streamDest )
{
    return ( a_streamDest << a_faceSource );
}

#endif
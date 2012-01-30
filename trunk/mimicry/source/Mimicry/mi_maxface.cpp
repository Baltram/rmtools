#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_3d.h"

mCMaxFace::mCMaxFace( MIUInt a_uA, MIUInt a_uB, MIUInt a_uC, MIUInt a_uMatID, MIU32 a_u32SGs ) :
    mCFace( a_uA, a_uB, a_uC ),
    m_uMatID( a_uMatID ),
    m_u32SGs( a_u32SGs )
{
}

mCMaxFace::mCMaxFace( mCMaxFace const & a_faceSource ) :
    mCFace( a_faceSource ),
    m_uMatID( a_faceSource.m_uMatID ),
    m_u32SGs( a_faceSource.m_u32SGs )
{
}

mCMaxFace::mCMaxFace( void ) :
    mCFace(),
    m_uMatID( 0 ),
    m_u32SGs( 0 )
{
}

mCMaxFace::~mCMaxFace( void )
{
}

MIBool mCMaxFace::operator == ( mCMaxFace const & a_faceOther ) const
{
    return ( ( mCFace::operator == ( a_faceOther ) ) &&
             ( m_uMatID == a_faceOther.m_uMatID ) &&
             ( m_u32SGs == a_faceOther.m_u32SGs ) );
}

MIBool mCMaxFace::operator != ( mCMaxFace const & a_faceOther ) const
{
    return !( *this == a_faceOther );
}

mCMaxFace & mCMaxFace::operator = ( mCMaxFace const & a_faceSource )
{
    mCFace::operator = ( a_faceSource );
    m_uMatID = a_faceSource.m_uMatID;
    m_u32SGs = a_faceSource.m_u32SGs;
    return *this;
}

MIUInt & mCMaxFace::AccessMatID( void )
{
    return m_uMatID;
}

MIU32 & mCMaxFace::AccessSGs( void )
{
    return m_u32SGs;
}

MIUInt mCMaxFace::GetMatID( void ) const
{
    return m_uMatID;
}

MIU32 mCMaxFace::GetSGs( void ) const
{
    return m_u32SGs;
}

void mCMaxFace::Swap( mCMaxFace & a_faceOther )
{
    mCFace::Swap( a_faceOther );
    g_swap( m_uMatID, a_faceOther.m_uMatID );
    g_swap( m_u32SGs, a_faceOther.m_u32SGs );
}


#endif
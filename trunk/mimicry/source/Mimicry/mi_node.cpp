#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_scene.h"

mCNode::mCNode( mCString const & a_strName, mCVec3 a_vecPosition, mCString const & a_strMaterialName, mCMesh const * a_pMesh, mCSkin const * a_pSkin ) :
    m_strName( a_strName ),
    m_strMaterialName( a_strMaterialName ),
    m_vecPosition( a_vecPosition ),
    m_pMesh( a_pMesh ? new mCMesh( *a_pMesh ) : 0 ),
    m_pSkin( a_pSkin ? new mCSkin( *a_pSkin ) : 0 )
{
}

mCNode::mCNode( mCNode const & a_nodeSource ) :
    m_strName( a_nodeSource.m_strName ),
    m_strMaterialName( a_nodeSource.m_strMaterialName ),
    m_vecPosition( a_nodeSource.m_vecPosition ),
    m_pMesh( a_nodeSource.m_pMesh ? new mCMesh( *a_nodeSource.m_pMesh ) : 0 ),
    m_pSkin( a_nodeSource.m_pSkin ? new mCSkin( *a_nodeSource.m_pSkin ) : 0 )
{
}

mCNode::mCNode( void ) :
    m_pMesh( 0 ),
    m_pSkin( 0 )
{
}

mCNode::~mCNode( void )
{
    delete m_pMesh;
    delete m_pSkin;
}

mCNode & mCNode::operator = ( mCNode const & a_nodeSource )
{
    mCNode nodeCopy( a_nodeSource );
    Swap( nodeCopy );
    return *this;
}

mCString & mCNode::AccessMaterialName( void )
{
    return m_strMaterialName;
}

mCString & mCNode::AccessName( void )
{
    return m_strName;
}

mCVec3 & mCNode::AccessPosition( void )
{
    return m_vecPosition;
}

mCString const & mCNode::GetMaterialName( void ) const
{
    return m_strMaterialName;
}

mCMesh const * mCNode::GetMesh( void ) const
{
    return m_pMesh;
}

mCString const & mCNode::GetName( void ) const
{
    return m_strName;
}

mCSkin const * mCNode::GetSkin( void ) const
{
    return m_pSkin;
}

mCVec3 const & mCNode::GetPosition( void ) const
{
    return m_vecPosition;
}

MIBool mCNode::HasMesh( void )
{
    return ( m_pMesh != 0 );
}

MIBool mCNode::HasSkin( void )
{
    return ( m_pSkin != 0 );
}

void mCNode::SetMesh( mCMesh const * a_pMesh )
{
    delete m_pMesh;
    m_pMesh = 0;
    if ( a_pMesh )
        m_pMesh = new mCMesh( *a_pMesh );
}

void mCNode::SetSkin( mCSkin const * a_pSkin )
{
    delete m_pSkin;
    m_pSkin = 0;
    if ( a_pSkin )
        m_pSkin = new mCSkin( *a_pSkin );
}

void mCNode::Swap( mCNode & a_nodeOther )
{
    m_strName.Swap( a_nodeOther.m_strName );
    m_strMaterialName.Swap( a_nodeOther.m_strMaterialName );
    m_vecPosition.Swap( a_nodeOther.m_vecPosition );
    g_swap( m_pMesh, a_nodeOther.m_pMesh );
    g_swap( m_pSkin, a_nodeOther.m_pSkin );
}

void mCNode::SwapMesh( mCMesh & a_meshOther )
{
    if ( !m_pMesh )
        m_pMesh = new mCMesh;
    m_pMesh->Swap( a_meshOther );
}

void mCNode::SwapSkin( mCSkin & a_skinOther )
{
    if ( !m_pSkin )
        m_pSkin = new mCSkin;
    m_pSkin->Swap( a_skinOther );
}

#endif
#include "mi_include_scene.h"

mCNode::mCNode( mCString const & a_strName, mCMatrix4 const & a_matTransform, mCString const & a_strMaterialName, mCMesh const * a_pMesh, mCSkin const * a_pSkin ) :
    m_strName( a_strName ),
    m_strMaterialName( a_strMaterialName ),
    m_matTransform( a_matTransform ),
    m_pMesh( a_pMesh ? new mCMesh( *a_pMesh ) : 0 ),
    m_pSkin( a_pSkin ? new mCSkin( *a_pSkin ) : 0 ),
    m_idParent( 0 )
{
}

mCNode::mCNode( mCNode const & a_nodeSource ) :
    m_strName( a_nodeSource.m_strName ),
    m_strMaterialName( a_nodeSource.m_strMaterialName ),
    m_matTransform( a_nodeSource.m_matTransform ),
    m_pMesh( a_nodeSource.m_pMesh ? new mCMesh( *a_nodeSource.m_pMesh ) : 0 ),
    m_pSkin( a_nodeSource.m_pSkin ? new mCSkin( *a_nodeSource.m_pSkin ) : 0 ),
    m_idParent( a_nodeSource.m_idParent )
{
}

mCNode::mCNode( void ) :
    m_pMesh( 0 ),
    m_pSkin( 0 ),
    m_idParent( 0 )
{
    m_matTransform.SetToIdentity();
}

mCNode::~mCNode( void )
{
    delete m_pMesh;
    delete m_pSkin;
}

mCNode & mCNode::operator = ( mCNode const & a_nodeSource )
{
    if ( this == &a_nodeSource )
        return *this;
    mCNode nodeCopy( a_nodeSource );
    Swap( nodeCopy );
    return *this;
}

mCString & mCNode::AccessMaterialName( void )
{
    return m_strMaterialName;
}

mCMesh * mCNode::AccessMesh( void )
{
    return m_pMesh;
}

mCString & mCNode::AccessName( void )
{
    return m_strName;
}

mCVec3 & mCNode::AccessPosition( void )
{
    return m_matTransform.AccessTranslation();
}

mCMatrix4 & mCNode::AccessTransform( void )
{
    return m_matTransform;
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
    return m_matTransform.GetTranslation();
}

mCMatrix4 const & mCNode::GetTransform( void ) const
{
    return m_matTransform;
}

MIBool mCNode::HasMesh( void ) const
{
    return ( m_pMesh != 0 );
}

MIBool mCNode::HasSkin( void ) const
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
    if ( this == &a_nodeOther )
        return;
    m_strName.Swap( a_nodeOther.m_strName );
    m_strMaterialName.Swap( a_nodeOther.m_strMaterialName );
    m_matTransform.Swap( a_nodeOther.m_matTransform );
    g_swap( m_pMesh, a_nodeOther.m_pMesh );
    g_swap( m_pSkin, a_nodeOther.m_pSkin );
    g_swap( m_idParent, a_nodeOther.m_idParent );
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

mCUnique::ID & mCNode::AccessParentID( void )
{
    return m_idParent;
}

mCUnique::ID const & mCNode::GetParentID( void ) const
{
    return m_idParent;
}
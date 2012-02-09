#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_scene.h"

mCScene::mCScene( mCScene const & a_sceneSource ) :
    m_strName( a_sceneSource.m_strName ),
    m_arrMaterials( a_sceneSource.m_arrMaterials ),
    m_arrNodes( a_sceneSource.m_arrNodes )
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; m_arrMaterials[ u ] = m_arrMaterials[ u ]->Clone() );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; m_arrNodes[ u ] = new mCNode( *m_arrNodes[ u ] ) );
}

mCScene::mCScene( void )
{
}

mCScene::~mCScene( void )
{
    Clear();
}

mCScene & mCScene::operator = ( mCScene const & a_sceneSource )
{
    mCScene sceneCopy( a_sceneSource );
    Swap( sceneCopy );
    return *this;
}

mCMaterialBase * mCScene::AccessMaterialAt( MIUInt a_uIndex )
{
    return m_arrMaterials[ a_uIndex ];
}

mCNode * mCScene::AccessNodeAt( MIUInt a_uIndex )
{
    return m_arrNodes[ a_uIndex ];
}

void mCScene::AddMaterial( mCMaterialBase const & a_matSource )
{
    m_arrMaterials.Add( a_matSource.Clone() );
}

mCMaterial & mCScene::AddNewMaterial( void )
{
    return *dynamic_cast< mCMaterial * >( m_arrMaterials.AddNew() = new mCMaterial );
}

mCMultiMaterial & mCScene::AddNewMultiMaterial( void )
{
    return *dynamic_cast< mCMultiMaterial * >( m_arrMaterials.AddNew() = new mCMultiMaterial );
}

mCNode & mCScene::AddNewNode( void )
{
    return *( m_arrNodes.AddNew() = new mCNode );
}

void mCScene::AddNode( mCNode const & a_nodeSource )
{
    m_arrNodes.Add( new mCNode( a_nodeSource ) );
}

void mCScene::Clear( void )
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; delete m_arrMaterials[ u ] );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; delete m_arrNodes[ u ] );
    m_arrMaterials.Clear();
    m_arrNodes.Clear();
    m_strName.Clear();
}

void mCScene::ClearNodeParent( MIUInt a_uNodeIndex )
{
    AccessNodeAt( a_uNodeIndex )->AccessParentID() = 0;
}

mCMaterialBase const * mCScene::GetMaterialAt( MIUInt a_uIndex ) const
{
    return m_arrMaterials[ a_uIndex ];
}

MIUInt mCScene::GetMaterialIndexByName( mCString a_strMaterialName ) const
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; )
        if ( m_arrMaterials[ u ]->GetName() == a_strMaterialName )
            return u;
    return MI_DW_INVALID;
}

mCString const & mCScene::GetName( void ) const
{
    return m_strName;
}

mCNode const * mCScene::GetNodeAt( MIUInt a_uIndex ) const
{
    return m_arrNodes[ a_uIndex ];
}

MIUInt mCScene::GetNodeIndexByID( mCUnique::ID a_ID ) const
{
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
        if ( m_arrNodes[ u ]->GetID() == a_ID )
            return u;
    return MI_DW_INVALID;
}

MIUInt mCScene::GetNodeIndexByName( mCString a_strNodeName ) const
{
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
        if ( m_arrNodes[ u ]->GetName() == a_strNodeName )
            return u;
    return MI_DW_INVALID;
}

MIUInt mCScene::GetNodeParentIndex( MIUInt a_uNodeIndex ) const
{
    return GetNodeIndexByID( GetNodeAt( a_uNodeIndex )->GetParentID() );
}
 
MIUInt mCScene::GetNumMaterials( void ) const
{
    return m_arrMaterials.GetCount();
}

MIUInt mCScene::GetNumNodes( void ) const
{
    return m_arrNodes.GetCount();
}

void mCScene::RemoveMaterial( mCMaterialBase * a_pMaterial )
{
    MIUInt const uIndex = m_arrMaterials.IndexOf( a_pMaterial );
    if ( uIndex == MI_DW_INVALID )
        return;
    delete m_arrMaterials[ uIndex ];
    m_arrMaterials.RemoveAt( uIndex );
}

void mCScene::RemoveNode( mCNode * a_pNode )
{
    MIUInt const uIndex = m_arrNodes.IndexOf( a_pNode );
    if ( uIndex == MI_DW_INVALID )
        return;
    delete m_arrNodes[ uIndex ];
    m_arrNodes.RemoveAt( uIndex );
}

void mCScene::SetName( mCString const & a_strName )
{
    m_strName = a_strName;
}

void mCScene::SetNodeParent( MIUInt a_uNodeIndex, MIUInt a_uparentNodeIndex )
{
    AccessNodeAt( a_uNodeIndex )->AccessParentID() = GetNodeAt( a_uparentNodeIndex )->GetID();
}

void mCScene::Swap( mCScene & a_sceneOther )
{
    m_strName.Swap( a_sceneOther.m_strName );
    m_arrMaterials.Swap( a_sceneOther.m_arrMaterials );
    m_arrNodes.Swap( a_sceneOther.m_arrNodes );
}

#endif
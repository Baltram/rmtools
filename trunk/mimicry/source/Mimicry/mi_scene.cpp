#include "mi_include_scene.h"

mCScene::mCScene( mCScene const & a_sceneSource ) :
    m_strName( a_sceneSource.m_strName ),
    m_arrMaterials( a_sceneSource.m_arrMaterials ),
    m_arrNodes( a_sceneSource.m_arrNodes )
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; m_arrMaterials[ u ] = m_arrMaterials[ u ]->Clone() );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; m_arrNodes[ u ] = new mCNode( *m_arrNodes[ u ] ) );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
        if ( m_arrNodes[ u ]->HasSkin() )
            m_arrNodes[ u ]->AccessSkin()->MigrateBoneIDs( *this, a_sceneSource );
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
    if ( this == &a_sceneSource )
        return *this;
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

void mCScene::AddMaterial( mCMaterialBase const & a_mtlSource )
{
    m_arrMaterials.Add( a_mtlSource.Clone() );
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
    if ( a_uIndex >= GetNumMaterials() )
        return 0;
    return m_arrMaterials[ a_uIndex ];
}

MIUInt mCScene::GetMaterialIndexByName( mCString const & a_strMaterialName ) const
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
    if ( a_uIndex >= GetNumNodes() )
        return 0;
    return m_arrNodes[ a_uIndex ];
}

MIUInt mCScene::GetNodeIndexByID( mCUnique::ID a_ID ) const
{
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
        if ( m_arrNodes[ u ]->GetID() == a_ID )
            return u;
    return MI_DW_INVALID;
}

MIUInt mCScene::GetNodeIndexByName( mCString const & a_strNodeName ) const
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

namespace
{
    void AddNodeIndex( mCScene const & a_sceneScene, MIUInt a_uNodeIndex, mTArray< MIUInt > & a_arrNodeIndices, mTArray< MIBool > & a_arrNodeStates )
    {
        if ( a_arrNodeStates[ a_uNodeIndex ] )
            return;
        MIUInt const uParentIndex = a_sceneScene.GetNodeParentIndex( a_uNodeIndex );
        if ( uParentIndex != MI_DW_INVALID )
            AddNodeIndex( a_sceneScene, uParentIndex, a_arrNodeIndices, a_arrNodeStates );
        a_arrNodeIndices.Add( a_uNodeIndex );
        a_arrNodeStates[ a_uNodeIndex ] = MITrue;
    }

    void GetNodeReorderPattern( mCScene const & a_sceneScene, mTArray< MIUInt > & a_arrDest )
    {
        MIUInt const uNodeCount = a_sceneScene.GetNumNodes();
        mTArray< MIBool > arrNodeStates( MIFalse, uNodeCount );
        a_arrDest.Reserve( uNodeCount );
        for ( MIUInt u = uNodeCount; u--; )
            AddNodeIndex( a_sceneScene, u, a_arrDest, arrNodeStates );
    }
}

void mCScene::GetNodesSortedByLinks( mTArray< mCNode const * > & a_arrDest ) const
{
    mTArray< MIUInt > arrPattern;
    GetNodeReorderPattern( *this, arrPattern );
    a_arrDest.SetAt( 0, m_arrNodes.GetBuffer(), GetNumNodes() );
    g_reorder( a_arrDest.AccessBuffer(), arrPattern.GetBuffer(), GetNumNodes() );
};

void mCScene::GetNodesSortedByLinks( mTArray< mCNode * > & a_arrDest )
{
    mTArray< MIUInt > arrPattern;
    GetNodeReorderPattern( *this, arrPattern );
    a_arrDest = m_arrNodes;
    g_reorder( a_arrDest.AccessBuffer(), arrPattern.GetBuffer(), GetNumNodes() );
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

void mCScene::SetNodeParent( MIUInt a_uNodeIndex, MIUInt a_uParentNodeIndex )
{
    AccessNodeAt( a_uNodeIndex )->AccessParentID() = GetNodeAt( a_uParentNodeIndex )->GetID();
}

void mCScene::SortNodesByLinks( void )
{
    GetNodesSortedByLinks( m_arrNodes );
}

void mCScene::Swap( mCScene & a_sceneOther )
{
    if ( this == &a_sceneOther )
        return;
    m_strName.Swap( a_sceneOther.m_strName );
    m_arrMaterials.Swap( a_sceneOther.m_arrMaterials );
    m_arrNodes.Swap( a_sceneOther.m_arrNodes );
}

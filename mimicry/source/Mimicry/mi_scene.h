#ifndef MI_SCENE_H_INCLUDED
#define MI_SCENE_H_INCLUDED

class mCScene :
    public mCUnique
{
public:
    mCScene( mCScene const & a_sceneSource );
    mCScene( void );
   ~mCScene( void );
public:
    mCScene & operator = ( mCScene const & a_sceneSource );
public:
    mCMaterialBase *       AccessMaterialAt( MIUInt a_uIndex );
    mCNode *               AccessNodeAt( MIUInt a_uIndex );
    void                   Clear( void );
    mCMaterialBase const * GetMaterialAt( MIUInt a_uIndex ) const;
    MIUInt                 GetMaterialIndexByName( mCString a_strMaterialName ) const;
    mCString const &       GetName( void );
    mCNode const *         GetNodeAt( MIUInt a_uIndex ) const;
    MIUInt                 GetNodeIndexByID( mCUnique::ID a_ID ) const;
    MIUInt                 GetNodeIndexByName( mCString a_strNodeName ) const; 
    MIUInt                 GetNumMaterials( void ) const;
    MIUInt                 GetNumNodes( void ) const;
    void                   RemoveMaterial( mCMaterialBase * a_pMaterial );
    void                   RemoveNode( mCNode * a_pNode );
    void                   SetName( mCString const & a_strName );
    void                   Swap( mCScene & a_sceneOther );
private:
    mCString                    m_strName;
    mTArray< mCMaterialBase * > m_arrMaterials;
    mTArray< mCNode * >         m_arrNodes;
};

#endif
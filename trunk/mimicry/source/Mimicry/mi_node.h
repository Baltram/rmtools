#ifndef MI_NODE_H_INCLUDED
#define MI_NODE_H_INCLUDED

class mCNode :
    public mCUnique
{
public:
    mCNode( mCString const & a_strName, mCVec3 a_vecPosition = mCVec3(), mCString const & a_strMaterialName = mCString(), mCMesh const * a_pMesh = 0, mCSkin const * a_pSkin = 0 );
    mCNode( mCNode const & a_nodeSource );
    mCNode( void );
   ~mCNode( void );
public:
    mCNode & operator = ( mCNode const & a_nodeSource );
public:
    mCString &       AccessMaterialName( void );
    mCString &       AccessName( void );
    mCVec3 &         AccessPosition( void );
    mCString const & GetMaterialName( void ) const;
    mCMesh const *   GetMesh( void ) const;
    mCString const & GetName( void ) const;
    mCSkin const *   GetSkin( void ) const;
    mCVec3 const &   GetPosition( void ) const;
    MIBool           HasMesh( void );
    MIBool           HasSkin( void );
    void             SetMesh( mCMesh const * a_pMesh );
    void             SetSkin( mCSkin const * a_pSkin );
    void             Swap( mCNode & a_nodeOther );
    void             SwapMesh( mCMesh & a_meshOther );
    void             SwapSkin( mCSkin & a_skinOther );
private:
    mCUnique::ID &       AccessParentID( void );
    mCUnique::ID const & GetParentID( void ) const;
private:
    mCString     m_strName;
    mCString     m_strMaterialName;
    mCVec3       m_vecPosition;
    mCMesh *     m_pMesh;
    mCSkin *     m_pSkin;
    mCUnique::ID m_idParent;
public:
    friend class mCScene;
};

#endif
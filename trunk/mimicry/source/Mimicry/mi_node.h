#ifndef MI_NODE_H_INCLUDED
#define MI_NODE_H_INCLUDED

class mCNode :
    public mCUnique
{
public:
    mCNode( mCString const & a_strName, mCMatrix4 const & a_matTransform, mCString const & a_strMaterialName = mCString(), mCMesh const * a_pMesh = 0, mCSkin const * a_pSkin = 0, MIBool a_bIsBone = MIFalse );
    mCNode( mCNode const & a_nodeSource );
    mCNode( void );
   ~mCNode( void );
public:
    mCNode & operator = ( mCNode const & a_nodeSource );
public:
    MIBool &          AccessIsBone( void );
    mCString &        AccessMaterialName( void );
    mCMesh *          AccessMesh( void );
    mCString &        AccessName( void );
    mCVec3 &          AccessPosition( void );
    mCSkin *          AccessSkin( void );
    mCMatrix4 &       AccessTransform( void );
    MIBool            GetIsBone( void ) const;
    mCString const &  GetMaterialName( void ) const;
    mCMesh const *    GetMesh( void ) const;
    mCString const &  GetName( void ) const;
    mCSkin const *    GetSkin( void ) const;
    mCMatrix4 const & GetTransform( void ) const;
    mCVec3 const &    GetPosition( void ) const;
    MIBool            HasMesh( void ) const;
    MIBool            HasSkin( void ) const;
    void              SetMesh( mCMesh const * a_pMesh );
    void              SetSkin( mCSkin const * a_pSkin );
    void              Swap( mCNode & a_nodeOther );
    void              SwapMesh( mCMesh & a_meshOther );
    void              SwapSkin( mCSkin & a_skinOther );
private:
    mCUnique::ID &       AccessParentID( void );
    mCUnique::ID const & GetParentID( void ) const;
private:
    mCUnique::ID m_idParent;
    mCMatrix4    m_matTransform;
    mCString     m_strName;
    mCString     m_strMaterialName;
    mCMesh *     m_pMesh;
    mCSkin *     m_pSkin;
    MIBool       m_bIsBone;
public:
    friend class mCScene;
};

#endif

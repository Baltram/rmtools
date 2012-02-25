#ifndef MI_MULTIMATERIAL_H_INCLUDED
#define MI_MULTIMATERIAL_H_INCLUDED

class mCMultiMaterial :
    public mCMaterialBase
{
public:
   virtual ~mCMultiMaterial( void );
public:
    mCMultiMaterial( mCString const & a_strName );
    mCMultiMaterial( mCMultiMaterial const & a_mtlSource );
    mCMultiMaterial( void );
public:
    mCMultiMaterial &  operator =  ( mCMultiMaterial const & a_mtlSource );
    mCMaterial &       operator [] ( MIUInt a_uIndex );
    mCMaterial const & operator [] ( MIUInt a_uIndex ) const;
public:
    virtual mCMultiMaterial * Clone( void ) const;
public:
    mTArray< mCMaterial > &       AccessSubMaterials( void );
    void                          Clear( void );
    mTArray< mCMaterial > const & GetSubMaterials( void ) const;
    void                          Swap( mCMultiMaterial & a_mtlOther );
private:
    mTArray< mCMaterial > m_arrSubMaterials;
};

#endif
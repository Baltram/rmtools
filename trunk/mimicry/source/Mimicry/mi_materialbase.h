#ifndef MI_MATERIALBASE_H_INCLUDED
#define MI_MATERIALBASE_H_INCLUDED

class mCMaterialBase :
    public mCUnique
{
public:
    virtual ~mCMaterialBase( void );
public:
    mCMaterialBase( mCString const & a_strName );
    mCMaterialBase( mCMaterialBase const & a_mtlSource );
    mCMaterialBase( void );
public:
    mCMaterialBase & operator =  ( mCMaterialBase const & a_mtlSource );
    MIBool           operator == ( mCMaterialBase const & a_mtlOther ) const;
    MIBool           operator != ( mCMaterialBase const & a_mtlOther ) const;
public:
    virtual mCMaterialBase * Clone( void ) const = 0;
public:
    mCString &       AccessName( void );
    mCString const & GetName( void ) const;
protected:
    void Swap( mCMaterialBase & a_mtlOther );
private:
    mCString m_strName;
};

#endif

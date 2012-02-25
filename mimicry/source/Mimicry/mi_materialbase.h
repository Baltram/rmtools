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
    mCMaterialBase & operator = ( mCMaterialBase const & a_mtlSource );
public:
    virtual mCMaterialBase * Clone( void ) const = 0;
public:
    mCString const & GetName( void ) const;
    void             SetName( mCString const & a_strName );
protected:
    void Swap( mCMaterialBase & a_mtlOther );
private:
    mCString m_strName;
};

#endif
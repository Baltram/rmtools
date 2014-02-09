#ifndef MI_TEXMAP_H_INCLUDED
#define MI_TEXMAP_H_INCLUDED

class mCTexMap
{
public:
    mCTexMap( mCString const & a_strName, mCString const & a_strTextureFilePath );
    mCTexMap( mCTexMap const & a_tmapSource );
    mCTexMap( void );
   ~mCTexMap( void );
public:
    mCTexMap & operator =  ( mCTexMap const & a_tmapSource );
    MIBool     operator == ( mCTexMap const & a_tmapOther ) const;
    MIBool     operator != ( mCTexMap const & a_tmapOther ) const;
public:
    mCString &       AccessName( void );
    mCString &       AccessTextureFilePath( void );
    mCString const & GetName( void ) const;
    mCString const & GetTextureFilePath( void ) const;
    MIBool           IsEmpty( void ) const;
    void             Swap( mCTexMap & a_tmapOther );
private:
    mCString m_strName;
    mCString m_strTextureFilePath;
};

#endif

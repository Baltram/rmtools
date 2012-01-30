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
    mCTexMap & operator = ( mCTexMap const & a_tmapSource );
public:
    mCString const & GetName( void ) const;
    mCString const & GetTextureFilePath( void ) const;
    void             SetName( mCString const & a_strName );
    void             SetTextureFilePath( mCString const & a_strTextureFilePath );
    void             Swap( mCTexMap & a_tmapOther );
private:
    mCString m_strName;
    mCString m_strTextureFilePath;
};

#endif
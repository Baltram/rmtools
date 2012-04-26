#ifndef MI_NAME_H_INCLUDED
#define MI_NAME_H_INCLUDED

class mCName
{
public:
    explicit mCName( mCString const & a_strString );
             mCName( mCName const & a_nameSource );
public:
   ~mCName( void );
public:
    MIBool   operator == ( mCName const & a_nameOther ) const;
    MIBool   operator != ( mCName const & a_nameOther ) const;
    mCName & operator =  ( mCName const & a_nameSource );
public:
    MIU32            GetID( void );
    mCString const & GetString( void );
protected:
    struct StringRecord;
protected:
    mCName( void );
protected:
    static mTNameMap< StringRecord > s_mapNames;
protected:
    MIU32 m_u32ID;
private:
    template< typename K, typename T, class C >
    friend class mTMap;
};

struct mCName::StringRecord
{
    mCString m_strString;
    mCName   m_nameNext;
};

#endif
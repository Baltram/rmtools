#ifndef MI_UNIQUE_H_INCLUDED
#define MI_UNIQUE_H_INCLUDED

class mCUnique
{
public:
    typedef MIU64 ID;
public:
    mCUnique( void );
   ~mCUnique( void );
public:
    ID GetID( void );
private:
    mCUnique( mCUnique const & );
private:
    mCUnique & operator = ( mCUnique const & );
private:
    ID m_ID;
};

#endif
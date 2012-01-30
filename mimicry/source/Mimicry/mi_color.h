#ifndef MI_COLOR_H_INCLUDED
#define MI_COLOR_H_INCLUDED

class mCColor
{
public:
    mCColor( MIU8 a_u8Red, MIU8 a_u8Green, MIU8 a_u8Blue, MIU8 a_u8Alpha );
    mCColor( mCColor const & a_colSource );
    mCColor( void );
   ~mCColor( void );
public:
    MIBool    operator == ( mCColor const & a_colOther ) const;
    MIBool    operator != ( mCColor const & a_colOther ) const;
    mCColor & operator =  ( mCColor const & a_colSource );
public:
    MIU8 & AccessRed( void );
    MIU8 & AccessGreen( void );
    MIU8 & AccessBlue( void );
    MIU8 & AccessAlpha( void );
    MIU8   GetRed( void );
    MIU8   GetGreen( void );
    MIU8   GetBlue( void );
    MIU8   GetAlpha( void );
    void   Swap( mCColor & a_colOther );
private:
    MIU8 m_u8Red;
    MIU8 m_u8Green;
    MIU8 m_u8Blue;
    MIU8 m_u8Alpha;
};

#endif
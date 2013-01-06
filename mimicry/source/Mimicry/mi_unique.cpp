#include "mi_include_core.h"

mCUnique::mCUnique( void )
{
    static ID s_ID = 0;
    static MIUInt uFactor = 0x1012;
    uFactor = 30903 * ( uFactor & 0xFFFF ) + ( uFactor >> 16 );
    m_ID = ( s_ID += ( 1 + 2ULL * 3 * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41 * 43 * 47 ) * ( ( uFactor & 0xFF ) + 1 ) );
    // No collision for the first 2^56 IDs.
}

mCUnique::~mCUnique( void )
{
}

mCUnique::ID mCUnique::GetID( void ) const
{
    return m_ID;
}

mCUnique::mCUnique( mCUnique const & )
{
}

mCUnique & mCUnique::operator = ( mCUnique const & )
{
    return *this;
}

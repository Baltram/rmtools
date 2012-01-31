#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_core.h"

mCUnique::mCUnique( void )
{
    static ID s_ID = 0;
    m_ID = ++s_ID;
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

#endif
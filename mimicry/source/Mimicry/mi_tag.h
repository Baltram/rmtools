#ifndef MI_TAG_H_INCLUDED
#define MI_TAG_H_INCLUDED

struct mSTagNonScalar {};
struct mSTagScalar {};

template< typename T >
struct mSTypeTagHelper
{
    typedef mSTagNonScalar msTag;
};

#define MI_TAG_AS_SCALAR( x )     \
template<>                        \
struct mSTypeTagHelper< x >       \
{                                 \
    typedef mSTagScalar msTag;    \
};                                \
template<>                        \
struct mSTypeTagHelper< const x > \
{                                 \
    typedef mSTagScalar msTag;    \
};

MI_TAG_AS_SCALAR( MIChar )
MI_TAG_AS_SCALAR( MIUnicodeChar )
MI_TAG_AS_SCALAR( MIFloat )
MI_TAG_AS_SCALAR( MIDouble )
MI_TAG_AS_SCALAR( MIBool )
MI_TAG_AS_SCALAR( MIInt )
MI_TAG_AS_SCALAR( MIUInt )
MI_TAG_AS_SCALAR( MII8 )
MI_TAG_AS_SCALAR( MIU8 )
MI_TAG_AS_SCALAR( MII16 )
MI_TAG_AS_SCALAR( MIU16 )
MI_TAG_AS_SCALAR( MII32 )
MI_TAG_AS_SCALAR( MIU32 )
MI_TAG_AS_SCALAR( MII64 )
MI_TAG_AS_SCALAR( MIU64 )

template< typename T >
struct mSTypeTagHelper< T * > 
{
    typedef mSTagScalar msTag;
};

template< typename T >
struct mSTypeTagHelper< T * const > 
{
    typedef mSTagScalar msTag;
};

#endif
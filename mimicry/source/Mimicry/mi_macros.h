#ifndef MI_MACROS_H_INCLUDED
#define MI_MACROS_H_INCLUDED

#define MI_DW_INVALID 0xFFFFFFFFL

#define MI_FORCE_DWORD 0x7FFFFFFFL
#define MI_EXTERN extern
#define MI_CDECL __cdecl

#define MI_UNREFERENCED_PARAMETER( x ) x;

#if ( ( ULONG_MAX ) == ( UINT_MAX ) )
#define MI_32_BIT
#define MI_ASSERT_32_BIT
#else
#define MI_ASSERT_32_BIT { ONLY_32_BIT_COMPATIBLE }
#endif

#ifdef _MSC_VER
#define MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_BEGIN \
    __pragma( warning( push ) )                              \
    __pragma( warning( disable : 4127 ) )
#define MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_END \
    __pragma( warning( pop ) )
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN \
    __pragma( warning( push ) )              \
    __pragma( warning( disable : 4201 ) )
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_END \
    __pragma( warning( pop ) )
#define MI_ASM __asm
#define MI_CRT_NO_WARNINGS( A )           \
    __pragma( warning( push ) )           \
    __pragma( warning( disable : 4996 ) ) \
    A;                                    \
    __pragma( warning( pop ) ) 
#define MI_ASSERT_MSVC
#else
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_END
#define MI_ASM asm
#define MI_CRT_NO_WARNINGS( A ) A
#define MI_ASSERT_MSVC { NOT_MSVC }
#endif

#define MI_PASTE_TOKENS( A, B ) MI_PASTE_TOKENS_( A, B )
#define MI_PASTE_TOKENS_( A, B ) MI_PASTE_TOKENS__( A, B )
#define MI_PASTE_TOKENS__( A, B ) A##B

#endif
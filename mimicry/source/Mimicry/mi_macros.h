#ifndef MI_MACROS_H_INCLUDED
#define MI_MACROS_H_INCLUDED

#define MI_DW_INVALID 0xFFFFFFFFL

#define MI_FORCE_DWORD 0x7FFFFFFFL
#define MI_EXTERN extern

#define MI_UNREFERENCED_PARAMETER( x ) x;

#if ( !defined( _MSC_VER ) && INTPTR_MAX == INT32_MAX ) || ( defined( _MSC_VER ) && defined( _M_IX86 ) )
#define MI_32_BIT
#define MI_ASSERT_32_BIT
#else
#define MI_ASSERT_32_BIT { ONLY_32_BIT_COMPATIBLE }
#endif

#ifdef _MSC_VER

#define MI_ASSERT_MSVC

#define MI_CDECL __cdecl
#define MI_STDCALL __stdcall

#define MI_ASM __asm

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
#define MI_CRT_NO_WARNINGS( A )           \
    __pragma( warning( push ) )           \
    __pragma( warning( disable : 4996 ) ) \
    A;                                    \
    __pragma( warning( pop ) ) 

#else

#define MI_ASSERT_MSVC { NOT_MSVC }

#define MI_CDECL
#define MI_STDCALL

#define MI_ASM asm

#define MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_BEGIN
#define MI_CONSTANT_CONDITIONAL_EXPRESSION_NO_WARNINGS_END
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN
#define MI_UNNAMED_STRUCTS_NO_WARNINGS_END
#define MI_CRT_NO_WARNINGS( A ) A

#endif

#define MI_PASTE_TOKENS( A, B ) MI_PASTE_TOKENS_( A, B )
#define MI_PASTE_TOKENS_( A, B ) MI_PASTE_TOKENS__( A, B )
#define MI_PASTE_TOKENS__( A, B ) A##B

#define MI_C_ARRAYSIZE( A ) ( sizeof( A ) / sizeof ( *( A ) ) )

#endif

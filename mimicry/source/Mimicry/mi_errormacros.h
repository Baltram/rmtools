#ifndef MI_ERRORMACROS_H_INCLUDED
#define MI_ERRORMACROS_H_INCLUDED

#define MI_ERROR( _TYPE_, _MEMBER_ENUM_ERROR_CODE_, _TEXT_ ) mCError::Error< _TYPE_ >( _TYPE_::_MEMBER_ENUM_ERROR_CODE_, _TEXT_, __FILE__, __LINE__ )

// In the following, "..." is a placeholder for enum error codes.

#define MI_ERROR_DEFINE_DERIVED( _NAME_, _PARENT_, ... ) \
    class _NAME_ :                                       \
        public _PARENT_                                  \
    {                                                    \
    public:                                              \
        enum                                             \
        {                                                \
            EForceDWORD = MI_FORCE_DWORD,                \
            EDefault = 0,                                \
            __VA_ARGS__                                  \
        };                                               \
    protected:                                           \
        virtual ~_NAME_( void )                          \
        {                                                \
        }                                                \
    protected:                                           \
        _NAME_( void )                                   \
        {                                                \
        }                                                \
    public:                                              \
        friend class mCError;                            \
    };

#endif
#include "mi_include_unity.h"

#ifdef MI_UNITY

#define MI_IN_UNITY_FILE

#include "mi_error.cpp"
#include "mi_global.cpp"
#include "mi_unique.cpp"

#undef MI_IN_UNITY_FILE

#endif
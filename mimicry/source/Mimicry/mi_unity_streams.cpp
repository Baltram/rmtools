#include "mi_include_unity.h"

#ifdef MI_UNITY

#define MI_IN_UNITY_FILE

#include "mi_iostream.cpp"
#include "mi_memorystreambase.cpp"
#include "mi_memorystream.cpp"
#include "mi_stringstream.cpp"
#include "mi_filestream.cpp"
#include "mi_maxfilestream.cpp"

#undef MI_IN_UNITY_FILE

#endif
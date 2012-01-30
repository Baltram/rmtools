#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_streams.h"

template class mTIOStream< mEStreamType_Binary >;
template class mTIOStream< mEStreamType_Formatted >;

#endif
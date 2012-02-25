#include "mi_include_unity.h"

#ifdef MI_UNITY

#define MI_IN_UNITY_FILE

#include "mi_box.cpp"
#include "mi_color.cpp"
#include "mi_face.cpp"
#include "mi_maxface.cpp"
#include "mi_vec3.cpp"
#include "mi_vertexmatcher.cpp"
#include "mi_matrix4.cpp"
#include "mi_quaternion.cpp"

#undef MI_IN_UNITY_FILE

#endif
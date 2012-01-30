#include "mi_include_unity.h"

#ifdef MI_UNITY

#define MI_IN_UNITY_FILE

#include "mi_material.cpp"
#include "mi_materialbase.cpp"
#include "mi_mesh.cpp"
#include "mi_multimaterial.cpp"
#include "mi_node.cpp"
#include "mi_scene.cpp"
#include "mi_skin.cpp"
#include "mi_texmap.cpp"

#undef MI_IN_UNITY_FILE

#endif
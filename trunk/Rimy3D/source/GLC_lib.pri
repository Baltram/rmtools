QT += opengl \
      core

CONFIG += exceptions
INCLUDEPATH += "$$(GLC_LIB_DIR)/include"

CONFIG(debug, debug|release) {
    LIBS += -L"$$(GLC_LIB_DIR)/lib" -lGLC_lib2
}

CONFIG(release, debug|release) {
    DEFINES += GLC_LIB_STATIC
    DEFINES += _CRT_SECURE_NO_WARNINGS

    GLCSRC = $$(GLC_LIB_DIR)/src

    INCLUDEPATH += "$$GLCSRC"
    INCLUDEPATH += "$$GLCSRC/3rdparty/zlib"

    HEADERS_QUAZIP += "$$GLCSRC/3rdparty/quazip/crypt.h" \
                      "$$GLCSRC/3rdparty/quazip/ioapi.h" \
                      "$$GLCSRC/3rdparty/quazip/quazip.h" \
                      "$$GLCSRC/3rdparty/quazip/quazipfile.h" \
                      "$$GLCSRC/3rdparty/quazip/quazipfileinfo.h" \
                      "$$GLCSRC/3rdparty/quazip/quazipnewinfo.h" \
                      "$$GLCSRC/3rdparty/quazip/unzip.h" \
                      "$$GLCSRC/3rdparty/quazip/zip.h"

    HEADERS_LIB3DS += "$$GLCSRC/3rdparty/lib3ds/atmosphere.h" \
                      "$$GLCSRC/3rdparty/lib3ds/background.h" \
                      "$$GLCSRC/3rdparty/lib3ds/camera.h" \
                      "$$GLCSRC/3rdparty/lib3ds/chunk.h" \
                      "$$GLCSRC/3rdparty/lib3ds/chunktable.h" \
                      "$$GLCSRC/3rdparty/lib3ds/ease.h" \
                      "$$GLCSRC/3rdparty/lib3ds/file.h" \
                      "$$GLCSRC/3rdparty/lib3ds/io.h" \
                      "$$GLCSRC/3rdparty/lib3ds/light.h" \
                      "$$GLCSRC/3rdparty/lib3ds/material.h" \
                      "$$GLCSRC/3rdparty/lib3ds/matrix.h" \
                      "$$GLCSRC/3rdparty/lib3ds/mesh.h" \
                      "$$GLCSRC/3rdparty/lib3ds/node.h" \
                      "$$GLCSRC/3rdparty/lib3ds/quat.h" \
                      "$$GLCSRC/3rdparty/lib3ds/shadow.h" \
                      "$$GLCSRC/3rdparty/lib3ds/tcb.h" \
                      "$$GLCSRC/3rdparty/lib3ds/tracks.h" \
                      "$$GLCSRC/3rdparty/lib3ds/types.h" \
                      "$$GLCSRC/3rdparty/lib3ds/vector.h" \
                      "$$GLCSRC/3rdparty/lib3ds/viewport.h"

    HEADERS_GLEXT += "$$GLCSRC/3rdparty/glext/glext.h"

    HEADERS_GLC_MATHS += "$$GLCSRC/maths/glc_utils_maths.h" \
                         "$$GLCSRC/maths/glc_vector2d.h" \
                         "$$GLCSRC/maths/glc_vector2df.h" \
                         "$$GLCSRC/maths/glc_vector3d.h" \
                         "$$GLCSRC/maths/glc_vector4d.h" \
                         "$$GLCSRC/maths/glc_vector3df.h" \
                         "$$GLCSRC/maths/glc_matrix4x4.h" \
                         "$$GLCSRC/maths/glc_interpolator.h" \
                         "$$GLCSRC/maths/glc_plane.h" \
                         "$$GLCSRC/maths/glc_geomtools.h" \
                         "$$GLCSRC/maths/glc_line3d.h"

    HEADERS_GLC_IO += "$$GLCSRC/io/glc_objmtlloader.h" \
                      "$$GLCSRC/io/glc_objtoworld.h" \
                      "$$GLCSRC/io/glc_stltoworld.h" \
                      "$$GLCSRC/io/glc_offtoworld.h" \
                      "$$GLCSRC/io/glc_3dstoworld.h" \
                      "$$GLCSRC/io/glc_3dxmltoworld.h" \
                      "$$GLCSRC/io/glc_colladatoworld.h" \
                      "$$GLCSRC/io/glc_worldto3dxml.h" \
                      "$$GLCSRC/io/glc_bsreptoworld.h" \
                      "$$GLCSRC/io/glc_xmlutil.h" \
                      "$$GLCSRC/io/glc_fileloader.h" \
                      "$$GLCSRC/io/glc_worldreaderplugin.h" \
                      "$$GLCSRC/io/glc_worldreaderhandler.h"

    HEADERS_GLC_SCENEGRAPH += "$$GLCSRC/sceneGraph/glc_3dviewcollection.h" \
                              "$$GLCSRC/sceneGraph/glc_3dviewinstance.h" \
                              "$$GLCSRC/sceneGraph/glc_structreference.h" \
                              "$$GLCSRC/sceneGraph/glc_structinstance.h" \
                              "$$GLCSRC/sceneGraph/glc_structoccurence.h" \
                              "$$GLCSRC/sceneGraph/glc_world.h" \
                              "$$GLCSRC/sceneGraph/glc_attributes.h" \
                              "$$GLCSRC/sceneGraph/glc_worldhandle.h" \
                              "$$GLCSRC/sceneGraph/glc_spacepartitioning.h" \
                              "$$GLCSRC/sceneGraph/glc_octree.h" \
                              "$$GLCSRC/sceneGraph/glc_octreenode.h" \
                              "$$GLCSRC/sceneGraph/glc_selectionset.h"

    HEADERS_GLC_GEOMETRY += "$$GLCSRC/geometry/glc_geometry.h" \
                            "$$GLCSRC/geometry/glc_circle.h" \
                            "$$GLCSRC/geometry/glc_cylinder.h" \
                            "$$GLCSRC/geometry/glc_point.h" \
                            "$$GLCSRC/geometry/glc_box.h" \
                            "$$GLCSRC/geometry/glc_meshdata.h" \
                            "$$GLCSRC/geometry/glc_primitivegroup.h" \
                            "$$GLCSRC/geometry/glc_mesh.h" \
                            "$$GLCSRC/geometry/glc_lod.h" \
                            "$$GLCSRC/geometry/glc_rectangle.h" \
                            "$$GLCSRC/geometry/glc_line.h" \
                            "$$GLCSRC/geometry/glc_rep.h" \
                            "$$GLCSRC/geometry/glc_3drep.h" \
                            "$$GLCSRC/geometry/glc_pointsprite.h" \
                            "$$GLCSRC/geometry/glc_bsrep.h" \
                            "$$GLCSRC/geometry/glc_wiredata.h" \
                            "$$GLCSRC/geometry/glc_arrow.h" \
                            "$$GLCSRC/geometry/glc_polylines.h" \
                            "$$GLCSRC/geometry/glc_disc.h" \
                            "$$GLCSRC/geometry/glc_cone.h" \
                            "$$GLCSRC/geometry/glc_sphere.h" \
                            "$$GLCSRC/geometry/glc_pointcloud.h"

    HEADERS_GLC_SHADING += "$$GLCSRC/shading/glc_material.h" \
                           "$$GLCSRC/shading/glc_texture.h" \
                           "$$GLCSRC/shading/glc_shader.h" \
                           "$$GLCSRC/shading/glc_selectionmaterial.h" \
                           "$$GLCSRC/shading/glc_light.h" \
                           "$$GLCSRC/shading/glc_renderproperties.h"

    HEADERS_GLC_VIEWPORT += "$$GLCSRC/viewport/glc_camera.h" \
                            "$$GLCSRC/viewport/glc_imageplane.h" \
                            "$$GLCSRC/viewport/glc_viewport.h" \
                            "$$GLCSRC/viewport/glc_movercontroller.h"\
                            "$$GLCSRC/viewport/glc_mover.h" \
                            "$$GLCSRC/viewport/glc_panmover.h" \
                            "$$GLCSRC/viewport/glc_repmover.h" \
                            "$$GLCSRC/viewport/glc_repcrossmover.h" \
                            "$$GLCSRC/viewport/glc_zoommover.h" \
                            "$$GLCSRC/viewport/glc_trackballmover.h" \
                            "$$GLCSRC/viewport/glc_reptrackballmover.h" \
                            "$$GLCSRC/viewport/glc_settargetmover.h" \
                            "$$GLCSRC/viewport/glc_turntablemover.h" \
                            "$$GLCSRC/viewport/glc_frustum.h" \
                            "$$GLCSRC/viewport/glc_flymover.h" \
                            "$$GLCSRC/viewport/glc_repflymover.h" \
                            "$$GLCSRC/viewport/glc_userinput.h" \
                            "$$GLCSRC/viewport/glc_tsrmover.h"

    HEADERS_GLC += "$$GLCSRC/glc_global.h" \
                   "$$GLCSRC/glc_object.h" \
                   "$$GLCSRC/glc_factory.h" \
                   "$$GLCSRC/glc_boundingbox.h" \
                   "$$GLCSRC/glc_exception.h" \
                   "$$GLCSRC/glc_openglexception.h" \
                   "$$GLCSRC/glc_fileformatexception.h" \
                   "$$GLCSRC/glc_ext.h" \
                   "$$GLCSRC/glc_state.h" \
                   "$$GLCSRC/glc_config.h" \
                   "$$GLCSRC/glc_cachemanager.h" \
                   "$$GLCSRC/glc_renderstatistics.h" \
                   "$$GLCSRC/glc_log.h" \
                   "$$GLCSRC/glc_errorlog.h" \
                   "$$GLCSRC/glc_tracelog.h" \
                   "$$GLCSRC/glc_openglstate.h"

    HEADERS_GLC_3DWIDGET += "$$GLCSRC/3DWidget/glc_3dwidget.h" \
                            "$$GLCSRC/3DWidget/glc_cuttingplane.h" \
                            "$$GLCSRC/3DWidget/glc_3dwidgetmanager.h" \
                            "$$GLCSRC/3DWidget/glc_3dwidgetmanagerhandle.h" \
                            "$$GLCSRC/3DWidget/glc_abstractmanipulator.h" \
                            "$$GLCSRC/3DWidget/glc_pullmanipulator.h" \
                            "$$GLCSRC/3DWidget/glc_rotationmanipulator.h" \
                            "$$GLCSRC/3DWidget/glc_axis.h"

    HEADERS += $${HEADERS_QUAZIP} $${HEADERS_LIB3DS} $${HEADERS_GLC_MATHS} $${HEADERS_GLC_IO}
    HEADERS += $${HEADERS_GLC} $${HEADERS_GLEXT} $${HEADERS_GLC_SCENEGRAPH} $${HEADERS_GLC_GEOMETRY}
    HEADERS += $${HEADERS_GLC_SHADING} $${HEADERS_GLC_VIEWPORT} $${HEADERS_GLC_3DWIDGET}

    SOURCES += "$$GLCSRC/3rdparty/zlib/adler32.c" \
               "$$GLCSRC/3rdparty/zlib/compress.c" \
               "$$GLCSRC/3rdparty/zlib/crc32.c" \
               "$$GLCSRC/3rdparty/zlib/deflate.c" \
               "$$GLCSRC/3rdparty/zlib/gzio.c" \
               "$$GLCSRC/3rdparty/zlib/inffast.c" \
               "$$GLCSRC/3rdparty/zlib/inflate.c" \
               "$$GLCSRC/3rdparty/zlib/inftrees.c" \
               "$$GLCSRC/3rdparty/zlib/trees.c" \
               "$$GLCSRC/3rdparty/zlib/uncompr.c" \
               "$$GLCSRC/3rdparty/zlib/zutil.c"

    SOURCES += "$$GLCSRC/3rdparty/quazip/ioapi.c" \
               "$$GLCSRC/3rdparty/quazip/quazip.cpp" \
               "$$GLCSRC/3rdparty/quazip/quazipfile.cpp" \
               "$$GLCSRC/3rdparty/quazip/quazipnewinfo.cpp" \
               "$$GLCSRC/3rdparty/quazip/unzip.c" \
               "$$GLCSRC/3rdparty/quazip/zip.c"

    SOURCES += "$$GLCSRC/3rdparty/lib3ds/atmosphere.c" \
               "$$GLCSRC/3rdparty/lib3ds/background.c" \
               "$$GLCSRC/3rdparty/lib3ds/camera.c" \
               "$$GLCSRC/3rdparty/lib3ds/chunk.c" \
               "$$GLCSRC/3rdparty/lib3ds/ease.c" \
               "$$GLCSRC/3rdparty/lib3ds/file.c" \
               "$$GLCSRC/3rdparty/lib3ds/io.c" \
               "$$GLCSRC/3rdparty/lib3ds/light.c" \
               "$$GLCSRC/3rdparty/lib3ds/material.c" \
               "$$GLCSRC/3rdparty/lib3ds/matrix.c" \
               "$$GLCSRC/3rdparty/lib3ds/mesh.c" \
               "$$GLCSRC/3rdparty/lib3ds/node.c" \
               "$$GLCSRC/3rdparty/lib3ds/quat.c" \
               "$$GLCSRC/3rdparty/lib3ds/shadow.c" \
               "$$GLCSRC/3rdparty/lib3ds/tcb.c" \
               "$$GLCSRC/3rdparty/lib3ds/tracks.c" \
               "$$GLCSRC/3rdparty/lib3ds/vector.c" \
               "$$GLCSRC/3rdparty/lib3ds/viewport.c"

    SOURCES += "$$GLCSRC/maths/glc_matrix4x4.cpp" \
               "$$GLCSRC/maths/glc_vector4d.cpp" \
               "$$GLCSRC/maths/glc_interpolator.cpp" \
               "$$GLCSRC/maths/glc_plane.cpp" \
               "$$GLCSRC/maths/glc_geomtools.cpp" \
               "$$GLCSRC/maths/glc_line3d.cpp"

    SOURCES += "$$GLCSRC/io/glc_objmtlloader.cpp" \
               "$$GLCSRC/io/glc_objtoworld.cpp" \
               "$$GLCSRC/io/glc_stltoworld.cpp" \
               "$$GLCSRC/io/glc_offtoworld.cpp" \
               "$$GLCSRC/io/glc_3dstoworld.cpp" \
               "$$GLCSRC/io/glc_3dxmltoworld.cpp" \
               "$$GLCSRC/io/glc_colladatoworld.cpp" \
               "$$GLCSRC/io/glc_worldto3dxml.cpp" \
               "$$GLCSRC/io/glc_bsreptoworld.cpp" \
               "$$GLCSRC/io/glc_fileloader.cpp"

    SOURCES += "$$GLCSRC/sceneGraph/glc_3dviewcollection.cpp" \
               "$$GLCSRC/sceneGraph/glc_3dviewinstance.cpp" \
               "$$GLCSRC/sceneGraph/glc_structreference.cpp" \
               "$$GLCSRC/sceneGraph/glc_structinstance.cpp" \
               "$$GLCSRC/sceneGraph/glc_structoccurence.cpp" \
               "$$GLCSRC/sceneGraph/glc_world.cpp" \
               "$$GLCSRC/sceneGraph/glc_attributes.cpp" \
               "$$GLCSRC/sceneGraph/glc_worldhandle.cpp" \
               "$$GLCSRC/sceneGraph/glc_spacepartitioning.cpp" \
               "$$GLCSRC/sceneGraph/glc_octree.cpp" \
               "$$GLCSRC/sceneGraph/glc_octreenode.cpp" \
               "$$GLCSRC/sceneGraph/glc_selectionset.cpp"

    SOURCES += "$$GLCSRC/geometry/glc_geometry.cpp" \
               "$$GLCSRC/geometry/glc_circle.cpp" \
               "$$GLCSRC/geometry/glc_cylinder.cpp" \
               "$$GLCSRC/geometry/glc_point.cpp" \
               "$$GLCSRC/geometry/glc_box.cpp" \
               "$$GLCSRC/geometry/glc_meshdata.cpp" \
               "$$GLCSRC/geometry/glc_primitivegroup.cpp" \
               "$$GLCSRC/geometry/glc_mesh.cpp" \
               "$$GLCSRC/geometry/glc_lod.cpp" \
               "$$GLCSRC/geometry/glc_rectangle.cpp" \
               "$$GLCSRC/geometry/glc_line.cpp" \
               "$$GLCSRC/geometry/glc_rep.cpp" \
               "$$GLCSRC/geometry/glc_3drep.cpp" \
               "$$GLCSRC/geometry/glc_pointsprite.cpp" \
               "$$GLCSRC/geometry/glc_bsrep.cpp" \
               "$$GLCSRC/geometry/glc_wiredata.cpp" \
               "$$GLCSRC/geometry/glc_arrow.cpp" \
               "$$GLCSRC/geometry/glc_polylines.cpp" \
               "$$GLCSRC/geometry/glc_disc.cpp" \
               "$$GLCSRC/geometry/glc_cone.cpp" \
               "$$GLCSRC/geometry/glc_sphere.cpp" \
               "$$GLCSRC/geometry/glc_pointcloud.cpp"

    SOURCES += "$$GLCSRC/shading/glc_material.cpp" \
               "$$GLCSRC/shading/glc_texture.cpp" \
               "$$GLCSRC/shading/glc_light.cpp" \
               "$$GLCSRC/shading/glc_selectionmaterial.cpp" \
               "$$GLCSRC/shading/glc_shader.cpp" \
               "$$GLCSRC/shading/glc_renderproperties.cpp"

    SOURCES += "$$GLCSRC/viewport/glc_camera.cpp" \
               "$$GLCSRC/viewport/glc_imageplane.cpp" \
               "$$GLCSRC/viewport/glc_viewport.cpp" \
               "$$GLCSRC/viewport/glc_movercontroller.cpp"\
               "$$GLCSRC/viewport/glc_mover.cpp" \
               "$$GLCSRC/viewport/glc_panmover.cpp" \
               "$$GLCSRC/viewport/glc_repmover.cpp" \
               "$$GLCSRC/viewport/glc_repcrossmover.cpp" \
               "$$GLCSRC/viewport/glc_zoommover.cpp" \
               "$$GLCSRC/viewport/glc_trackballmover.cpp" \
               "$$GLCSRC/viewport/glc_reptrackballmover.cpp" \
               "$$GLCSRC/viewport/glc_settargetmover.cpp" \
               "$$GLCSRC/viewport/glc_turntablemover.cpp" \
               "$$GLCSRC/viewport/glc_frustum.cpp" \
               "$$GLCSRC/viewport/glc_flymover.cpp" \
               "$$GLCSRC/viewport/glc_repflymover.cpp" \
               "$$GLCSRC/viewport/glc_userinput.cpp" \
               "$$GLCSRC/viewport/glc_tsrmover.cpp"

    SOURCES += "$$GLCSRC/glc_global.cpp" \
               "$$GLCSRC/glc_object.cpp" \
               "$$GLCSRC/glc_factory.cpp" \
               "$$GLCSRC/glc_boundingbox.cpp" \
               "$$GLCSRC/glc_exception.cpp" \
               "$$GLCSRC/glc_openglexception.cpp" \
               "$$GLCSRC/glc_fileformatexception.cpp" \
               "$$GLCSRC/glc_ext.cpp" \
               "$$GLCSRC/glc_state.cpp" \
               "$$GLCSRC/glc_cachemanager.cpp" \
               "$$GLCSRC/glc_renderstatistics.cpp" \
               "$$GLCSRC/glc_log.cpp" \
               "$$GLCSRC/glc_errorlog.cpp" \
               "$$GLCSRC/glc_tracelog.cpp" \
               "$$GLCSRC/glc_openglstate.cpp"

    SOURCES += "$$GLCSRC/3DWidget/glc_3dwidget.cpp" \
               "$$GLCSRC/3DWidget/glc_cuttingplane.cpp" \
               "$$GLCSRC/3DWidget/glc_3dwidgetmanager.cpp" \
               "$$GLCSRC/3DWidget/glc_3dwidgetmanagerhandle.cpp" \
               "$$GLCSRC/3DWidget/glc_abstractmanipulator.cpp" \
               "$$GLCSRC/3DWidget/glc_pullmanipulator.cpp" \
               "$$GLCSRC/3DWidget/glc_rotationmanipulator.cpp" \
               "$$GLCSRC/3DWidget/glc_axis.cpp"
}

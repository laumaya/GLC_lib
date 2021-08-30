# GLC_lib qmake configuration
TEMPLATE = lib
QT += core opengl quick concurrent

win32 {
    LIBS += -lopengl32
}


CONFIG += exceptions warn_on

ios: CONFIG += static


TARGET = GLC_lib
DESTDIR = ./
VERSION = 3.0.0

DEFINES += CREATE_GLC_LIB_DLL
DEFINES += QUAZIP_BUILD
DEFINES += LIB3DS_EXPORTS
DEFINES += _CRT_SECURE_NO_WARNINGS

OBJECTS_DIR = ./Build
MOC_DIR = ./Build
UI_DIR = ./Build
RCC_DIR = ./Build

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ./3rdparty/zlib
INCLUDEPATH += ./3rdparty/quazip

RESOURCES += glc_lib.qrc

# Input					

HEADERS_QUAZIP += \
                    3rdparty/quazip/minizip_crypt.h \
                    3rdparty/quazip/ioapi.h \
                    3rdparty/quazip/JlCompress.h \
                    3rdparty/quazip/quaadler32.h \
                    3rdparty/quazip/quachecksum32.h \
                    3rdparty/quazip/quacrc32.h \
                    3rdparty/quazip/quagzipfile.h \
                    3rdparty/quazip/quaziodevice.h \
                    3rdparty/quazip/quazipdir.h \
                    3rdparty/quazip/quazipfile.h \
                    3rdparty/quazip/quazipfileinfo.h \
                    3rdparty/quazip/quazip_global.h \
                    3rdparty/quazip/quazip.h \
                    3rdparty/quazip/quazipnewinfo.h \
                    3rdparty/quazip/unzip.h \
                    3rdparty/quazip/zip.h


HEADERS_LIB3DS +=   3rdparty/lib3ds/atmosphere.h \
                    3rdparty/lib3ds/background.h \
                    3rdparty/lib3ds/camera.h \
                    3rdparty/lib3ds/chunk.h \
                    3rdparty/lib3ds/chunktable.h \
                    3rdparty/lib3ds/ease.h \
                    3rdparty/lib3ds/file.h \
                    3rdparty/lib3ds/io.h \
                    3rdparty/lib3ds/light.h \
                    3rdparty/lib3ds/material.h \
                    3rdparty/lib3ds/matrix.h \
                    3rdparty/lib3ds/mesh.h \
                    3rdparty/lib3ds/node.h \
                    3rdparty/lib3ds/quat.h \
                    3rdparty/lib3ds/shadow.h \
                    3rdparty/lib3ds/tcb.h \
                    3rdparty/lib3ds/tracks.h \
                    3rdparty/lib3ds/types.h \
                    3rdparty/lib3ds/vector.h \
                    3rdparty/lib3ds/viewport.h
           
HEADERS_GLEXT += 3rdparty/glext/glext.h

HEADERS_CLIP2TR +=  3rdparty/clip2tri/clip2tri/clip2tri.h \
                    3rdparty/clip2tri/clipper/clip_clipper.hpp \
                    3rdparty/clip2tri/poly2tri/common/clip_shapes.h \
                    3rdparty/clip2tri/poly2tri/common/clip_utils.h \
                    3rdparty/clip2tri/poly2tri/sweep/clip_advancing_front.h \
                    3rdparty/clip2tri/poly2tri/sweep/clip_cdt.h \
                    3rdparty/clip2tri/poly2tri/sweep/clip_sweep.h \
                    3rdparty/clip2tri/poly2tri/sweep/clip_sweep_context.h \
                    3rdparty/clip2tri/poly2tri/clip_poly2tri.h

HEADERS_GLC_MATHS += 	maths/glc_utils_maths.h \
                        maths/glc_vector2d.h \
                        maths/glc_vector2df.h \
                        maths/glc_vector3d.h \
                        maths/glc_vector4d.h \
                        maths/glc_vector3df.h \
                        maths/glc_matrix4x4.h \
                        maths/glc_interpolator.h \
                        maths/glc_plane.h \
                        maths/glc_geomtools.h \
                        maths/glc_line3d.h \
                        maths/glc_triangle.h \
                        maths/glc_polygon.h
						
HEADERS_GLC_IO +=   io/glc_objmtlloader.h \
                    io/glc_objtoworld.h \
                    io/glc_stltoworld.h \
                    io/glc_offtoworld.h \
                    io/glc_3dstoworld.h \
                    io/glc_3dxmltoworld.h \
                    io/glc_colladatoworld.h \
                    io/glc_worldto3dxml.h \
                    io/glc_worldto3ds.h \
                    io/glc_bsreptoworld.h \
                    io/glc_xmlutil.h \
                    io/glc_fileloader.h \
                    io/glc_worldreaderplugin.h \
                    io/glc_worldreaderhandler.h \
                    io/glc_worldtoobj.h \
                    io/glc_assimptoworld.h \
                    io/glc_colladaxmlelement.h \
                    io/glc_worldtocollada.h

HEADERS_GLC_SCENEGRAPH +=   sceneGraph/glc_3dviewcollection.h \
                            sceneGraph/glc_3dviewinstance.h \
                            sceneGraph/glc_structreference.h \
                            sceneGraph/glc_structinstance.h \
                            sceneGraph/glc_structoccurrence.h \
                            sceneGraph/glc_world.h \
                            sceneGraph/glc_attributes.h \
                            sceneGraph/glc_worldhandle.h \
                            sceneGraph/glc_spacepartitioning.h \
                            sceneGraph/glc_octree.h \
                            sceneGraph/glc_octreenode.h \
                            sceneGraph/glc_selectionset.h
							
HEADERS_GLC_GEOMETRY += geometry/glc_geometry.h \
                        geometry/glc_circle.h \
                        geometry/glc_cylinder.h \
                        geometry/glc_point.h \
                        geometry/glc_box.h \
                        geometry/glc_meshdata.h \
                        geometry/glc_primitivegroup.h \
                        geometry/glc_mesh.h \
                        geometry/glc_lod.h \
                        geometry/glc_rectangle.h \
                        geometry/glc_line.h \
                        geometry/glc_rep.h \
                        geometry/glc_3drep.h \
                        geometry/glc_pointsprite.h \
                        geometry/glc_bsrep.h \
                        geometry/glc_wiredata.h \
                        geometry/glc_arrow.h \
                        geometry/glc_polylines.h \
                        geometry/glc_disc.h \
                        geometry/glc_cone.h \
                        geometry/glc_sphere.h \
                        geometry/glc_pointcloud.h \
                        geometry/glc_extrudedmesh.h \
                        geometry/glc_text.h \
                        geometry/glc_csghelper.h \
                        geometry/glc_csgnode.h \
                        geometry/glc_csgoperatornode.h \
                        geometry/glc_csgleafnode.h \
                        geometry/glc_lathemesh.h \
                        geometry/glc_image.h


HEADERS_GLC_SHADING +=  shading/glc_material.h \
                        shading/glc_texture.h \
                        shading/glc_shader.h \
                        shading/glc_selectionmaterial.h \
                        shading/glc_light.h \
                        shading/glc_renderproperties.h \
                        shading/glc_renderer.h
						
HEADERS_GLC_VIEWPORT +=	viewport/glc_camera.h \
                        viewport/glc_imageplane.h \
                        viewport/glc_viewport.h \
                        viewport/glc_movercontroller.h\
                        viewport/glc_mover.h \
                        viewport/glc_panmover.h \
                        viewport/glc_repmover.h \
                        viewport/glc_repcrossmover.h \
                        viewport/glc_zoommover.h \
                        viewport/glc_trackballmover.h \
                        viewport/glc_reptrackballmover.h \
                        viewport/glc_settargetmover.h \
                        viewport/glc_turntablemover.h \
                        viewport/glc_frustum.h \
                        viewport/glc_flymover.h \
                        viewport/glc_repflymover.h \
                        viewport/glc_userinput.h \
                        viewport/glc_tsrmover.h \
                        viewport/glc_viewhandler.h \
                        viewport/glc_openglviewhandler.h \
                        viewport/glc_inputeventinterpreter.h \
                        viewport/glc_defaulteventinterpreter.h \
                        viewport/glc_screenshotsettings.h \
                        viewport/glc_openglviewwidget.h \
                        viewport/glc_openglviewinterface.h

HEADERS_GLC += glc_global.h \
               glc_object.h \
               glc_factory.h \
               glc_boundingbox.h \
               glc_exception.h \
               glc_openglexception.h \
               glc_fileformatexception.h \
               glc_ext.h \
               glc_state.h \
               glc_config.h \
               glc_cachemanager.h \
               glc_renderstatistics.h \
               glc_log.h \
               glc_errorlog.h \
               glc_tracelog.h \
               glc_context.h \
               glc_contextmanager.h \
               glc_contextshareddata.h \
               glc_uniformshaderdata.h \
               glc_selectionevent.h \
               glc_renderstate.h
           
HEADERS_GLC_3DWIDGET += 3DWidget/glc_3dwidget.h \
                        3DWidget/glc_cuttingplane.h \
                        3DWidget/glc_3dwidgetmanager.h \
                        3DWidget/glc_3dwidgetmanagerhandle.h \
                        3DWidget/glc_abstractmanipulator.h \
                        3DWidget/glc_pullmanipulator.h \
                        3DWidget/glc_rotationmanipulator.h \
                        3DWidget/glc_planemanipulator.h \
                        3DWidget/glc_axis.h

HEADERS_GLC_GLU +=	glu/glc_glu.h

HEADERS_GLC_QML +=      qml/glc_quickitem.h \
                        qml/glc_quickview.h \
                        qml/glc_quickcamera.h \
                        qml/glc_quickoccurrence.h \
                        qml/glc_quickselection.h \
                        qml/glc_quickviewhandler.h

HEADERS_GLC_CSGJS += 3rdparty/csgjs/csgjs.h


HEADERS_ZLIB    += 3rdparty/zlib/crc32.h \
                   3rdparty/zlib/deflate.h \
                   3rdparty/zlib/inffast.h \
                   3rdparty/zlib/inffixed.h \
                   3rdparty/zlib/inflate.h \
                   3rdparty/zlib/inftrees.h \
                   3rdparty/zlib/trees.h \
                   3rdparty/zlib/zconf.h \
                   3rdparty/zlib/zconf.in.h \
                   3rdparty/zlib/zlib.h \
                   3rdparty/zlib/zutil.h

HEADERS += $${HEADERS_QUAZIP} $${HEADERS_LIB3DS} $${HEADERS_GLC_MATHS} $${HEADERS_GLC_IO}
HEADERS += $${HEADERS_GLC} $${HEADERS_GLEXT} $${HEADERS_GLC_SCENEGRAPH} $${HEADERS_GLC_GEOMETRY}
HEADERS += $${HEADERS_GLC_SHADING} $${HEADERS_GLC_VIEWPORT} $${HEADERS_GLC_3DWIDGET} $${HEADERS_GLC_GLU}
HEADERS += $${HEADERS_GLC_QML} $${HEADERS_CLIP2TR} $${HEADERS_GLC_CSGJS} $${HEADERS_ZLIB}
		   
SOURCES += 3rdparty/zlib/adler32.c \
           3rdparty/zlib/compress.c \
           3rdparty/zlib/crc32.c \
           3rdparty/zlib/deflate.c \
           3rdparty/zlib/gzio.c \
           3rdparty/zlib/inffast.c \
           3rdparty/zlib/inflate.c \
           3rdparty/zlib/inftrees.c \
           3rdparty/zlib/trees.c \
           3rdparty/zlib/uncompr.c \
           3rdparty/zlib/zutil.c


SOURCES += 3rdparty/quazip/qioapi.cpp \
           3rdparty/quazip/JlCompress.cpp \
           3rdparty/quazip/quaadler32.cpp \
           3rdparty/quazip/quacrc32.cpp \
           3rdparty/quazip/quagzipfile.cpp \
           3rdparty/quazip/quaziodevice.cpp \
           3rdparty/quazip/quazip.cpp \
           3rdparty/quazip/quazipdir.cpp \
           3rdparty/quazip/quazipfile.cpp \
           3rdparty/quazip/quazipfileinfo.cpp \
           3rdparty/quazip/quazipnewinfo.cpp \
           3rdparty/quazip/unzip.c \
           3rdparty/quazip/zip.c

SOURCES += 3rdparty/lib3ds/atmosphere.c \
           3rdparty/lib3ds/background.c \
           3rdparty/lib3ds/camera.c \
           3rdparty/lib3ds/chunk.c \
           3rdparty/lib3ds/ease.c \
           3rdparty/lib3ds/file.c \
           3rdparty/lib3ds/io.c \
           3rdparty/lib3ds/light.c \
           3rdparty/lib3ds/material.c \
           3rdparty/lib3ds/matrix.c \
           3rdparty/lib3ds/mesh.c \
           3rdparty/lib3ds/node.c \
           3rdparty/lib3ds/quat.c \
           3rdparty/lib3ds/shadow.c \
           3rdparty/lib3ds/tcb.c \
           3rdparty/lib3ds/tracks.c \
           3rdparty/lib3ds/vector.c \
           3rdparty/lib3ds/viewport.c
   

SOURCES += 3rdparty/clip2tri/clip2tri/clip2tri.cpp \
            3rdparty/clip2tri/clipper/clip_clipper.cpp \
            3rdparty/clip2tri/poly2tri/common/clip_shapes.cc \
            3rdparty/clip2tri/poly2tri/sweep/clip_advancing_front.cc \
            3rdparty/clip2tri/poly2tri/sweep/clip_cdt.cc \
            3rdparty/clip2tri/poly2tri/sweep/clip_sweep.cc \
            3rdparty/clip2tri/poly2tri/sweep/clip_sweep_context.cc

SOURCES +=      3rdparty/csgjs/csgjs.cpp

SOURCES +=	maths/glc_matrix4x4.cpp \
                maths/glc_vector4d.cpp \
                maths/glc_interpolator.cpp \
                maths/glc_plane.cpp \
                maths/glc_geomtools.cpp \
                maths/glc_line3d.cpp \
                maths/glc_triangle.cpp \
                maths/glc_polygon.cpp

SOURCES +=	io/glc_objmtlloader.cpp \
                io/glc_objtoworld.cpp \
                io/glc_stltoworld.cpp \
                io/glc_offtoworld.cpp \
                io/glc_3dstoworld.cpp \
                io/glc_3dxmltoworld.cpp \
                io/glc_colladatoworld.cpp \
                io/glc_worldto3dxml.cpp \
                io/glc_worldto3ds.cpp \
                io/glc_bsreptoworld.cpp \
                io/glc_fileloader.cpp \
                io/glc_worldtoobj.cpp \
                io/glc_assimptoworld.cpp \
                io/glc_worldtocollada.cpp

SOURCES +=	sceneGraph/glc_3dviewcollection.cpp \
                sceneGraph/glc_3dviewinstance.cpp \
                sceneGraph/glc_structreference.cpp \
                sceneGraph/glc_structinstance.cpp \
                sceneGraph/glc_world.cpp \
                sceneGraph/glc_attributes.cpp \
                sceneGraph/glc_worldhandle.cpp \
                sceneGraph/glc_spacepartitioning.cpp \
                sceneGraph/glc_octree.cpp \
                sceneGraph/glc_octreenode.cpp \
                sceneGraph/glc_selectionset.cpp \
                sceneGraph/glc_structoccurrence.cpp

SOURCES +=	geometry/glc_geometry.cpp \
                geometry/glc_circle.cpp \
                geometry/glc_cylinder.cpp \
                geometry/glc_point.cpp \
                geometry/glc_box.cpp \
                geometry/glc_meshdata.cpp \
                geometry/glc_primitivegroup.cpp \
                geometry/glc_mesh.cpp \
                geometry/glc_lod.cpp \
                geometry/glc_rectangle.cpp \
                geometry/glc_line.cpp \
                geometry/glc_rep.cpp \
                geometry/glc_3drep.cpp \
                geometry/glc_pointsprite.cpp \
                geometry/glc_bsrep.cpp \
                geometry/glc_wiredata.cpp \
                geometry/glc_arrow.cpp \
                geometry/glc_polylines.cpp \
                geometry/glc_disc.cpp \
                geometry/glc_cone.cpp \
                geometry/glc_sphere.cpp \
                geometry/glc_pointcloud.cpp \
                geometry/glc_extrudedmesh.cpp \
                geometry/glc_text.cpp \
                geometry/glc_csghelper.cpp \
                geometry/glc_csgnode.cpp \
                geometry/glc_csgoperatornode.cpp \
                geometry/glc_csgleafnode.cpp \
                geometry/glc_lathemesh.cpp \
                geometry/glc_image.cpp



SOURCES +=	shading/glc_material.cpp \
                shading/glc_texture.cpp \
                shading/glc_light.cpp \
                shading/glc_selectionmaterial.cpp \
                shading/glc_shader.cpp \
                shading/glc_renderproperties.cpp \
                shading/glc_renderer.cpp

SOURCES +=	viewport/glc_camera.cpp \
                viewport/glc_imageplane.cpp \
                viewport/glc_viewport.cpp \
                viewport/glc_movercontroller.cpp\
                viewport/glc_mover.cpp \
                viewport/glc_panmover.cpp \
                viewport/glc_repmover.cpp \
                viewport/glc_repcrossmover.cpp \
                viewport/glc_zoommover.cpp \
                viewport/glc_trackballmover.cpp \
                viewport/glc_reptrackballmover.cpp \
                viewport/glc_settargetmover.cpp \
                viewport/glc_turntablemover.cpp \
                viewport/glc_frustum.cpp \
                viewport/glc_flymover.cpp \
                viewport/glc_repflymover.cpp \
                viewport/glc_userinput.cpp \
                viewport/glc_tsrmover.cpp \
                viewport/glc_viewhandler.cpp \
                viewport/glc_openglviewhandler.cpp \
                viewport/glc_inputeventinterpreter.cpp \
                viewport/glc_defaulteventinterpreter.cpp \
                viewport/glc_screenshotsettings.cpp \
                viewport/glc_openglviewwidget.cpp

		
SOURCES +=	glc_global.cpp \
                glc_object.cpp \
                glc_factory.cpp \
                glc_boundingbox.cpp \
                glc_exception.cpp \
                glc_openglexception.cpp \
                glc_fileformatexception.cpp \
                glc_ext.cpp \
                glc_state.cpp \
                glc_cachemanager.cpp \
                glc_renderstatistics.cpp \
                glc_log.cpp \
                glc_errorlog.cpp \
                glc_tracelog.cpp \
                glc_context.cpp \
                glc_contextmanager.cpp \
                glc_contextshareddata.cpp \
                glc_uniformshaderdata.cpp \
                glc_selectionevent.cpp \
                glc_renderstate.cpp

SOURCES +=	3DWidget/glc_3dwidget.cpp \
                3DWidget/glc_cuttingplane.cpp \
                3DWidget/glc_3dwidgetmanager.cpp \
                3DWidget/glc_3dwidgetmanagerhandle.cpp \
                3DWidget/glc_abstractmanipulator.cpp \
                3DWidget/glc_pullmanipulator.cpp \
                3DWidget/glc_rotationmanipulator.cpp \
                3DWidget/glc_planemanipulator.cpp \
                3DWidget/glc_axis.cpp
			
SOURCES +=	glu/glc_project.cpp

SOURCES +=      qml/glc_quickitem.cpp \
                qml/glc_quickview.cpp \
                qml/glc_quickcamera.cpp \
                qml/glc_quickoccurrence.cpp \
                qml/glc_quickselection.cpp \
                qml/glc_quickviewhandler.cpp

# Windows compilation configuration
win32:CONFIG *= dll

# install header
HEADERS_INST = GLC_BoundingBox \
               GLC_Box \
               GLC_Camera \
               GLC_Circle \
               GLC_3DViewCollection \
               GLC_Cylinder \
               GLC_Exception \
               GLC_Factory \
               GLC_FileFormatException \
               GLC_Geometry \
               GLC_ImagePlane \
               GLC_3DViewInstance \
               GLC_Interpolator \
               GLC_Light \
               GLC_Material \
               GLC_Matrix4x4 \
               GLC_Node \
               GLC_Object \
               GLC_OpenGlException \
               GLC_Point \
               GLC_Point2d \
               GLC_Point2df \
               GLC_Point3d \
               GLC_Point3df \
               GLC_Texture \
               GLC_Vector2d \
               GLC_Vector2df \
               GLC_Vector3d \
               GLC_Vector3df \
               GLC_Vector4d \
               GLC_Viewport \
               GLC_World \
               GLC_Shader \
               GLC_SelectionMaterial \
               GLC_State \
               GLC_Mover \
               GLC_MoverController \
               GLC_PanMover \
               GLC_ZoomMover \
               GLC_TrackBallMover \
               GLC_RepMover \
               GLC_RepCrossMover \
               GLC_RepTrackBallMover \
               GLC_TurnTableMover \
               GLC_Attributes \
               GLC_Rectangle \
               GLC_Mesh \
               GLC_StructOccurrence \
               GLC_StructInstance \
               GLC_StructReference \
               GLC_Line \
               GLC_Rep \
               GLC_3DRep \
               GLC_PointSprite \
               GLC_CacheManager \
               GLC_BSRep \
               GLC_RenderProperties \
               GLC_Global \
               GLC_SpacePartitioning \
               GLC_Octree \
               GLC_OctreeNode \
               GLC_Plane \
               GLC_Frustum \
               GLC_GeomTools \
               GLC_Line3d \
               GLC_3DWidget \
               GLC_CuttingPlane \
               GLC_3DWidgetManager \
               GLC_3DWidgetManagerHandle \
               GLC_Arrow \
               GLC_Polylines \
               GLC_Disc \
               GLC_AbstractManipulator \
               GLC_PullManipulator \
               GLC_RotationManipulator \
               GLC_FlyMover \
               GLC_RepFlyMover \
               GLC_WorldTo3dxml \
               GLC_WorldTo3ds \
               GLC_RenderStatistics \
               GLC_Ext \
               GLC_Cone \
               GLC_Sphere \
               GLC_Axis \
               GLC_Log \
               GLC_ErrorLog \
               GLC_TraceLog \
               glcXmlUtil \
               GLC_RenderState \
               GLC_FileLoader \
               GLC_WorldReaderPlugin \
               GLC_WorldReaderHandler \
               GLC_PointCloud \
               GLC_SelectionSet \
               GLC_UserInput \
               GLC_TsrMover \
               GLC_Glu \
               GLC_Context \
               GLC_ContextManager \
               GLC_Renderer \
               GLC_ExtrudedMesh \
               GLC_LatheMesh \
               GLC_QuickItem \
               GLC_ViewHandler \
               GLC_InputEventInterpreter \
               GLC_SelectionEvent \
               GLC_ScreenShotSettings \
               GLC_QuickView \
               GLC_QuickCamera \
               GLC_QuickOccurrence \
               GLC_QuickViewHandler \
               GLC_OpenGLViewHandler \
               GLC_QuickSelection \
               GLC_OpenGLViewWidget \
               GLC_Text \
               GLC_PlaneManipulator \
               GLC_CsgHelper \
               GLC_CsgNode \
               GLC_CsgOperatorNode \
               GLC_CsgLeafNode \
               GLC_Triangle \
               GLC_LatheMesh \
               GLC_Polygon \
               GLC_OpenGLViewInterface \
               GLC_WorldToCollada \
               GLC_Image


include (../../install.pri)

include.path = $${INCLUDE_DIR}
include_lib3ds.path = $${INCLUDE_DIR}/3rdparty/lib3ds
include_glext.path = $${INCLUDE_DIR}/3rdparty/glext
include_quazip.path = $${INCLUDE_DIR}/3rdparty/quazip
include_zlib.path = $${INCLUDE_DIR}/3rdparty/zlib
include_glc_maths.path = $${INCLUDE_DIR}/maths
include_glc_io.path = $${INCLUDE_DIR}/io
include_glc_scengraph.path = $${INCLUDE_DIR}/sceneGraph
include_glc_geometry.path = $${INCLUDE_DIR}/geometry
include_glc_shading.path = $${INCLUDE_DIR}/shading
include_glc_viewport.path = $${INCLUDE_DIR}/viewport
include_glc_3dwidget.path = $${INCLUDE_DIR}/3DWidget
include_glc_glu.path = $${INCLUDE_DIR}/glu
include_glc_qml.path = $${INCLUDE_DIR}/qml

include.files = $${HEADERS_GLC} $${HEADERS_INST}
include_lib3ds.files = $${HEADERS_LIB3DS}
include_glext.files =$${HEADERS_GLEXT}
include_quazip.files = $${HEADERS_QUAZIP}
include_zlib.files = $${HEADERS_ZLIB}
include_glc_maths.files= $${HEADERS_GLC_MATHS}
include_glc_io.files= $${HEADERS_GLC_IO}
include_glc_scengraph.files= $${HEADERS_GLC_SCENEGRAPH}
include_glc_geometry.files= $${HEADERS_GLC_GEOMETRY}
include_glc_shading.files = $${HEADERS_GLC_SHADING}
include_glc_viewport.files = $${HEADERS_GLC_VIEWPORT}
include_glc_3dwidget.files = $${HEADERS_GLC_3DWIDGET}
include_glc_glu.files = $${HEADERS_GLC_GLU}
include_glc_qml.files = $${HEADERS_GLC_QML}

# install library
target.path = $${LIB_DIR}
   
# "make install" configuration options
INSTALLS += include_lib3ds include_glext include_quazip include_zlib include_glc_maths include_glc_io
INSTALLS += include_glc_scengraph include_glc_geometry include_glc_shading include_glc_viewport
INSTALLS += include_glc_3dwidget include_glc_glu include_glc_qml

INSTALLS += target
INSTALLS += include


DISTFILES += \
    ../../sonar-project.properties




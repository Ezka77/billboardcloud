# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src
# Target is an application:  bin/ter

FORMS += W_Main.ui \
         W_Density.ui 
HEADERS += Camera.h \
           Color.h \
           Entity.h \
           Light.h \
           Material.h \
           Matrix3.h \
           Matrix4.h \
           Mesh.h \
           OpenGL.h \
           Plane.h \
           Quaternion.h \
           ShadowMap.h \
           Scene.h \
           TextureLayer.h \
           Timer.h \
           Transform.h \
           Vector3.h \
           Viewer.h \
           Density.h \
           Billboard.h \
           BillboardCloud.h \
           BBWindow.h \
           TgaLoader.h \
           OffScreenRendering.h \
           DensityViewer.h \
           Image.h \
           textfile.h
SOURCES += Camera.cpp \
           Color.cpp \
           Entity.cpp \
           Light.cpp \
           main.cpp \
           Material.cpp \
           Matrix3.cpp \
           Matrix4.cpp \
           Mesh.cpp \
           Plane.cpp \
           Quaternion.cpp \
           ShadowMap.cpp \
           Scene.cpp \
           TextureLayer.cpp \
           Transform.cpp \
           Vector3.cpp \
           Viewer.cpp \
           Density.cpp \
           Billboard.cpp \
           BillboardCloud.cpp \
           BBWindow.cpp \
           TgaLoader.cpp \
           OffScreenRendering.cpp \
           DensityViewer.cpp \
           Image.cpp \
           textfile.cpp
TEMPLATE = app
CONFIG += release \
warn_on \
thread \
qt\
opengl\
glew \
glut
TARGET = bin/ter
CPU = ""$system(uname -m)""
RESOURCES = application.qrc
MOC_DIR = ../obj/moc
QT += opengl
glut {
    unix {
        LIBS += -L/usr/X11R6/lib -lglut -lXi -lGLEW -lGL -lX11 -lpng12
    }
}
debug {
    DEFINES += DEBUG
    OBJECTS_DIR = ../obj/debug
}
release {
    OBJECTS_DIR = ../obj/release
}
DESTDIR += ../ \
 ..
PATH += .

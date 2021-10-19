QT += quick multimedia

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        CorePlayer.cpp \
        GstBinding.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

LIBS += -LC:/gstreamer/1.0/mingw_x86/bin
LIBS += -llibgio-2.0-0 -llibgstapp-1.0-0 -llibgstbase-1.0-0 \
        -llibgstreamer-1.0-0 -llibgobject-2.0-0 -llibglib-2.0-0

INCLUDEPATH += $$PWD/include \
               C:/gstreamer/1.0/mingw_x86/include \
               C:/gstreamer/1.0/mingw_x86/include/gstreamer-1.0 \
               C:/gstreamer/1.0/mingw_x86/include/glib-2.0 \
               C:/gstreamer/1.0/mingw_x86/include/glib-2.0/gio \
               C:/gstreamer/1.0/mingw_x86/include/glib-2.0/glib \
               C:/gstreamer/1.0/mingw_x86/include/glib-2.0/gobject \
               C:/gstreamer/1.0/mingw_x86/include/gio-win32-2.0

DEPENDPATH += $$PWD/include \
              C:/gstreamer/1.0/mingw_x86/include \
              C:/gstreamer/1.0/mingw_x86/include/gstreamer-1.0 \
              C:/gstreamer/1.0/mingw_x86/include/glib-2.0 \
              C:/gstreamer/1.0/mingw_x86/include/glib-2.0/gio \
              C:/gstreamer/1.0/mingw_x86/include/glib-2.0/glib \
              C:/gstreamer/1.0/mingw_x86/include/glib-2.0/gobject \
              C:/gstreamer/1.0/mingw_x86/include/gio-win32-2.0

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CorePlayer.h \
    GstBinding.h \
    RawPlayer.h

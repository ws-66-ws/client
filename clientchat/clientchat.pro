QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = icon.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bubblebase.cpp \
    chatmessagewidget.cpp \
    chatuserwidget.cpp \
    chatwidget.cpp \
    clickablelabel.cpp \
    conuseritem.cpp \
    global.cpp \
    httpmgr.cpp \
    listitembase.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    messagetextedit.cpp \
    profilecardwidget.cpp \
    registerwidget.cpp \
    tcpmgr.cpp \
    textbubble.cpp \
    timerbtn.cpp \
    userdata.cpp \
    usermgr.cpp

HEADERS += \
    Singleton.h \
    bubblebase.h \
    chatmessagewidget.h \
    chatuserwidget.h \
    chatwidget.h \
    clickablelabel.h \
    conuseritem.h \
    global.h \
    httpmgr.h \
    listitembase.h \
    loginwidget.h \
    mainwindow.h \
    messagetextedit.h \
    profilecardwidget.h \
    registerwidget.h \
    tcpmgr.h \
    textbubble.h \
    timerbtn.h \
    userdata.h \
    usermgr.h

FORMS += \
    chatuserwidget.ui \
    chatwidget.ui \
    chatwidget.ui \
    conuseritem.ui \
    loginwidget.ui \
    mainwindow.ui \
    profilecardwidget.ui \
    registerwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.ini \
    resource/edit_24.svg

RESOURCES += \
    resource.qrc

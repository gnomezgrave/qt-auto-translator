QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AboutWnd.cpp \
    ChangeLanguage.cpp \
    FileIO.cpp \
    HowToUseWnd.cpp \
    QtTranslator.cpp \
    main.cpp

HEADERS += \
    AboutWnd.h \
    ChangeLanguage.h \
    FileIO.h \
    HowToUseWnd.h \
    QtTranslator.h \
    version.h

FORMS += \
    About.ui \
    ChangeLanguageWnd.ui \
    HowToUseWnd.ui \
    QtTranslator.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    QtTranslator.qrc

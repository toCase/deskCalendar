 #-------------------------------------------------
#
# Project created by QtCreator 2012-07-02T22:51:32
#
#-------------------------------------------------

QT       += core gui sql webkit

TARGET = deskCalendar
TEMPLATE = app


SOURCES += main.cpp\
        deskcalendar.cpp \
    event.cpp \
    viewDia.cpp

HEADERS  += deskcalendar.h \
    event.h \
    viewDia.h

FORMS    += deskcalendar.ui \
    event.ui \
    viewDia.ui

OTHER_FILES += iconca.rc

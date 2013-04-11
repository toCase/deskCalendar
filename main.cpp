#include <QtGui/QApplication>
#include <QDesktopWidget>
#include <QTextCodec>
#include "deskcalendar.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    DeskCalendar *w = new DeskCalendar(a.applicationDirPath(), a.desktop()->height(), a.desktop()->width());

/*
    QString strCSS("QMainWindow {border: 3px solid rgb(0, 0, 250); "
                       "border-radius: 12px; "
                       "padding: 2px 4px; "
                       "color: rgb(220,20,60); "
                       "background-color: rgb(240, 230, 140);} "
                   "QToolButton {border: 2px solid rgb(0, 0, 250); "
                   "border-radius: 5px;"
                   "color: rgb(0, 0, 250);}");
    a.setStyleSheet(strCSS);
    */
    w->show();
    return a.exec();
}

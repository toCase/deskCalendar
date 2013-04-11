#ifndef VIEWDIA_H
#define VIEWDIA_H

#include <QtWebKit/QtWebKit>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include "ui_viewDia.h"

class viewDia:public QMainWindow{
    Q_OBJECT
public:
    viewDia(QString _text, QString _path, QWidget *parent = 0);
    Ui::viewer ui;
    QString text;
    QString appPath;

public slots:
    void load();
    void print();
    void loadIcons();
};

#endif // VIEWDIA_H

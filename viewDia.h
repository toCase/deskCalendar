#ifndef VIEWDIA_H
#define VIEWDIA_H

#include <QtWebKit/QtWebKit>
#include "ui_viewDia.h"

class viewDia:public QMainWindow{
    Q_OBJECT
public:
    viewDia(QString _text, QWidget *parent = 0);
    Ui::viewer ui;
    QString text;

public slots:
    void load();
    void print();
};

#endif // VIEWDIA_H

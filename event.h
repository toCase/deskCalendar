#ifndef EVENT_H
#define EVENT_H

#include "ui_event.h"
#include <QtSql/QtSql>
#include <QMessageBox>

class Events:public QDialog{
    Q_OBJECT
public:
    Events(QList<int>_l, int _i, QDate _d, QWidget *parent = 0);
    Ui::events_dialog ui;
    QList<int> list;
    int item;
    QDate dat;

private slots:
    void createEvent();

    void saveEvent();
    void deleteEvent();

    void toFirst();
    void toPrev();
    void toNext();
    void toLast();
    ///////
    void setPeriodicly();
    void setFullDay(bool x);

};

#endif // EVENT_H

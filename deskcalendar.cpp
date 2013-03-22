#include "deskcalendar.h"
#include "ui_deskcalendar.h"
#include "event.h"
#include <QPainter>


DeskCalendar::DeskCalendar(int h, int w, QWidget *parent) :QMainWindow(parent),
    ui(new Ui::DeskCalendar)  {
    ui->setupUi(this);
    /*
    #ifdef Q_WS_WIN
    #define WFLAGS Qt::Widget
    #else
    #define WFLAGS Qt::X11BypassWindowManagerHint
    #endif
    window()->setWindowFlags( Qt::Window |   WFLAGS ); //Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint |a

*/
    //время
    dc_time();
    dc_date();

    connectDB();
    ui->groupBox_notes->hide();


   // window()->setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(w-window()->width(), 30, window()->width(), h - 75);

    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(addNotes()));
    connect(ui->pushButton_close_event, SIGNAL(clicked()), this, SLOT(cancelNotes()));


    QTimer *timerA = new QTimer(this);
    timerA->start(60000);
    connect(timerA, SIGNAL(timeout()), this, SLOT(updateCalendar()));

    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(saveNotes()));
    connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->tableWidget_notes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editNotes()));
    //connect(ui->pushButton_check, SIGNAL(clicked()), this, SLOT(checkNotes()));

    connect(ui->calendar, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentDate()));
    connect(ui->toolButton_prev, SIGNAL(clicked()), this, SLOT(prevMonth()));
    connect(ui->toolButton_next, SIGNAL(clicked()), this, SLOT(nextMonth()));
    updateCalendar();


}

DeskCalendar::~DeskCalendar()
{
    db.close();
    delete ui;
}

// время
void DeskCalendar::dc_time(){
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    setCurrentTime();
    connect(timer, SIGNAL(timeout()), this, SLOT(setCurrentTime()));
}

void DeskCalendar::setCurrentTime(){
    ui->clock->setTime(QTime::currentTime());
}
////////////////////
// дата
void DeskCalendar::dc_date(){
    ui->dateEdit_day->setDate(QDate::currentDate());
    ui->dateEdit_month->setDate(QDate::currentDate());
    ui->dateEdit_year->setDate(QDate::currentDate());
    cDay = QDate::currentDate().day();
    cMonth = QDate::currentDate().month();
    cYear = QDate::currentDate().year();
    cDate = QDate::currentDate();
}
void DeskCalendar::setCurrentDate(){
   if (ui->calendar->item(ui->calendar->currentRow(), ui->calendar->currentColumn())->text() > " "){
        cDay = ui->calendar->item(ui->calendar->currentRow(), ui->calendar->currentColumn())->text().toInt();
        QString d;
        d.append(QString("%1-").arg(cYear));
        if (cMonth < 10){
            d.append(QString("0%1-").arg(cMonth));
        } else {
            d.append(QString("%1-").arg(cMonth));
        }
        if (cDay < 10){
            d.append(QString("0%1").arg(cDay));
        } else {
            d.append(QString("%1").arg(cDay));
        }
        cDate = QDate::fromString(d, "yyyy-MM-dd");
        ui->label_events->setText(QString("Events on %1").arg(cDate.toString("d MMM yyyy")));
        //makeCalendar(cDate);
        readNotes();
    }
}
void DeskCalendar::prevMonth(){
    if (cMonth > 1){
        cMonth = cMonth - 1;
    } else if (cMonth == 1){
        cMonth = 12;
        cYear = cYear - 1;
    }
    QString d;
    d.append(QString("%1-").arg(cYear));
    if (cMonth < 10){
        d.append(QString("0%1-").arg(cMonth));
    } else {
        d.append(QString("%1-").arg(cMonth));
    }
    d.append("01");
    cDate = QDate::fromString(d, "yyyy-MM-dd");
    makeCalendar(cDate);
     readNotes();
}
void DeskCalendar::nextMonth(){
    if (cMonth < 12){
        cMonth = cMonth + 1;
    } else if (cMonth == 12){
        cMonth = 1;
        cYear = cYear + 1;
    }
    QString d;
    d.append(QString("%1-").arg(cYear));
    if (cMonth < 10){
        d.append(QString("0%1-").arg(cMonth));
    } else {
        d.append(QString("%1-").arg(cMonth));
    }
    d.append("01");
    cDate = QDate::fromString(d, "yyyy-MM-dd");
    makeCalendar(cDate);
    readNotes();
}

////////////////////
// база данных
void DeskCalendar::connectDB(){
    db =  QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dCalendar.db3");
    db.open();
}
///////////////////
// заметки
void DeskCalendar::addNotes(){

    ui->groupBox_notes->setVisible(true);
    id = 0;
    ui->lineEdit_title->clear();
    ui->dateEdit_event->setDate(cDate);
    ui->timeEdit_s->setTime(ui->clock->time());
    ui->timeEdit_e->setTime(ui->clock->time().addSecs(1800));
    /*
    QList<int> list;
    Events *ev = new Events(list, 0, cDate, this);
    ev->exec();
    makeCalendar(cDate);
    */
}

void DeskCalendar::editNotes(){
    /*
    id = ui->tableWidget_notes->item(ui->tableWidget_notes->currentRow(), 0)->text().toInt();
    QSqlQuery r(QString("select cale.id, cale.ctime, cale.cdate, cale.ctext,  cale.imp from cale "
                        "where cale.id = \'%1\' ").arg(id));
    r.next();
    ui->timeEdit_notes->setTime(r.value(1).toTime());
    ui->dateEdit_notes->setDate(r.value(2).toDate());
    ui->textEdit_text->setPlainText(r.value(3).toString());
    ui->checkBox_imp->setChecked(r.value(4).toBool());
    ui->groupBox_notes->setVisible(true);
    */
    //***************
    QList<int> list;
    for (int a = 0; a < ui->tableWidget_notes->rowCount(); a++){
        list << ui->tableWidget_notes->item(a, 0)->text().toInt();
    }
    Events *ev = new Events(list, ui->tableWidget_notes->currentRow(), cDate, this);
    ev->exec();
    readNotes();
    makeCalendar(cDate);
}

void DeskCalendar::cancelNotes(){
    ui->groupBox_notes->hide();
}

void DeskCalendar::saveNotes(){    
    if (!ui->lineEdit_title->text().isEmpty()){
        QSqlQuery query("insert into events (name, date_s, time_s, time_e, color) "
                        "values (:name, :date_s, :time_s, :time_e, :color)");
        query.bindValue(0, ui->lineEdit_title->text());
        query.bindValue(1, ui->dateEdit_event->date().toString("yyyy-MM-dd"));
        query.bindValue(2, ui->timeEdit_s->time().toString("hh:mm"));
        query.bindValue(3, ui->timeEdit_e->time().toString("hh:mm"));
        query.bindValue(4, "lightskyblue");
        query.exec();
    }
    ui->pushButton_detail->setEnabled(true);
    readNotes();
}

void DeskCalendar::readNotes(){
    ui->tableWidget_notes->setColumnHidden(0, true);
    for (int a = ui->tableWidget_notes->rowCount(); a >= 0; a--){
        ui->tableWidget_notes->removeRow(a);
    }
    QSqlQuery r(QString("select events.id, events.time_s, events.time_e, events.name, events.color from events "
                        "where events.date_s = \'%1\' order by events.time_s ").arg(cDate.toString("yyyy-MM-dd")));
    int row = 0;
    while (r.next()){
        ui->tableWidget_notes->insertRow(row);

        QTableWidgetItem *id = new QTableWidgetItem(r.value(0).toString());
        ui->tableWidget_notes->setItem(row, 0, id);

        QString events(QString("%1 - %2  %3") // строка сообщения
                       .arg(r.value(1).toTime().toString("hh:mm"))
                       .arg(r.value(2).toTime().toString("hh:mm"))
                       .arg(r.value(3).toString()));
        QTableWidgetItem *notes = new QTableWidgetItem(events);
        QBrush brush;

        QColor color(QString("%1").arg(r.value(4).toString()));
        brush.setColor("white");
        notes->setBackgroundColor(color);
        notes->setForeground(brush);
        //notes->setBackground(brush);

        ui->tableWidget_notes->setItem(row, 1, notes);
        row++;
    }

    if (ui->tableWidget_notes->rowCount() == 0){
        //ui->pushButton_check->setEnabled(false);
    } else {
        //ui->pushButton_check->setEnabled(true);
    }

    ui->tableWidget_notes->resizeRowsToContents();
}

void DeskCalendar::checkNotes(){
    /*
    ui->tableWidget_notes->setFocus();
    id = ui->tableWidget_notes->item(ui->tableWidget_notes->currentRow(), 0)->text().toInt();
    if (id > 0){
        QSqlQuery up(QString("update cale set status = \'%1\' where cale.id = \'%2\' ")
                     .arg("2")
                     .arg(id));
        up.exec();
    }
    readNotes();
    makeCalendar(cDate);
    */
}

void DeskCalendar::updateNotes(){
    /*
    QDate firstDate = QDate::fromString(QString("%1-01").arg(cDate.toString("yyyy-MM")), "yyyy-MM-dd");
    QDate lastDate = QDate::fromString(QString("%1-%2")
                                       .arg(cDate.toString("yyyy-MM"))
                                       .arg(cDate.daysInMonth()), "yyyy-MM-dd");
    for (int d = firstDate.toJulianDay(); d < lastDate.toJulianDay(); d++){
        QDate resDate = QDate::fromJulianDay(d);
        QSqlQuery q(QString("select cale.id, cale.cdate, cale.ctime, cale.status from cale where cale.cdate = \'%1\' and status = 1 ")
                    .arg(resDate.toString("yyyy-MM-dd")));
        while (q.next()){
            if (q.value(1).toDate() < QDate::currentDate()){
                QSqlQuery up(QString("update cale set status = 3 where cale.id = \'%1\' ").arg(q.value(0).toInt()));
                up.exec();
            } else if ((q.value(1).toDate() == QDate::currentDate() and q.value(2).toTime() <= QTime::currentTime())){
                QSqlQuery up(QString("update cale set status = 3 where cale.id = \'%1\' ").arg(q.value(0).toInt()));
                up.exec();
            }
        }
    }
    */

}

//календарь
void DeskCalendar::makeCalendar(QDate ddate){
    //clearing
    for (int a = ui->calendar->rowCount(); a >= 0; a--){
        ui->calendar->removeRow(a);
    }
    //определяем день недели первого числа месяца и дни первой недели
    QDate firstDate = QDate::fromString(QString("%1-01").arg(ddate.toString("yyyy-MM")), "yyyy-MM-dd");

    int dd = 0;
    int row = 0;
    ui->calendar->insertRow(row);
    for (int col = 0; col < 7; col++){
        QDate resDate = QDate::fromJulianDay(firstDate.toJulianDay() + dd);
        if (resDate.dayOfWeek() - 1 == col){
            QTableWidgetItem *item = new QTableWidgetItem(resDate.toString("d"));
            if (col >= 5){ // выходные*
                item->setForeground(Qt::red);
            }
            if (resDate == QDate::currentDate()){ //сегодня
                item->setBackgroundColor("mediumorchid");
            }
            // иконки событий
            /*
            QSqlQuery q(QString("select cale.id, cale.status, cale.imp from cale where cale.cdate = \'%1\' ")
                        .arg(resDate.toString("yyyy-MM-dd")));
            int b = 0;
            int r = 0;
            int o = 0;
            int c = 0;
            while (q.next()){
                if (q.value(1).toInt() == 3){
                    r++;
                }
                if (q.value(2).toInt() == 2 and q.value(1) == 1){
                    o++;
                }
                if (q.value(2).toInt() == 0 and q.value(1) == 1){
                    b++;
                }
                if (q.value(1).toInt() == 1){
                    c++;
                }
            }
            if (r > 0){
                QIcon ico("icons/rcube.png");
                item->setIcon(ico);
            } else {
                if (o > 0){
                    QIcon ico("icons/ycube.png");
                    item->setIcon(ico);
                } else {
                    if (b > 0){
                        QIcon ico("icons/bcube.png");
                        item->setIcon(ico);
                    } else {
                        if (c > 0){
                            QIcon ico("icons/done.png");
                            item->setIcon(ico);
                        }
                    }
                }
            }
            */
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->calendar->setItem(0, col, item);
            dd = dd + 1;
        } else {
            QTableWidgetItem *item = new QTableWidgetItem(" ");
            ui->calendar->setItem(0, col, item);
        }
    }

    //определяем кол-во недель в месяце
    QDate lastDate = QDate::fromString(QString("%1-%2")
                                       .arg(ddate.toString("yyyy-MM"))
                                       .arg(ddate.daysInMonth()), "yyyy-MM-dd");
    int rowC = 0;
    if (lastDate.weekNumber() < firstDate.weekNumber()){ // проблема 53 недели
        rowC = 53 - firstDate.weekNumber();
    } else {
        rowC = lastDate.weekNumber() - firstDate.weekNumber();
    }
    ui->calendar->setRowCount(rowC + 1);
    //добавляем остальные числа
    for (int row = 1; row <= rowC + 1; row++ ){
        for (int col = 0; col < 7; col++){
            QDate resDate = QDate::fromJulianDay(firstDate.toJulianDay() + dd);
            if (resDate.dayOfWeek() - 1 == col){
                QTableWidgetItem *item = new QTableWidgetItem(resDate.toString("d"));
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                ui->calendar->setItem(row, col, item);
                if (col >= 5){ // выходные*
                    item->setForeground(Qt::red);
                }
                if (resDate == QDate::currentDate()){ //сегодня
                    item->setBackgroundColor("mediumorchid");
                }
                // иконки событий
                /*
                QSqlQuery q(QString("select cale.id, cale.status, cale.imp from cale where cale.cdate = \'%1\' ")
                            .arg(resDate.toString("yyyy-MM-dd")));

                int b = 0;
                int r = 0;
                int o = 0;
                int c = 0;
                while (q.next()){
                    if (q.value(1).toInt() == 3){
                        r++;
                    }
                    if (q.value(2).toInt() == 2 and q.value(1) == 1){
                        o++;
                    }
                    if (q.value(2).toInt() == 0 and q.value(1) == 1){
                        b++;
                    }
                    if (q.value(1).toInt() == 2){
                        c++;
                    }
                }
                if (r > 0){
                    QIcon ico("icons/rcube.png");
                    item->setIcon(ico);
                } else {
                    if (o > 0){
                        QIcon ico("icons/ycube.png");
                        item->setIcon(ico);
                    } else {
                        if (b > 0){
                            QIcon ico("icons/bcube.png");
                            item->setIcon(ico);
                        } else {
                            if (c > 0){
                                QIcon ico("icons/done.png");
                                item->setIcon(ico);
                            }
                        }
                    }
                }
                */
                //
                dd = dd + 1;
            }
            if (resDate > lastDate){
                QTableWidgetItem *item = new QTableWidgetItem(" ");
                ui->calendar->setItem(row, col, item);
            }
        }
    }
    //ui->calendar->resizeColumnsToContents();
    ui->label_events->setText(QString("Events on %1").arg(ddate.toString("d MMM yyyy")));
}

void DeskCalendar::updateCalendar(){
    updateNotes();
    readNotes();
    makeCalendar(cDate);
}

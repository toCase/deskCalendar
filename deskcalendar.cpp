#include "deskcalendar.h"
#include "ui_deskcalendar.h"
#include "event.h"
#include <QPainter>
#include "viewDia.h"



DeskCalendar::DeskCalendar(QString _path, int h, int w, QWidget *parent) :QMainWindow(parent),
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
    connectDB();
    testDB();

    appPath = _path;
    loadIcons();
    //время
    dc_time();
    dc_date();


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

    connect(ui->calendar, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentDate()));
    connect(ui->toolButton_prev, SIGNAL(clicked()), this, SLOT(prevMonth()));
    connect(ui->toolButton_next, SIGNAL(clicked()), this, SLOT(nextMonth()));
    connect(ui->toolButton_dia, SIGNAL(clicked()), this, SLOT(toDia()));
    connect(ui->toolButton_about, SIGNAL(clicked()), this, SLOT(toInfo()));
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

    QSqlQuery remQuery(QString("select events.id, events.name, events.date_s, events.time_s, events.color "
                               "from events where events.rem_date = \'%1\' and events.rem_time = \'%2\' ")
                       .arg(cDate.toString("yyyy-MM-dd"))
                       .arg(QTime::currentTime().toString("hh:mm:ss")));
    remQuery.next();

    if (remQuery.isValid()){
        QString remind(QString("<h3 align=center>%1<br>%2</h3><p><b>%3</b></p>")
                       .arg(remQuery.value(2).toString())
                       .arg(remQuery.value(3).toString())
                       .arg(remQuery.value(1).toString()));
        QMessageBox messa;
        messa.setWindowTitle(tr("Reminder"));
        messa.setWindowFlags(Qt::WindowStaysOnTopHint);
        messa.setModal(true);
        messa.setText(remind);
        messa.exec();
    }


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
    ui->lineEdit_title->setFocus();
}

void DeskCalendar::editNotes(){
    QList<int> list;
    for (int a = 0; a < ui->tableWidget_notes->rowCount(); a++){
        list << ui->tableWidget_notes->item(a, 0)->text().toInt();
    }
    Events *ev = new Events(list, ui->tableWidget_notes->currentRow(), cDate, appPath, this);
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
        query.bindValue(2, ui->timeEdit_s->time().toString("hh:mm:ss"));
        query.bindValue(3, ui->timeEdit_e->time().toString("hh:mm:ss"));
        query.bindValue(4, "lightskyblue");
        query.exec();
    }
    //ui->pushButton_detail->setEnabled(true);
    readNotes();
    makeCalendar(cDate);
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

        ui->tableWidget_notes->setItem(row, 1, notes);
        row++;
    }
    ui->tableWidget_notes->resizeRowsToContents();
}

void DeskCalendar::checkNotes(){
}

void DeskCalendar::updateNotes(){
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
                item->setForeground(Qt::white);
            }
            // иконки событий
            QSqlQuery q(QString("select Count(events.id) from events where events.date_s = \'%1\' ")
                        .arg(resDate.toString("yyyy-MM-dd")));
            q.next();
            if (q.value(0).toInt() > 0){
                item->setForeground(Qt::blue);
                item->setBackgroundColor(Qt::green);
            }
            //***
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
                    item->setForeground(Qt::white);
                }
                // иконки событий
                QSqlQuery q(QString("select Count(events.id) from events where events.date_s = \'%1\' ")
                            .arg(resDate.toString("yyyy-MM-dd")));
                q.next();
                if (q.value(0).toInt() > 0){
                    item->setForeground(Qt::blue);
                    item->setBackgroundColor(Qt::green);
                }
                //
                dd = dd + 1;
            }
            if (resDate > lastDate){
                QTableWidgetItem *item = new QTableWidgetItem(" ");
                ui->calendar->setItem(row, col, item);
            }
        }
    }
    ui->label_events->setText(QString("Events on %1").arg(ddate.toString("d MMM yyyy")));
}

void DeskCalendar::updateCalendar(){
    updateNotes();
    readNotes();
    makeCalendar(cDate);
}

void DeskCalendar::toDia(){
    QSqlQuery q(QString("select events.id, events.time_s, events.time_e, events.name, events.color from events "
                        "where events.date_s = \'%1\' order by events.time_s ").arg(cDate.toString("yyyy-MM-dd")));
    int line = 0; // кол-во полос
    while (q.next()){
        line++;
    }
    int height_canvas = 0; //
    if (line > 11){
        height_canvas = 455 + (35 * (line - 11));
    } else {
        height_canvas = 455;
    }
    QString t;
    t.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> "
             "<html><head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>"
             "<body>");
    t.append(QString("<h3 align=left>Events on %1</h3>").arg(cDate.toString("d MMM yyyy")));
    t.append(QString("<canvas id=\"canva\" width=\"630\" height=\"%1\" style=\"border: 2px solid\"></canvas>").arg(height_canvas));
    t.append("<script>"
             "function drawDia(){ "
             "var canvas = document.getElementById(\'canva\'); "
             "var coord = canvas.getContext(\'2d\'); "
             "coord.lineJoin = \'round\'; "
             "coord.strokeStyle = \'black\'; "
             "coord.beginPath(); "
             "coord.moveTo(15, 15); ");
    t.append(QString("coord.lineTo(15, %1);  coord.lineTo(615, %1); ").arg(height_canvas - 35));
    for (int x = 40; x < 616; x = x+25){
        t.append(QString("coord.moveTo(%1, %2); coord.lineTo(%1, %3); ").arg(x).arg(height_canvas - 38).arg(height_canvas - 32));
    }
    t.append("coord.stroke(); "
             "coord.save(); "
             "coord.font = \"11px arial\"; "
             "coord.fillStyle = \'black\'; ");
    int y = 1;
    for (int x = 38; x < 240; x = x+25){
       t.append(QString("coord.fillText(\'%1\', %2, %3, 5); ").arg(y).arg(x).arg(height_canvas - 20));
       y++;
    }
    for (int x=260; x < 615; x = x+25){
        t.append(QString("coord.fillText(\'%1\', %2, %3, 10); ").arg(y).arg(x).arg(height_canvas - 20));
        y++;
    }
    t.append(QString("coord.save(); "
             "coord.font = \"11px arial black\"; "
             "coord.fillStyle = \'black\'; "
             "coord.fillText(\'Events\', 15, 13, 300); "
             "coord.fillText(\'Timeline\', 560, %1, 300); ").arg(height_canvas - 5));

    int y1 = height_canvas - 50;
    int y2 = 0;
    QSqlQuery q2(QString("select events.id, events.time_s, events.time_e, events.name, events.color from events "
                        "where events.date_s = \'%1\' order by events.time_s ").arg(cDate.toString("yyyy-MM-dd")));

    while (q2.next()){
        QString name(QString("id%1").arg(q2.value(0).toString()));
        double x1 = 0;
        double x2 = 0;
        QString hourA(QString("%1.0").arg(q2.value(1).toTime().hour()));
        QString minA(QString("%1.0").arg(q2.value(1).toTime().minute()));
        x1 = 15.0 + ((hourA.toDouble() + minA.toDouble()/59.0) * 25.0);
        QString hourB(QString("%1.0").arg(q2.value(2).toTime().hour()));
        QString minB(QString("%1.0").arg(q2.value(2).toTime().minute()));
        x2 = 15.0 + ((hourB.toDouble() + minB.toDouble()/59.0) * 25.0);        
        y2 = y1 - 13;
        t.append(QString("var %1 = canvas.getContext(\'2d\'); "
                         "%1.lineWidth = 20; "
                         "%1.lineJoin = \'round\'; "
                         "%1.strokeStyle = \'%2\'; "
                         "%1.beginPath(); "
                         "%1.moveTo(%3, %5); "
                         "%1.lineTo(%4, %5); "
                         "%1.stroke(); "
                         "%1.save(); "
                         "%1.font = \"11px arial black\"; "
                         "%1.fillStyle = \'black\'; "
                         "%1.textAlign = \'left\'; "
                         "%1.fillText(\'%7\', 18, %6, 300); ")
                 .arg(name)
                 .arg(q2.value(4).toString())
                 .arg(x1)
                 .arg(x2)
                 .arg(y1)
                 .arg(y2)
                 .arg(q2.value(3).toString()));
        y1 = y1 - 35;
    }

    t.append(" } "
             "window.addEventListener(\"load\", drawDia, true); "
             "</script></body></html>");
    viewDia *vd = new viewDia(t, appPath, this);
    vd->show();
}

void DeskCalendar::loadIcons(){
    QIcon iPrev(QDir::toNativeSeparators(QString("%1/icons/toPrev.png").arg(appPath)));
    ui->toolButton_prev->setIcon(iPrev);
    QIcon iNext(QDir::toNativeSeparators(QString("%1/icons/toNext3.png").arg(appPath)));
    ui->toolButton_next->setIcon(iNext);
    QIcon iDia(QDir::toNativeSeparators(QString("%1/icons/stat.png").arg(appPath)));
    ui->toolButton_dia->setIcon(iDia);
    QIcon iAdd(QDir::toNativeSeparators(QString("%1/icons/add.png").arg(appPath)));
    ui->pushButton_add->setIcon(iAdd);
    QIcon iClose(QDir::toNativeSeparators(QString("%1/icons/close.png").arg(appPath)));
    ui->pushButton_close->setIcon(iClose);
    ui->pushButton_close_event->setIcon(iClose);
    QIcon iSave(QDir::toNativeSeparators(QString("%1/icons/done.png").arg(appPath)));
    ui->pushButton_save->setIcon(iSave);
    QIcon iWin(QDir::toNativeSeparators(QString("%1/icons/cale.png").arg(appPath)));
    setWindowIcon(iWin);
    QIcon iInfo(QDir::toNativeSeparators(QString("%1/icons/info.png").arg(appPath)));
    ui->toolButton_about->setIcon(iInfo);
}

void DeskCalendar::toInfo(){
    //QIcon iInfo(QDir::toNativeSeparators(QString("%1/icons/info.png").arg(appPath)));
    QMessageBox messa;
    messa.setIcon(QMessageBox::Information);
    messa.setText("<p align=center><b>DeskCalendar 0.2beta</b><br>april 2013<br>"
                  "Ev Shevchenko<br>Skyrim78@yandex.ru<br> "
                  "Icons from www.pixelmixer.com </p>");
    QPushButton *buttAbout = messa.addButton("About Qt", QMessageBox::ActionRole);
    QPushButton *buttClose = messa.addButton(QMessageBox::Close);
    messa.exec();
    if (messa.clickedButton() == buttClose){
           messa.close();
    } else if (messa.clickedButton() == buttAbout){
        QMessageBox messaA;
        messaA.aboutQt(this, "About Qt");
        messaA.show();
    }
}

void DeskCalendar::testDB(){
    QSqlQuery query_a("pragma MAIN.table_info (events)");
    QStringList eventsColumn;
    while (query_a.next()){
        eventsColumn << query_a.value(1).toString();
    }
    if (!eventsColumn.contains("rem_date")){
        QSqlQuery query_b("ALTER TABLE main.events ADD COLUMN 'rem_date' date");
        query_b.exec();
        QSqlQuery query_c("ALTER TABLE main.events ADD COLUMN 'rem_time' time");
        query_c.exec();
    }
}

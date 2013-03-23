#include "event.h"

Events::Events(QList<int> _l, int _i, QDate _d, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _l;
    item = _i;
    dat = _d;

    createEvent();

    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveEvent()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deleteEvent()));

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));

    connect(ui.checkBox_period, SIGNAL(clicked()), this, SLOT(setPeriodicly()));
    connect(ui.checkBox_full_day, SIGNAL(clicked(bool)), this, SLOT(setFullDay(bool)));
}

void Events::createEvent(){
    QSqlQuery query(QString("select events.name, events.date_s, events.time_s, events.time_e, events.note, events.color "
                             "from events "
                             "where events.id = %1").arg(list.at(item)));
    query.next();
    ui.lineEdit_name->setText(query.value(0).toString());
    ui.dateEdit_main_start->setDate(query.value(1).toDate());
    ui.timeEdit_main_start->setTime(query.value(2).toTime());
    ui.timeEdit_main_end->setTime(query.value(3).toTime());
    ui.textEdit_note->setPlainText(query.value(4).toString());
    ui.comboBox_color->setCurrentIndex(ui.comboBox_color->findText(query.value(5).toString()));
    //full day
    if (ui.timeEdit_main_start->time().toString("H:mm") == "0:00" and
            ui.timeEdit_main_end->time().toString("H:mm") == "23:59"){
        ui.checkBox_full_day->setChecked(true);
        ui.timeEdit_main_start->setVisible(false);
        ui.timeEdit_main_end->setVisible(false);
        ui.label_time->setVisible(false);
    }

    if (list.size() == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
        ui.pushButton_del->setEnabled(false);
    } else {
        ui.pushButton_toFirst->setEnabled(true);
        ui.pushButton_toNext->setEnabled(true);
        ui.pushButton_toLast->setEnabled(true);
        ui.pushButton_toPrev->setEnabled(true);
        ui.pushButton_del->setEnabled(true);
    }
    if (item == 0 and list.size() > 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
    } else if (item > 0 and list.size() > 0) {
        ui.pushButton_toFirst->setEnabled(true);
        ui.pushButton_toPrev->setEnabled(true);
    }
    if (item == list.size() -1 and list.size() > 0){
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
    } else if (item < list.size() -1 and list.size() > 0) {
        ui.pushButton_toNext->setEnabled(true);
        ui.pushButton_toLast->setEnabled(true);
    }
}

void Events::saveEvent(){
    QString err;
    if (!ui.lineEdit_name->text().isEmpty()){
        QSqlQuery query(QString("update events set name = \'%1\', date_s = \'%2\', time_s = \'%3\', time_e = \'%4\', "
                                "note = \'%5\', color = \'%6\' where events.id = \'%7\' ")
                        .arg(ui.lineEdit_name->text())
                        .arg(ui.dateEdit_main_start->date().toString("yyyy-MM-dd"))
                        .arg(ui.timeEdit_main_start->time().toString("hh:mm:ss"))
                        .arg(ui.timeEdit_main_end->time().toString("hh:mm:ss"))
                        .arg(ui.textEdit_note->toPlainText())
                        .arg(ui.comboBox_color->currentText())
                        .arg(list.at(item)));
        query.exec();
        err.append(query.lastError().text());
        if (err.size() == 1){
            ui.lineEdit_status->setText("Сохранено...");
        } else {
            ui.lineEdit_status->setText(err);
        }
    } else {
        ui.lineEdit_status->setText("Нет заголовка!!!");
    }
}

void Events::deleteEvent(){
    QMessageBox messa;
    messa.setText("Удалить?");
    QPushButton *yes = messa.addButton(QMessageBox::Yes);
    QPushButton *no = messa.addButton(QMessageBox::No);
    messa.exec();
    if (messa.clickedButton() == yes){
        QSqlQuery del(QString("delete from events where events.id = \'%1\' ").arg(list.at(item)));
        del.exec();
        close();
    } else if (messa.clickedButton() == no){
        messa.close();
    }
}

void Events::toFirst(){
    item = 0;
    createEvent();
}

void Events::toLast(){
    item = list.size() - 1;
    createEvent();
}

void Events::toNext(){
    item = item + 1;
    createEvent();
}

void Events::toPrev(){
    item = item - 1;
    createEvent();
}

void Events::setFullDay(bool x){
    if (x == true){
        ui.timeEdit_main_start->setTime(QTime::fromString("00:00:00", "hh:mm:ss"));
        ui.timeEdit_main_start->hide();
        ui.timeEdit_main_end->setTime(QTime::fromString("23:59:00", "hh:mm:ss"));
        ui.timeEdit_main_end->hide();
        ui.label_time->hide();
    } else if (x == false){
        ui.timeEdit_main_start->setVisible(true);
        ui.timeEdit_main_end->setVisible(true);
        ui.label_time->setVisible(true);
    }
}
void Events::setPeriodicly(){

}

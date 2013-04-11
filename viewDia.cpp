#include "viewDia.h"

viewDia::viewDia(QString _text, QString path, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    text = _text;
    appPath = path;

    loadIcons();
    load();
    connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
}

void viewDia::load(){
    ui.webView->setHtml(text);
}

void viewDia::print(){
    QPrinter *printer = new QPrinter(QPrinterInfo::defaultPrinter());
    QPrintDialog pDialog(printer, this);
    if (pDialog.exec() == QDialog::Accepted){
        ui.webView->print(printer);
    }
}

void viewDia::loadIcons(){
    QIcon iPrint(QDir::toNativeSeparators(QString("%1/icons/print.png").arg(appPath)));
    ui.actionPrint->setIcon(iPrint);
    QIcon iClose(QDir::toNativeSeparators(QString("%1/icons/close.png").arg(appPath)));
    ui.actionClose->setIcon(iClose);
    QIcon iWin(QDir::toNativeSeparators(QString("%1/icons/stat.png").arg(appPath)));
    setWindowIcon(iWin);
}

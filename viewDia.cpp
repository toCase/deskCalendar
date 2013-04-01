#include "viewDia.h"

viewDia::viewDia(QString _text, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    text = _text;
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

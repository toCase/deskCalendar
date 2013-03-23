#include "viewDia.h"

viewDia::viewDia(QString _text, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    text = _text;
    load();
    connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
}

void viewDia::load(){
    ui.webView->setHtml(text);
}

void viewDia::print(){

}

#include "mainwindow.hh"
#include "beepgenerator.hh"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString phrasefile="phrasebook";
    for(int i=1;i<argc;i++){
        QString str(argv[i]);
        if(str.startsWith("--phrasebook=")){
            phrasefile=str.mid(13);
            qDebug()<<phrasefile;
        }
    }
    MainWindow w(phrasefile);
    w.show();
    return a.exec();

}

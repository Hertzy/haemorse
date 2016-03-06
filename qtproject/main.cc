#include "mainwindow.hh"
#include "beepgenerator.hh"

#include "codestatistics.h"

#include <QApplication>
#include <QDebug>

#include <QFile>
#include <QByteArray>


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

    QFile florem(":/plaintext/loremipsum");
    florem.open(QIODevice::ReadOnly);
    QByteArray blorem=florem.readAll();
    QString lorem=QString::fromLatin1(blorem);

    CodeStatistics loremStats(lorem);
    qDebug()<<"Lorem ipsum:"<<loremStats;

    QFile fwiki(":/plaintext/wikipedia");
    fwiki.open(QIODevice::ReadOnly);
    QByteArray bwiki=fwiki.readAll();
    QString wiki=QString::fromLatin1(bwiki);

    CodeStatistics wikiStats(wiki);
    qDebug()<<"Wiki articre stats"<<wikiStats;

    MainWindow w(phrasefile);
    w.show();
    return a.exec();

}

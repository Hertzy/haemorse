#include "mainwindow.hh"
#include "beepgenerator.hh"

#include "pulsedecoder.h"
#include "pulsedecodertester.h"
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

    /*
     * Commenting out the decoder testing
    QFile fwiki(":/plaintext/wikipedia");
    fwiki.open(QIODevice::ReadOnly);
    QByteArray bwiki=fwiki.readAll();
    QString wiki=QString::fromLatin1(bwiki);

    CodeStatistics wikiStats(wiki);
    qDebug()<<"Wiki articre stats"<<wikiStats;

    PulseDecoder decoder(&wikiStats);

    PulseDecoderTester tester(&decoder);
    tester.test();*/

    MainWindow w(phrasefile);
    w.show();
    return a.exec();

}

#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QDebug>
#include<QStringList>
#include <QThread>
#include <QFile>
#include <QIODevice>

QStringList readPhraseBookFromDevice(QIODevice* dev){
    QStringList ret;
    if(!dev->isOpen()){
        qDebug()<<"Could not read file";
        return ret;
    }
    while(dev->bytesAvailable()>0){
        QByteArray line=dev->readLine();
        if(!line.isEmpty()){
            ret.append(QString::fromUtf8(line));
        }
    }
    return ret;
}

MainWindow::MainWindow(QString phrasefilename, QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow),rowc(0),charc(0),spacedown(false)
{
    cur_="";
    curidx=0;

    beeper=new Beeper(this);
    beeper->setPadding(4);

    installEventFilter(this);

    QFile* book=new QFile(phrasefilename);
    if(book->exists()){
        qDebug()<<"Phrasebook found";
        book->open(QIODevice::ReadOnly|QIODevice::Text);
        beeper->setPhrasebook(readPhraseBookFromDevice(book));
    }
    delete book;


    beeper->setMode(Beeper::GENERATE_MESSAGE|Beeper::CONTINUOUS);

    ui->setupUi(this);


    connect(beeper,SIGNAL(aboutToSend(QString)),this,SLOT(sendingMessage(QString)));
    connect(this,SIGNAL(readyToReceive()),beeper,SLOT(next()));
    connect(ui->spinBox_padding,SIGNAL(valueChanged(int)),beeper,SLOT(setPadding(int)));
    connect(ui->pushButton,SIGNAL(clicked(bool)),beeper,SLOT(setPaused(bool)));
    connect(ui->spinBox_unit,SIGNAL(valueChanged(int)),beeper,SLOT(setUnitlength(int)));
    ui->spinBox_padding->setValue(beeper->padding());
    ui->spinBox_unit->setValue(beeper->unitlength());
    ui->checkBox->setChecked(true);

    beeper->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete beeper;
}

bool MainWindow::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::ShortcutOverride &&((QKeyEvent*)e)->key()==Qt::Key_Space) {
        if(spacedown){
            spacedown=false;
        }else{
            spacedown=true;
            keyPressEvent(((QKeyEvent*)e));
        }
        return true;
    }
    return false;
}

void MainWindow::show()
{
    QMainWindow::show();
    beeper->start();
}

void MainWindow::sendingMessage(QString msg)
{
    if(!cur_.isEmpty()&&curidx<cur_.length()){
        QString missed="<font color=\"red\">";
        for(;curidx<cur_.length();curidx++){
            missed+=cur_.at(curidx);
        }
        missed+="</font>";
        insertHtml(missed);
    }
    rowc++;
    newPara();
    cur_=msg.toUpper();
    curidx=0;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key()<Qt::Key_Space||e->key()>Qt::Key_ydiaeresis){
        return;
    }
    if(!cur_.isEmpty()){

        QString toAdd=e->text().toUpper();
        if(e->key()==Qt::Key_Space){
            toAdd=" ";
        }
        if(curidx<cur_.length()){

            if(toAdd!=cur_.mid(curidx,1)){
                toAdd="<font color=\"red\">"+cur_.mid(curidx,1)+"</font>";
                ui->statusBar->showMessage(encode_morse(cur_.mid(curidx,1))+" is "+cur_.mid(curidx,1),1500);
            }
        }else{
            toAdd="<font color='red'>_</font>";
        }
        insertHtml(toAdd);
        curidx++;
    }
    if(curidx==cur_.length()){
        curidx++;
        emit readyToReceive();
    }
}

void MainWindow::focusInEvent(QFocusEvent *e)
{
    if(!beeper->isRunning()){
        beeper->start();
    }else{
        beeper->unpause();
    }
}

void MainWindow::focusOutEvent(QFocusEvent *e)
{
    beeper->pause();
}

void MainWindow::insertHtml(QString toAdd)
{
    QString html=ui->textBrowser->toHtml();
    if(rowc>3){
        int first=html.indexOf("<p");
        int last=html.indexOf("</p>")+4;
        html.remove(first,last-first);
        rowc--;
    }
    if(html.contains("<br />")){
        html="<p>"+toAdd+"</p>";
    }else if(html.contains(QRegExp("<p [^>]*> </p>"))){
        html.replace("> </p>",">"+toAdd+"</p>");
    }else{
        int pidx=html.lastIndexOf("</p>");
        html.insert(pidx,toAdd);
    }

    ui->textBrowser->setHtml(html);
}

void MainWindow::newPara()
{
    QString html=ui->textBrowser->toHtml();
    int pidx=html.lastIndexOf("</p>")+4;
    html.insert(pidx,"<p>&nbsp;</p>");

    ui->textBrowser->setHtml(html);
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked!=(beeper->mode()&Beeper::CONTINUOUS)){
        beeper->setMode(beeper->mode()^Beeper::CONTINUOUS);
    }
}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    if(checked){
        beeper->setMode(beeper->mode()|Beeper::PHRASEBOOK);
    }else{
        beeper->setMode(beeper->mode()& ~Beeper::PHRASEBOOK);
    }
}

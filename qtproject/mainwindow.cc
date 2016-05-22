#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QDebug>
#include<QStringList>
#include <QThread>
#include <QFile>
#include <QIODevice>

/**
 * @brief readPhraseBookFromDevice reads text from the given device and returns a list
 * of the lines
 * @param dev
 * @return
 */
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
    connect(ui->pushButton,SIGNAL(toggled(bool)),beeper,SLOT(setPaused(bool)));
    connect(ui->spinBox_unit,SIGNAL(valueChanged(int)),beeper,SLOT(setUnitlength(int)));
    connect(ui->rangeStart,SIGNAL(editingFinished()),this,SLOT(rangeChanged()));
    connect(ui->rangeEnd,SIGNAL(editingFinished()),this,SLOT(rangeChanged()));
    ui->spinBox_padding->setValue(beeper->padding());
    ui->spinBox_unit->setValue(beeper->unitlength());
    //ui->checkBox_cont->setChecked(true);
    //ui->checkBox_phrases->setChecked(false);

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
    static bool streak[]={false,false,false,false,false,
                          false,false,false,false,false,
                         false,false,false,false,false,
                         false,false,false,false,false,
                         false,false,false,false,false};
    static const int streakCt=25;
    static int streakidx=0;
    //Check that the key typed was a character or letter
    if(e->key()<=Qt::Key_Space||e->key()>=Qt::Key_ydiaeresis){
        ui->pushButton->toggle();
        return;
    }
    if(!cur_.isEmpty()){
        bool correct=true;
        //Compare to current message being played
        QString toAdd=e->text().toUpper();
        if(e->key()==Qt::Key_Space){
            toAdd=" ";
        }
        if(curidx<cur_.length()){
            //mark wrong answers as the correct letter in red
            if(toAdd!=cur_.mid(curidx,1)){
                correct=false;
                toAdd="<font color=\"red\">"+cur_.mid(curidx,1)+"</font>";
                ui->statusBar->showMessage(encode_morse(cur_.mid(curidx,1))+" is "+cur_.mid(curidx,1),1500);
            }
            streak[streakidx]=correct;
            streakidx=(streakidx+1)%streakCt;
            int correctCount=0;
            for(int i=0;i<streakCt;i++){
                correctCount+=streak[i];
            }
            if(correctCount>0.9*streakCt){
                handleStreak();
                for(int i=0;i<streakCt;i++){
                    streak[i]=false;
                }
            }
        }else{
            //Denote excess characters with underscores
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

    while(rowc>ui->textBrowser->geometry().height()/(36+2)){
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

void MainWindow::handleStreak()
{
    //If no advance is set, return.
    if(ui->radio_none->isChecked())return;
    if(ui->radio_Padding->isChecked()){
        if(ui->spinBox_padding->value()>0){
            ui->spinBox_padding->setValue(ui->spinBox_padding->value()-1);
        }
    }
    if(ui->radio_wpm->isChecked()){
        double wpm=12e2/double(ui->spinBox_unit->value());
        wpm+=1;
        int newUnit=int(12e2/wpm+0.5);
        newUnit=std::max(ui->spinBox_unit->minimum(),newUnit);
        ui->spinBox_unit->setValue(newUnit);
    }
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

void MainWindow::rangeChanged()
{
    QChar startchar=ui->rangeStart->text().trimmed().at(0);
    QChar endchar=ui->rangeEnd->text().trimmed().at(0);
    int startIdx=0;
    int endIdx=CODE_COUNT;
    for(int i=0;i<CODE_COUNT;i++){
        if(startchar==MORSE_CODES[i].first){
            startIdx=i;
        }
        if(endchar==MORSE_CODES[i].first){
            endIdx=i+1;
        }
    }
    QPair<int,int> range(startIdx,endIdx);
    if(endIdx<startIdx){
        range.first=endIdx;
        range.second=startIdx;
    }
    beeper->setRange(range);
}

void MainWindow::on_rangeStart_textChanged(const QString &arg1)
{
    QString trimmed=arg1.trimmed();
    if(trimmed.length()>1){
        QChar character=trimmed.at(trimmed.length()-1);
        ui->rangeStart->setText(character);
    }
}

void MainWindow::on_rangeEnd_textChanged(const QString &arg1)
{
    QString trimmed=arg1.trimmed();
    if(trimmed.length()>1){
        QChar character=trimmed.at(trimmed.length()-1);
        ui->rangeStart->setText(character);
    }
}

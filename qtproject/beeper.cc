#include "beeper.hh"
#include "encoder.h"
#include <QAudio>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QList>
#include <QDebug>

Beeper::Beeper(QObject *parent) :
    QThread(parent), unitlength_(75),mode_(CONTINUOUS),gen_(new BeepGenerator(400.0,0.0,this)),
    runsema(0),padding_(0),paused(false)
{
    QAudioFormat format;
    format.setByteOrder(QAudioFormat::BigEndian);
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setSampleRate(44100);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo dev=QAudioDeviceInfo::defaultOutputDevice();
    if(!dev.isFormatSupported(format)){
        format=dev.nearestFormat(format);
        qDebug()<<format;
    }
    gen_->setFormat(format);
    msgqueue_.clear();
}

Beeper::~Beeper()
{
    mode_=0;
    msgqueue_.clear();
    unitlength_=0;
    padding_=0;
    mutex_.lock();
    this->terminate();
    mutex_.unlock();
    delete gen_;
}

void Beeper::sendMorse(QString morse)
{
    char c;
    int spcount=0;
    for(int i=0;i<morse.length();i++){
        c=morse[i].toLatin1();
        switch(c){
        case '.':
            spcount=0;
            gen_->beep(unitlength_);
            gen_->silence(unitlength());
            break;
        case '-':
            spcount=0;
            gen_->beep(unitlength_*3);
            gen_->silence(unitlength());
            break;
        case ' ':
            while(paused);
            spcount++;
            if(spcount==1){
                QChar sent(decode_single(morse.left(i).split(" ").last()));
                emit charSent(sent);
                if(padding_>0){
                    gen_->silence(unitlength()*9*padding_);
                }
                gen_->silence(unitlength_*3);
            }else{
                gen_->silence(unitlength_*2);
            }
            break;
        }
    }
    QThread::msleep(unitlength_*7);

    if((mode_&CONTINUOUS)==CONTINUOUS) next();
}
QStringList Beeper::getPhrasebook() const
{
    return phrasebook;
}

void Beeper::setPhrasebook(const QStringList &value)
{
    phrasebook = value;
}

int Beeper::padding() const
{
    return padding_;
}

void Beeper::setPadding(int padding)
{
    padding_ = padding;
}


void Beeper::addPlain(QString plain)
{
    QString morse=encode_morse(plain);
    addMorse(morse);
}

void Beeper::addMorse(QString morse)
{
    mutex_.lock();
    msgqueue_.enqueue(morse);
    mutex_.unlock();
}
qint64 Beeper::unitlength() const
{
    return unitlength_;
}

void Beeper::setUnitlength(const int &unitlength)
{
    unitlength_ = unitlength;
}

void Beeper::next()
{
    runsema.release();
}

void Beeper::pause()
{
    qDebug()<<"Paused";
    paused=true;
}

void Beeper::setPaused(bool paused)
{
    if(paused){
        pause();
    }else{
        unpause();
    }
}

void Beeper::unpause()
{
    qDebug()<<"Resumed";
    paused=false;
}

void Beeper::run()
{
    for(;;){
        runsema.acquire();
        while(paused);
        if((mode_&PHRASEBOOK)==PHRASEBOOK){
            if(phrasebook.isEmpty()){
                mode_=(mode_|GENERATE_MESSAGE) & ~PHRASEBOOK;
            }else{
                //Select a random phrase and send it
                int r=rand()%phrasebook.length();
                mutex_.lock();
                msgqueue_.enqueue(encode_morse(phrasebook.at(r)));
                mutex_.unlock();
            }
        }
        if((mode_&GENERATE_MESSAGE)==GENERATE_MESSAGE && msgqueue_.isEmpty()){
            QString morse;
            int chars=rand()%3+2;
            for(int i=0;i<chars;i++){
                QPair<char,QString> p =MORSE_CODES[rand()%CODE_COUNT];
                morse+=p.second+" ";
            }

            mutex_.lock();
            msgqueue_.enqueue(morse);
            mutex_.unlock();
        }
        if(!msgqueue_.isEmpty()){
            mutex_.lock();
            QString morse=msgqueue_.dequeue();
            mutex_.unlock();
            emit aboutToSend(decode_morse(morse));
            sendMorse(morse);
        }
    }
}

int Beeper::mode() const
{
    return mode_;
}

void Beeper::setMode(const int &mode)
{
    mode_ = mode;
    mutex_.lock();
    msgqueue_.clear();
    if((mode&GENERATE_MESSAGE)==GENERATE_MESSAGE){
        if(phrasebook.isEmpty()){
            QPair<char,QString> p =MORSE_CODES[rand()%CODE_COUNT];
            msgqueue_.enqueue(p.second+" ");
        }else{
            int r=rand()%phrasebook.length();
            msgqueue_.enqueue(encode_morse(phrasebook.at(r)));
        }
    }
    mutex_.unlock();
}

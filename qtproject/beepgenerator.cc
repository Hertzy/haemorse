#include "beepgenerator.hh"
#include <math.h>
#include <cstdlib>

#include <QThread>

#include <QDebug>

#define PI 3.141592

inline void writeToDevice(QIODevice* odev_, char* toPlay,int samples){
    int toWrite=samples;
    int written=0;
    int d=0;
    while(toWrite>0){
        odev_->waitForBytesWritten(1);
        d=odev_->write(toPlay+written,toWrite);
        written+=d;
        toWrite-=d;
    }
}

BeepGenerator::BeepGenerator(double f, double snr, QObject *parent):
    QObject(parent),f_(f),snr_(snr),odev_(0),odevMutex_(QMutex::NonRecursive)
{
    dev_=QAudioDeviceInfo::defaultOutputDevice();
    format_.setByteOrder(QAudioFormat::BigEndian);
    format_.setSampleRate(44100);
    format_.setSampleSize(8);
    format_.setChannelCount(1);
    if(!dev_.isFormatSupported(format_)){
        format_=dev_.nearestFormat(format_);
    }
    out_=new QAudioOutput(dev_,format_);
    generateBuffer();
}

BeepGenerator::~BeepGenerator()
{
    odevMutex_.lock();
    if(odev_!=0) delete odev_;
    odevMutex_.unlock();
}


void BeepGenerator::beep(qint64 millis)
{
    if(odev_==0) return;
    int bytes=int(format_.sampleSize()/8);
    int samples=(int)(millis*bytes*format_.sampleRate()/1000);
    while(samples%bytes!=0){
        samples++;
    }

    int len=precalc_.length();
    int offs=rand()%len;
    offs=offs-(offs%bytes);

    char* toPlay=new char[samples];
    for(int i=0;i<samples;i++){
        toPlay[i]=(precalc_.at((i+offs)%len));
    }

    odevMutex_.lock();
    writeToDevice(odev_,toPlay,samples);
    odevMutex_.unlock();
    delete[] toPlay;
}

void BeepGenerator::generateBuffer()
{
    precalc_=QByteArray();
    int bytes=(format_.sampleSize()/8);
    int samples=(int)(format_.sampleRate()*bytes);

    double fi=f_*2*PI/format_.sampleRate();//frequency in radians/sample

    qint64 range=(qint64)(pow(2,format_.sampleSize()-1)*0.6);//range to either direction

    if(format_.sampleType()==QAudioFormat::UnSignedInt){
        range/=2;
    }

    double noise=0;
    if(snr_>0){
        noise=(1/snr_);
    }
    double y=0;
    qint64 out=0;
    for(int i=0;i<samples;i++){
        //calculate signal level
        y=cos(i*fi);

        if(snr_>0){
            //add noise
            y+=noise*rand()/RAND_MAX;
            y/=1+noise;
        }
        //scale signal level to sample size
        out=(qint64)(range*y);

        if(format_.sampleType()==QAudioFormat::UnSignedInt){
            out+=range;
        }

        //Enqueue sample
        char outc=out&0xff;
        switch(format_.byteOrder()){
        case QAudioFormat::BigEndian:
            for(int k=format_.sampleSize()-8;k>=0;k-=8){
                outc=(out&(0xff<<k))>>k;
                precalc_.append(outc);
            }
            break;
        case QAudioFormat::LittleEndian:
            for(int k=0;k<format_.sampleSize();k+=8){
                outc=(out& (0xff<<k))>>k;
                precalc_.append(outc);
            }
            break;
        }
    }
}

void BeepGenerator::silence(qint64 millis)
{
    int bytes=int(format_.sampleSize()/8);
    int samples=(int)(millis*format_.sampleRate()*bytes/1000);

    while(samples%bytes!=0){
        samples++;
    }

    qint64 range=(qint64)(pow(2,format_.sampleSize()-2));//range to either direction
    qint64 noise=0;
    if(snr_>0){
        noise=(range/snr_);
    }
    qint64 out=0;
    char* toPlay=new char[samples];
    for(int i=0;i<samples;i+=(int)(format_.sampleSize()/8)){
        out=0;
        if(noise>0){
            out=(rand()%noise);
        }
        char outc;
            for(int k=0;k<format_.sampleSize();k+=8){
                outc=(out& (0xff<<k))>>k;
                switch(format_.byteOrder()){
                case QAudioFormat::BigEndian:
                    toPlay[int(i+(format_.sampleSize()-k)/8-1)]=outc;
                    break;
                case QAudioFormat::LittleEndian:
                    toPlay[int(i+k/8)]=outc;
                    break;
                }
            }
    }
    odevMutex_.lock();
    writeToDevice(odev_,toPlay,samples);
    odevMutex_.unlock();
    delete[] toPlay;
}
QAudioFormat BeepGenerator::format() const
{
    return format_;
}

void BeepGenerator::setFormat(const QAudioFormat &format)
{
    format_ = format;
    if(!dev_.isFormatSupported(format_)){
        format_=dev_.nearestFormat(format_);
    }
    generateBuffer();
    delete out_;
    odevMutex_.lock();
    if(odev_!=0){
        odev_->deleteLater();
    }
    out_=new QAudioOutput(dev_,format_);
    odev_=out_->start();
    odevMutex_.unlock();
}


double BeepGenerator::frequency() const
{
    return f_;
}

void BeepGenerator::setFrequency(double f)
{
    f_ = f;
}





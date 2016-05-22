#include "pulsedecodertester.h"

#include <QDebug>

#include <chrono>
#include "encoder.h"

PulseDecoderTester::PulseDecoderTester(PulseDecoder* target):target_(target)
{

}

void PulseDecoderTester::test()
{
    static const long unitlength=100;
    static const long vars[]={0,10,15,20,25};
    static const long varCount=5;

    for(int i=0;i<varCount;i++){
        target_->clear();
        helloWorldTest(unitlength,vars[i]);
        liveSendTest(unitlength,vars[i]);
    }
}

void PulseDecoderTester::helloWorldTest(long unitLength, long var)
{
    std::normal_distribution<double> dist=std::normal_distribution<double>(double(unitLength),double(var));
    QString expected_plain="HELLO WORLD! HAVE SOME MORSE CODE.";
    static const QString morse=encode_morse(expected_plain,"\t"," ");
    static const QString expected=encode_morse(expected_plain,"   "," ");

    PulseString testInput=morseToPulse(morse,dist);

    QString output=target_->decodePulseString(testInput);
    qDebug()<<"Hello World test"<<"\nunit length"<<unitLength<<"var"<<var
           <<"\nOutput  "<<output<<decode_morse(output)<<" unit length"<<target_->unitlength()
          << "\nExpected"<<expected<<decode_morse(expected);
}

void PulseDecoderTester::liveSendTest(long unitLength, long var)
{
    std::normal_distribution<double> dist=std::normal_distribution<double>(double(unitLength),double(var));

    qDebug()<<"Live send test"<<"\nunit length"<<unitLength<<"var"<<var;
    target_->clear();

    QString expected_plain="";
    for(int j=0;j<300;j++){
        for(int k=0;k<5;k++){
            expected_plain+=char('A'+rand()%26);
        }
        expected_plain+=" ";
    }
    QString morse=encode_morse(expected_plain,"\t"," ");

    PulseString testInput=morseToPulse(morse,dist);
    target_->decodePulseString(testInput,true);

    for(int i=0;i<41;i++){
        expected_plain="";
        for(int j=0;j<3;j++){
            for(int k=0;k<3;k++){
                expected_plain+=char('A'+rand()%26);
            }
            expected_plain+=" ";
        }

        QString morse=encode_morse(expected_plain,"\t"," ");
        QString expected=encode_morse(expected_plain,"   "," ");

        PulseString testInput=morseToPulse(morse,dist);

        QString output=target_->decodePulseString(testInput,true);
        if(i%20==0){
            qDebug()<<"Iteration "<<(i+1)
                   <<"\nOutput  "<<output<<decode_morse(output)<<" unit length"<<target_->unitlength()
                  << "\nExpected"<<expected<<decode_morse(expected);
        }
    }
}

PulseString PulseDecoderTester::morseToPulse(QString morse, std::normal_distribution<double> dist)
{
    PulseString testInput;
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine generator (seed);

    double dLength;
    for(int i=0;i<morse.length();i++){
        char c=morse.at(i).toLatin1();
        switch(c){
        case '.':
            dLength=dist(generator);
            testInput.append(Pulse(true,long(dLength)));
            dLength=dist(generator);
            testInput.append(Pulse(false,long(dLength)));
            break;
        case '-':
            dLength=dist(generator)+dist(generator)+dist(generator);
            testInput.append(Pulse(true,long(dLength)));
            dLength=dist(generator);
            testInput.append(Pulse(false,long(dLength)));
            break;
        case ' ':
            dLength=dist(generator)+dist(generator)+dist(generator);
            if(!testInput.last().first) testInput.last().second=dLength;
            else testInput.append(Pulse(false,long(dLength)));
            break;
        case '\t':
            dLength=dist(generator)+dist(generator)+
                    dist(generator)+dist(generator)+
                    dist(generator)+dist(generator)+
                    dist(generator);
            if(!testInput.last().first) testInput.last().second=dLength;
            else testInput.append(Pulse(false,long(dLength)));
            break;
        }
    }
    return testInput;
}

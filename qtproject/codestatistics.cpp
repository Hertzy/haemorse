#include "codestatistics.h"
#include "encoder.h"

/**
 * @brief MEAN_WEIGHT is the weight given to the old mean and standard deviation when approximating new ones.
 */
const double MEAN_WEIGHT=0.9;


CodeStatistics::CodeStatistics(const QString& initPlain):codestring_(encode_morse(initPlain)),totalDahs_(0),totalDits_(0),
    interElementSpaces_(0),interLetterSpaces_(0),interWordSpaces_(0),meanHigh_(2),varHigh_(1),meanLow_(2),varLow_(3)
{
    updateStatsFromCode(codestring_);
}

void CodeStatistics::updatePlain(const QString &plaintext)
{
    QString code=encode_morse(plaintext);
    updateCode(code);
}

void CodeStatistics::updateCode(const QString &morse)
{
    updateStatsFromCode(morse);
    codestring_+=morse;
}

int CodeStatistics::totalDits() const
{
    return totalDits_;
}

int CodeStatistics::totalDahs() const
{
    return totalDahs_;
}

int CodeStatistics::interElementSpaces() const
{
    return interElementSpaces_;
}

int CodeStatistics::interLetterSpaces() const
{
    return interLetterSpaces_;
}

int CodeStatistics::interWordSpaces() const
{
    return interWordSpaces_;
}

double CodeStatistics::ditRatio() const
{
    double total=totalDits()+totalDahs();
    if(total!=0){
        return totalDits()/total;
    }else{
        return 0.6;
    }
}

double CodeStatistics::interElementRatio() const
{
    int total=interElementSpaces()+interWordSpaces()+interLetterSpaces();
    double ret=double(interElementSpaces())/total;
    return ret;
}

void CodeStatistics::updateStatsFromCode(const QString &code)
{
    ElementFrequencyTree& freqTreeNode=freqTree_;
    for(int i=0;i<code.length();i++){
        char c=code[i].toLatin1();

        bool element=false;
        switch(c){
        case '.':
            totalDits_++;
            addHigh(1);
            element=true;
            freqTreeNode.addDit();
            freqTreeNode=freqTreeNode.ditBranch();
            break;
        case '-':
            totalDahs_++;
            addHigh(3);
            element=true;
            freqTreeNode.addDah();
            freqTreeNode=freqTreeNode.dahBranch();
            break;
        case ' ':
            freqTreeNode.addTermination();
            freqTreeNode=freqTree_;
            element=false;
            break;
        }
        char next=0;
        if(i<code.length()-1){
            next=code[i+1].toLatin1();
        }
        if(element){
            if(next=='.'||next=='-'){
                interElementSpaces_++;
                addLow(1);
            }
        }else{
            if(next!=' '){
                interLetterSpaces_++;
                addLow(3);
            }else{
                //Seek to the last space.
                i++;
                while(i<code.length()&&next==' '){
                    if(i<code.length()-1){
                        next=code[i+1].toLatin1();
                    }else{
                        next=0;
                    }
                    i++;
                }
                interWordSpaces_++;
                addLow(7);
            }
        }
    }
}

void CodeStatistics::addHigh(double d)
{
    meanHigh_=meanHigh_*MEAN_WEIGHT+d*(1.0-MEAN_WEIGHT);
    double diff=(meanHigh_-d);
    varHigh_=varHigh_*MEAN_WEIGHT+diff*diff*(1.0-MEAN_WEIGHT);
}

void CodeStatistics::addLow(double d)
{
    meanLow_=meanLow_*MEAN_WEIGHT+d*(1.0-MEAN_WEIGHT);
    double diff=meanLow_-d;
    varLow_=varLow_*MEAN_WEIGHT+diff*diff*(1.0-MEAN_WEIGHT);
}

ElementFrequencyTree& CodeStatistics::freqTree()
{
    return freqTree_;
}

double CodeStatistics::meanLow() const
{
    return meanLow_;
}

double CodeStatistics::varLow() const
{
    return varLow_;
}

double CodeStatistics::varHigh() const
{
    return varHigh_;
}

double CodeStatistics::meanHigh() const
{
    return meanHigh_;
}

QDebug &operator <<(QDebug &dbg, const CodeStatistics &stats)
{
    dbg<<"Code stats: ditratio"<<QString::number(stats.ditRatio()*100.0,'f')+"%"
      <<"interelementratio"<<QString::number(stats.interElementRatio()*100.0,'f')+"%"
      <<"\n mean high"
      <<QString::number(stats.meanHigh())<<"var"<<QString::number(stats.varHigh())
     <<"\n mean low"<<QString::number(stats.meanLow())<<"var"<<QString::number(stats.varLow());
}

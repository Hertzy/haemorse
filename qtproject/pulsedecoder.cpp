#include "pulsedecoder.h"

#include "algorithm"

#include <cmath>

/**
 * @brief pdf returns the normal distribution PDF for x
 * @param x
 * @return
 */
double pdf(double x){
    static const double inv_sqrt_2pi = 0.3989422804014327;
    double exponent=-x*x/2;
    return inv_sqrt_2pi*exp(exponent);
}

/**
 * @brief cdf returns the normal distribution CDF for x
 * @param x
 * @return
 */
double cdf(double x)
{
    //from Zelen & Severo: Handbook of mathematical functions with formulas, graphs and mathematical tables
    static const double p=0.2316419;
    static const double b1 = 0.319381530;
    static const double b2 = -0.356563782;
    static const double b3 = 1.781477937;
    static const double b4 = -1.821255978;
    static const double b5 = 1.330274429;

    double phi=pdf(x);

    double t=1/(1+p*x);
    double t2= t*t;
    double t3= t2*t;
    double t4= t2*t2;
    double t5= t2*t3;

    return 1-phi*(b1*t+b2*t2+b3*t3+b4*t4+b5*t5);

}

PulseDecoder::PulseDecoder(CodeStatistics *basestats):unitlength_(-1),stats_(basestats),freqTreeNode_(basestats->freqTree()),
    memorySize(1000)
{

}

PulseDecoder::~PulseDecoder()
{
    delete stats_;
}

void PulseDecoder::guessFromCodePulseString(PulseString pstr)
{
    meanHigh_=calculateMeanHigh(pstr);
    varHigh_=calculateVarHigh(pstr,meanHigh_);

    double guessFromMeanHigh=meanHigh_/stats_->meanHigh();
    double guessFromVarHigh=varHigh_/stats_->varHigh();

    meanLow_=calculateMeanLow(pstr);
    varLow_=calculateVarLow(pstr,meanLow_);

    double guessFromMeanLow=meanLow_/stats_->meanLow();
    double guessFromVarLow=varLow_/stats_->varLow();

    //Estimate the dit mean and variance by taking the
    //bottom ditratio-section of high pulses.
    //Classify the rest as Dahs.
    PulseString sortedHigh;
    PulseString sortedLow;
    PulseString dits;
    PulseString dahs;

    for(Pulse p:pstr){
        if(p.first)sortedHigh.append(p);
        else sortedLow.append(p);
    }

    std::sort(sortedHigh.begin(),sortedHigh.end(),[=](Pulse l,Pulse r)->bool{
        return l.second<r.second;
    });

    int ditIdx;
    for(ditIdx=0;ditIdx<sortedHigh.length()*stats_->ditRatio();ditIdx++){
        dits.append(sortedHigh[ditIdx]);
    }

    for(int i=ditIdx;i<sortedHigh.length();i++){
        dahs.append(sortedHigh[i]);
    }

    int count=dits.length();
    if(count!=0){
        ditMean_=0;
        for(int i=0;i<count;i++){
            ditMean_+=double(dits[i].second)/count;
        }

        ditVar_=0;
        for(int i=0;i<count;i++){
            double diff=dits[i].second-ditMean_;
            ditVar_+=diff*diff/count;
        }
    }else{
        ditMean_=meanHigh_-varHigh_;
        ditVar_=varHigh_;
    }

    count=dahs.length();
    if(count!=0){
        dahMean_=0;
        for(int i=0;i<count;i++){
            dahMean_+=double(dahs[i].second)/count;
        }

        dahVar_=0;
        for(int i=0;i<count;i++){
            double diff=dahs[i].second-dahMean_;
            dahVar_+=diff*diff/count;
        }
    }else{
        dahMean_=3*ditMean_;
        dahVar_=varHigh_;
    }

    if(ditVar_==0){
        ditVar_=varHigh_/3;
    }

    if(dahVar_==0){
        dahVar_=varHigh_/3;
    }

    unitlength_=long(ditMean_+0.5);
}

double PulseDecoder::calculateMeanHigh(PulseString pstr)
{
    double ret=0;
    int count;
    for(int i=0;i<pstr.length();i++){
        if(pstr[i].first){
            ret+=double(pstr[i].second);
            count++;
        }
    }
    if(count==0)return 0;
    return ret/count;
}

double PulseDecoder::calculateVarHigh(PulseString pstr, double mean)
{
    double ret=0;
    int count=0;
    for(int i=0;i<pstr.length();i++){
        if(pstr[i].first){
            count++;
        }
    }
    for(int i=0;i<pstr.length();i++){
        if(pstr[i].first){
            double diff=double(pstr[i].second)-mean;
            ret+=diff*diff/count;
        }
    }
    return ret;
}

double PulseDecoder::calculateMeanLow(PulseString pstr)
{
    int count=0;
    double cum=0;
    for(int i=0;i<pstr.length();i++){
        if(!pstr[i].first){
            count++;
        }
    }
    for(int i=0;i<pstr.length();i++){
        if(!pstr[i].first){
            cum+=pstr[i].second/count;
        }
    }

    return cum/count;
}

double PulseDecoder::calculateVarLow(PulseString pstr,double mean)
{
    int count=0;
    double cum=0;
    for(int i=0;i<pstr.length();i++){
        if(!pstr[i].first){
            count++;
        }
    }

    for(int i=0;i<pstr.length();i++){
        if(!pstr[i].first){
            double diff=double(pstr[i].second)-mean;
            cum+=diff*diff/count;
        }
    }

    return cum/count;
}

QString PulseDecoder::decodePulse(Pulse p)
{
    Q_ASSERT(unitlength_>0);
    if(p.first){
        return QString()+decodeHighPulse(p);
    }else{
        QString ret="";
        double lengthInUnits=double(p.second)/unitlength();
        if(lengthInUnits>2 && lengthInUnits<3.5){
            ret=" ";
            freqTreeNode_=stats_->freqTree();
        }else if(lengthInUnits>=3.5){
            ret="    ";
        }
        return ret;
    }
}

char PulseDecoder::decodeHighPulse(Pulse p)
{
    //Bayesian classification
    long length=p.second;
    //One millisecond bin for dit:
    double zDitb=(length-ditMean_)/sqrt(ditVar_);//Start
    double zDite=(length-ditMean_+1)/sqrt(ditVar_);//End
    double pDitBin=cdf(zDite)-cdf(zDitb);//Probability of length in dit bin

    //One millisecond bin for dit:
    double zDahb=(length-dahMean_)/sqrt(dahVar_);//Start
    double zDahe=(length-dahMean_+1)/sqrt(dahVar_);//End
    double pDahBin=cdf(zDahe)-cdf(zDahb);//Probability of length in dit bin

    double pDitAP=freqTreeNode_.ditRatio();
    double pDahAP=1-pDitAP;

    double ditBelievability=pDitBin*pDitAP;
    double dahBelievability=pDahBin*pDahAP;

    if(ditBelievability<dahBelievability) return '-';
    else return '.';
}

long PulseDecoder::unitlength() const
{
    return unitlength_;
}

QString PulseDecoder::decodePulseString(PulseString pstr, bool live)
{
    if(unitlength()<0) guessFromCodePulseString(pstr);
    QString ret="";
    for(int i=0;i<pstr.length();i++){
        ret+=decodePulse(pstr[i]);
    }

    if(live){
        stats_->updateCode(ret);
        memory_.append(pstr);
        while(memory_.size()>memorySize){
            memory_.removeAt(0);
        }

        guessFromCodePulseString(memory_);
    }
    return ret;
}

void PulseDecoder::clear()
{
    unitlength_=-1;
}

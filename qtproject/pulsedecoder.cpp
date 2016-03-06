#include "pulsedecoder.h"

#include <cmath>

/**
 * @brief pdf returns the normal distribution for x
 * @param x
 * @return
 */
double pdf(double x){
    static const double inv_sqrt_2pi = 0.3989422804014327;
    double exponent=-x*x/2;
    return inv_sqrt_2pi*exp(exponent);
}

/**
 * @brief cdf returns the cumulative distribution function for x
 * @param x
 * @return
 */
double cdf(double x)
{
    static const double p=0.2316419;
    static const double b1 = 0.319381530;
    static const double b2 = -0.356563782;
    static const double b3 = 1.781477937;
    static const double b4 = -1.821255978;
    static const double b5 = 1.330274429;

    double phi=pdf(x);

    double t=1(1+p*x);
    double t2= t*t;
    double t3= t2*t;
    double t4= t2*t2;
    double t5= t2*t3;

    return 1-phi*(b1*t+b2*t2+b3*t3+b4*t4+b5*t5);

}

PulseDecoder::PulseDecoder(CodeStatistics *basestats):unitlength_(-1),stats_(basestats)
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

    unitlength_=long(guessFromMeanHigh+guessFromVarHigh+0.5);
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
    int count;
    for(int i=0;i<pstr.length();i++){
        if(pstr[i].first){
            double diff=double(pstr[i].second-mean);
            ret+=diff*diff;
            count++;
        }
    }
    if(count==0)return 0;
    return ret/count;
}

QString PulseDecoder::decodePulse(Pulse p)
{
    Q_ASSERT(unitlength_>0);
    if(p.first){
        return ""+decodeHighPulse(p);
    }else{
        QString ret="";
        double lengthInUnits=double(p.second)/unitlength();
        for(int i=0;i<lengthInUnits;i++){
            ret+=" ";
        }
        return ret;
    }
}

char PulseDecoder::decodeHighPulse(Pulse p)
{
    //TODO: add a bayesian classification.
    if(p.second>meanHigh_) return '-';
    else return '.';
}

long PulseDecoder::unitlength() const
{
    return unitlength_;
}

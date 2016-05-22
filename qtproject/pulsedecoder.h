#ifndef PULSEDECODER_H
#define PULSEDECODER_H

#include "codestatistics.h"
#include "elementfrequencytree.h"

#include <QPair>
#include <QList>

typedef QPair<bool,long> Pulse;
typedef QList<Pulse> PulseString;

/**
 * @brief The PulseDecoder class takes a list of QPair<bool,long> representing the high/low
 * state of the signal and signal length, deduces the unit length and decodes the morse code.
 *
 * The class makes use of a CodeStatistics object to deduce the statistics of the ideal code,
 *
 *
 */
class PulseDecoder
{
public:
    /**
     * @brief PulseDecoder contructs a new instance.
     *
     * @param basestats pointer to the CodeStatistics to be used as a reference to the expected code.
     * PulseDecoder takes ownership of the pointer.
     */
    PulseDecoder(CodeStatistics* basestats=new CodeStatistics());
    ~PulseDecoder();
    long unitlength() const;
    /**
     * @brief decodePulseString decodes the given pulse string
     * @param pstr
     * @param live add statistics from the code string to a priori data
     * @return
     */
    QString decodePulseString(PulseString pstr,bool live=false);
    /**
     * @brief clear forgets the guessed unit length
     */
    void clear();
private:
    void guessFromCodePulseString(PulseString pstr);
    double calculateMeanHigh(PulseString pstr);
    double calculateVarHigh(PulseString pstr,double mean);
    double calculateMeanLow(PulseString pstr);
    double calculateVarLow(PulseString pstr, double mean);
    QString decodePulse(Pulse p);
    char decodeHighPulse(Pulse p);
    long unitlength_;
    /**
     * @brief meanHigh_ is the mean length of a high signal
     */
    double meanHigh_;
    /**
     * @brief varHigh_ is the variance of high signal length
     */
    double varHigh_;
    /**
     * @brief meanLow_ is the mean length of a low signal
     */
    double meanLow_;
    /**
     * @brief varLow_ is the variance of low signal length
     */
    double varLow_;

    /**
     * @brief ditMean_ is the mean length of a dit
     */
    double ditMean_;
    /**
     * @brief ditVar_ is the variance of dit length
     */
    double ditVar_;
    double dahMean_;
    double dahVar_;

    CodeStatistics* stats_;
    ElementFrequencyTree& freqTreeNode_;

    PulseString memory_;
    int memorySize;
};

#endif // PULSEDECODER_H

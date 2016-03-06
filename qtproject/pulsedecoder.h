#ifndef PULSEDECODER_H
#define PULSEDECODER_H

#include "codestatistics.h"

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

private:
    void guessFromCodePulseString(PulseString pstr);
    double calculateMeanHigh(PulseString pstr);
    double calculateVarHigh(PulseString pstr,double mean);
    QString decodePulse(Pulse p);
    char decodeHighPulse(Pulse p);
    long unitlength_;
    double meanHigh_;
    double varHigh_;
    CodeStatistics* stats_;
};

#endif // PULSEDECODER_H

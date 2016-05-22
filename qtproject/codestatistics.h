#ifndef CODESTATISTICS_H
#define CODESTATISTICS_H

#include <QString>

#include "elementfrequencytree.h"

#include <QDebug>

/**
 * @brief The CodeStatistics class provides a way to extract statistics about the
 * morse code being examined, to provide a priori information about morse code for
 * decoding purposes.
 */
class CodeStatistics
{
public:
    /**
     * @brief CodeStatistics
     */
    CodeStatistics(const QString &initPlain=QString());
    void updatePlain(const QString & plaintext);
    void updateCode(const QString & morse);

    int totalDits() const;

    int totalDahs() const;

    int interElementSpaces() const;

    int interLetterSpaces() const;

    int interWordSpaces() const;

    /**
     * @brief ditRatio calculates the ratio of dits to total elements.
     * @return
     */
    double ditRatio() const;

    /**
     * @brief interElementRatio calculates the ratio of inter-element pauses to total pauses
     * @return
     */
    double interElementRatio() const;

    double meanHigh() const;

    double varHigh() const;

    double varLow() const;

    double meanLow() const;

    ElementFrequencyTree &freqTree();

private:
    void updateStatsFromCode(const QString &code);
    /**
     * @brief addHigh adjusts meanHigh_ by adding the given double to the dataset.
     * @param d
     */
    void addHigh(double d);
    void addLow(double d);
    int totalDits_;
    int totalDahs_;
    /**
     * @brief interElementSpaces_ is the amount of dit-length spaces
     */
    int interElementSpaces_;
    /**
     * @brief interLetterSpaces_ is the amount of dah-length spaces
     */
    int interLetterSpaces_;
    /**
     * @brief interWordSpaces_ is the amount of seven-unit spaces.
     */
    int interWordSpaces_;
    /**
     * @brief meanHigh_ is the approximated mean element length in dits
     */
    double meanHigh_;
    /**
     * @brief stdHigh_ is the approximated standard deviation of element length in dits.
     */
    double varHigh_;
    /**
     * @brief meanLow_ is the approximated pause length in dits
     */
    double meanLow_;
    /**
     * @brief varLow_ is the variance of the pause length in dits.
     */
    double varLow_;
    QString codestring_;
    ElementFrequencyTree freqTree_;
};

QDebug& operator <<(QDebug& dbg,const CodeStatistics &stats);

#endif // CODESTATISTICS_H

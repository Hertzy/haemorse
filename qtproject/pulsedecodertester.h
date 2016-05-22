#ifndef PULSEDECODERTESTER_H
#define PULSEDECODERTESTER_H

#include "pulsedecoder.h"
#include <QString>
#include <random>

/**
 * @brief The PulseDecoderTester class is a class to test the performance of pusledecoder
 * with generated pulse strings.
 */
class PulseDecoderTester
{
public:
    PulseDecoderTester(PulseDecoder* target);
    /**
     * @brief test runs a battery of tests on the target and prints results to QDebug.
     */
    void test();
private:
    /**
     * @brief helloWorldTest generates the text "Hello World" in morse code with
     * the given variance in unit length.
     * @param unitLength
     * @param var
     */
    void helloWorldTest(long unitLength=400, long var=0);
    /**
     * @brief liveSendTest sends random characters in ten character packets
     * @param unitLength
     * @param var
     */
    void liveSendTest(long unitLength=400,long var=0);
    PulseString morseToPulse(QString morse,std::normal_distribution<double> dist);
    PulseDecoder* target_;
};

#endif // PULSEDECODERTESTER_H

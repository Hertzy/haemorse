#ifndef BEEPGENERATOR_HH
#define BEEPGENERATOR_HH

#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QQueue>
#include <QObject>
#include <QMutex>

class BeepGenerator: public QObject
{
    Q_OBJECT
public:
    explicit BeepGenerator(QObject* parent=0);

    /**
     * @brief BeepGenerator
     * @param f primary frequency for the beep
     * @param snr ratio of beep to noise
     * @param fvar amount of random fluctuation in frequency
     * @param harmonics amount of harmonic frequencies to include
     */
    BeepGenerator(double frequency, double snr=-1,QObject* parent=0);
    ~BeepGenerator();

    void beep(qint64 millis);
    /**
     * @brief silence produces a pause
     * @param millis
     */
    void silence(qint64 millis);

    QAudioFormat format() const;
    void setFormat(const QAudioFormat &format);

    double frequency() const;
    void setFrequency(double frequency);
protected:
    void generateBuffer();
private:
    double f_;
    double snr_;
    QAudioFormat format_;
    QAudioDeviceInfo dev_;

    QAudioOutput* out_;
    QIODevice* odev_;

    QMutex odevMutex_;

    QByteArray precalc_;
};

#endif // BEEPGENERATOR_HH

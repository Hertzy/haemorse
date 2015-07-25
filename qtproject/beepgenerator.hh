#ifndef BEEPGENERATOR_HH
#define BEEPGENERATOR_HH

#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QQueue>
#include <QObject>
#include <QMutex>

/**
 * @brief The BeepGenerator class is responsible for producing the individual dits and dahs.
 */
class BeepGenerator: public QObject
{
    Q_OBJECT
public:

    /**
     * @brief BeepGenerator
     * @param f primary frequency for the beep
     * @param snr ratio of beep to noise
     */
    BeepGenerator(double frequency=500, double snr=-1,QObject* parent=0);
    ~BeepGenerator();

    /**
     * @brief beep produces a tone of the given length
     * @param millis milliseconds to beep
     */
    void beep(qint64 millis);
    /**
     * @brief silence produces a pause
     * @param millis milliseconds of pause
     */
    void silence(qint64 millis);

    QAudioFormat format() const;
    void setFormat(const QAudioFormat &format);

    double frequency() const;
    void setFrequency(double frequency);
private:

    void generateBuffer();
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

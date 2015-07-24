#ifndef BEEPER_HH
#define BEEPER_HH

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QAudioOutput>
#include <QThread>

#include "beepgenerator.hh"
#include "encoder.h"

class Beeper : public QThread
{
    Q_OBJECT
public:
    enum BeeperMode{CONTINUOUS=0x01,
                    SINGLE_MESSAGE=0x02,
                    GENERATE_MESSAGE=0x04,
                   PHRASEBOOK=0x08};
    explicit Beeper(QObject *parent = 0);
    ~Beeper();

    /**
     * @brief addPlain encodes a Qstring and adds it to the message queue
     * @param plain
     */
    void addPlain(QString plain);
    /**
     * @brief addMorse adds an encoded message into the message queue
     * @param morse
     * @pre morse is a valid morse code string
     */
    void addMorse(QString morse);

    int mode() const;
    void setMode(const int &mode);

    qint64 unitlength() const;

    int padding() const;

    QStringList getPhrasebook() const;
    void setPhrasebook(const QStringList &value);

signals:
    /**
     * @brief messageSent is emitted when a message has been sent from the queue.
     * @param morse
     */
    void aboutToSend(QString msg);
    void charSent(QChar c);
public slots:
    //Play the next character in cache, if not in continuous mode
    void next();
    void pause();
    void setPaused(bool paused);
    void unpause();
    void setPadding(int padding);
    void setUnitlength(const int &unitlength);
protected:
    void run();
private:
    void sendMorse(QString morse);
    qint64 unitlength_;//length of a dit in milliseconds
    int mode_;
    BeepGenerator* gen_;
    QQueue<QString> msgqueue_; //Message queue
    QMutex mutex_; //Protects the message queue
    QSemaphore runsema;

    int padding_;
    bool paused;

    QStringList phrasebook;
};

#endif // BEEPER_HH

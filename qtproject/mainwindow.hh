#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

#include <QKeyEvent>
#include <QFocusEvent>
#include <QString>

#include "beeper.hh"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow builds
     * @param phrasefilename
     * @param parent
     */
    explicit MainWindow(QString phrasefilename="phrasebook",QWidget *parent = 0);
    ~MainWindow();
    bool eventFilter(QObject *object, QEvent *e);
public slots:
    void show();
    void sendingMessage(QString msg);
signals:
    void readyToReceive();
protected:
    void keyPressEvent(QKeyEvent * e);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
private slots:
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);

private:
    void insertHtml(QString toAdd);
    void newPara();
    Ui::MainWindow *ui;
    QString cur_;
    int curidx;
    Beeper* beeper;

    int rowc;
    int charc;

    //For capturing spacebar events
    bool spacedown;
};

#endif // MAINWINDOW_HH

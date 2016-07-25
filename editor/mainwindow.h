#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#define EEPROM_RESOURCE ":/res/fm7segm_rda5807.bin"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void updateHexTable(int pos);
    void updateHexTable();

private:
    QString fileName;
    QByteArray eep;

    void readEepromFile(QString name);
    void saveEepromFile(QString name);

    double getFreq(int pos);
    void setFreq (double value, int pos);

    int fmStepEep2Index(uint8_t value);
    uint8_t fmStepIndex2Step(uint8_t index);

private slots:
    void about();
    void aboutQt();

    void openEeprom();
    void saveEeprom();
    void saveEepromAs();
    void loadDefaultEeprom();

    void setTuner(int tuner);
    void setFmfreq(double value);
    void setFmMin(double value);
    void setFmMax(double value);
    void setFmstep1(int value);
    void setFmstep2(int value);
    void setFmmono(int value);
    void setFmRds(int value);
    void setFmctrl();

    void setOther();
    void setBrstby(int value);
    void setEncres(int value);
    void setVolume(double value);
    void setHourzero(int value);
};

#endif // MAINWINDOW_H

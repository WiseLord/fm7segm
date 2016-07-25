#include "mainwindow.h"

#include <QDebug>
#include <QtWidgets>

#include "aboutdialog.h"

#include "../eeprom.h"
#include "../tuner/tuner.h"
#include "../tuner/tea5767.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    /* Create hex table */
    wgtHexTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    wgtHexTable->setFont(QFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family(), 9, QFont::Bold));
    wgtHexTable->verticalHeader()->setFont(QFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family(), 9, QFont::Bold));
    wgtHexTable->horizontalHeader()->setFont(QFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family(), 9, QFont::Bold));
    for (int y = 0; y < wgtHexTable->rowCount(); y++) {
        wgtHexTable->setVerticalHeaderItem(y, new QTableWidgetItem(QString("%1").arg(y * 16, 4, 16, QChar('0')).toUpper()));
        for (int x = 0; x < 16; x++)
            wgtHexTable->setItem(y, x, new QTableWidgetItem());
    }
    for (int x = 0; x < 16; x++)
        wgtHexTable->setHorizontalHeaderItem(x, new QTableWidgetItem(QString("%1").arg(x, 0, 16).toUpper()));

    /* Load default eeprom file */
    readEepromFile(EEPROM_RESOURCE);
}

void MainWindow::updateHexTable(int pos)
{
    QTableWidgetItem *item = wgtHexTable->item(pos / 16, pos % 16);
    item->setText(eep.mid(pos, 1).toHex().toUpper());
    if (item->text() == "FF")
        item->setTextColor(Qt::gray);
    else
        item->setTextColor(Qt::black);
}

void MainWindow::updateHexTable()
{
    for (int pos = 0; pos < wgtHexTable->rowCount() * 16; pos++)
        updateHexTable(pos);
}

void MainWindow::readEepromFile(QString name)
{
    /* Reading file to QByteArray buffer */

    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
        return;
    if (name != EEPROM_RESOURCE) {
        actionSaveEeprom->setEnabled(true);
        fileName = name;
        Ui_MainWindow::statusBar->showMessage(
                    tr("File") + " " + fileName + " " + tr("loaded"));
    } else {
        actionSaveEeprom->setEnabled(false);
        fileName.clear();
        Ui_MainWindow::statusBar->showMessage("Default eeprom loaded");
    }

    eep = file.readAll();
    file.close();
    updateHexTable();

    // Processing tuner
    int tuner = eep[EEPROM_FM_TUNER];
    if (tuner >= TUNER_END)
        tuner = TUNER_TEA5767;
    setTuner(tuner);

    // Processing other functions
    setOther();
}

void MainWindow::saveEepromFile(QString name)
{
    fileName = name;

    QFile file(name);
    if (!file.open(QIODevice::WriteOnly)) {
        Ui_MainWindow::statusBar->showMessage(tr("Can't save") + " " + name);
        return;
    }
    file.write(eep);
    file.close();
    Ui_MainWindow::statusBar->showMessage(tr("Saved as") + " " + name);

}

void MainWindow::openEeprom()
{
    QString name = QFileDialog::getOpenFileName(this,
                                                tr("Open eeprom binary"),
                                                "../eeprom/",
                                                tr("EEPROM files (*.bin);;All files (*.*)"));

    readEepromFile(name);
}

void MainWindow::saveEeprom()
{
    saveEepromFile(fileName);
}

void MainWindow::saveEepromAs()
{
    QString name = QFileDialog::getSaveFileName(this,
                                                tr("Save eeprom binary"),
                                                "../eeprom/" + fileName,
                                                tr("EEPROM files (*.bin)"));

    if (name.isEmpty())
        return;

    saveEepromFile(name);
}

void MainWindow::loadDefaultEeprom()
{
    readEepromFile(EEPROM_RESOURCE);
}

double MainWindow::getFreq(int pos)
{
    double freq;

    freq = (unsigned char)eep[pos];
    freq += (unsigned char)eep[pos + 1] * 256;
    freq /= 100;

    return freq;
}

void MainWindow::setFreq(double value, int pos)
{
    int freq = value * 100;

    eep[pos] = (char)(freq & 0x00FF);
    eep[pos + 1] = (char)((freq & 0xFF00) >> 8);

    updateHexTable(pos);
    updateHexTable(pos + 1);
}

int MainWindow::fmStepEep2Index(uint8_t value)
{
    if (value < 2)
        return 0;
    else if (value < 5)
        return 1;
    else if (value < 10)
        return 2;
    else if (value < 20)
        return 3;
    else
        return 4;
}

uint8_t MainWindow::fmStepIndex2Step(uint8_t index)
{
    switch (index) {
    case 1:
        return 2;
    case 2:
        return 5;
    case 3:
        return 10;
    case 4:
        return 20;
    default:
        return 1;
    }
}

void MainWindow::about()
{
    AboutDialog dlg;

    dlg.exec();
}

void MainWindow::aboutQt()
{
    qApp->aboutQt();
}

void MainWindow::setTuner(int tuner)
{
    cbxTuner->setCurrentIndex(tuner);
    wgtFmfreq->hide();
    wgtFmMin->hide();
    wgtFmMax->hide();
    wgtFmstep1->hide();
    wgtFmstep2->hide();
    wgtFmmono->hide();
    wgtFmRDS->hide();
    wgtFmctrl->hide();

    switch (tuner) {
    case TUNER_TEA5767:
        wgtFmctrl->show();
        cbxFmctrlHcc->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_HCC);
        cbxFmctrlSnc->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_SNC);
        cbxFmctrlSm->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_SMUTE);
        cbxFmctrlDtc->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_DTC);
        cbxFmctrlBl->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_BL);
        cbxFmctrlPllref->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_PLLREF);
        cbxFmctrlXtal->setChecked(eep[EEPROM_FM_CTRL] & TEA5767_XTAL);
    case TUNER_RDA5807:
    case TUNER_RDA5807_DF:
        if (tuner != TUNER_TEA5767)
            wgtFmRDS->show();
        setFmRds(eep[EEPROM_FM_RDS]);
        cbxFmRDS->setCurrentIndex(eep[EEPROM_FM_RDS]);
    case TUNER_RDA5802:
        wgtFmmono->show();
        setFmmono(eep[EEPROM_FM_MONO]);
        cbxFmmono->setCurrentIndex(eep[EEPROM_FM_MONO]);
    case TUNER_TUX032:
    case TUNER_LM7001:
        wgtFmfreq->show();
        wgtFmMin->show();
        wgtFmMax->show();
        dsbFmfreq->setValue(getFreq(EEPROM_FM_FREQ));
        dsbFmMin->setValue(getFreq(EEPROM_FM_FREQ_MIN));
        dsbFmMax->setValue(getFreq(EEPROM_FM_FREQ_MAX));
        if (dsbFmfreq->value() < 76)
            dsbFmfreq->setSingleStep((double)eep[EEPROM_FM_STEP1] / 100);
        else
            dsbFmfreq->setSingleStep((double)eep[EEPROM_FM_STEP2] / 100);
        wgtFmstep1->show();
        wgtFmstep2->show();
        cbxFmstep1->setCurrentIndex(fmStepEep2Index(eep[EEPROM_FM_STEP1]));
        cbxFmstep2->setCurrentIndex(fmStepEep2Index(eep[EEPROM_FM_STEP2]));
        break;
    }

    eep[EEPROM_FM_TUNER] = tuner;
    updateHexTable(EEPROM_FM_TUNER);
}

void MainWindow::setFmfreq(double value)
{
    if (value < 76)
        dsbFmfreq->setSingleStep((double)fmStepIndex2Step(cbxFmstep1->currentIndex()) / 100);
    else
        dsbFmfreq->setSingleStep((double)fmStepIndex2Step(cbxFmstep2->currentIndex()) / 100);
    setFreq(value, EEPROM_FM_FREQ);
}

void MainWindow::setFmMin(double value)
{
    if (value < 76)
        dsbFmMin->setSingleStep((double)fmStepIndex2Step(cbxFmstep1->currentIndex()) / 100);
    else
        dsbFmMin->setSingleStep((double)fmStepIndex2Step(cbxFmstep2->currentIndex()) / 100);
    setFreq(value, EEPROM_FM_FREQ_MIN);
}

void MainWindow::setFmMax(double value)
{
    if (value < 76)
        dsbFmMax->setSingleStep((double)fmStepIndex2Step(cbxFmstep1->currentIndex()) / 100);
    else
        dsbFmMax->setSingleStep((double)fmStepIndex2Step(cbxFmstep2->currentIndex()) / 100);
    setFreq(value, EEPROM_FM_FREQ_MAX);
}

void MainWindow::setFmstep1(int value)
{
    eep[EEPROM_FM_STEP1] = fmStepIndex2Step(value);
    updateHexTable(EEPROM_FM_STEP1);
}

void MainWindow::setFmstep2(int value)
{
    eep[EEPROM_FM_STEP2] = fmStepIndex2Step(value);
    updateHexTable(EEPROM_FM_STEP2);
}

void MainWindow::setFmmono(int value)
{
    if (value)
        eep[EEPROM_FM_MONO] = 0x01;
    else
        eep[EEPROM_FM_MONO] = 0x00;
    updateHexTable(EEPROM_FM_MONO);
}

void MainWindow::setFmRds(int value)
{
    if (value)
        eep[EEPROM_FM_RDS] = 0x01;
    else
        eep[EEPROM_FM_RDS] = 0x00;
    updateHexTable(EEPROM_FM_RDS);
}

void MainWindow::setFmctrl()
{
    char ctrl = 0;

    if (cbxFmctrlHcc->isChecked()) ctrl |= TEA5767_HCC;
    if (cbxFmctrlSnc->isChecked()) ctrl |= TEA5767_SNC;
    if (cbxFmctrlSm->isChecked()) ctrl |= TEA5767_SMUTE;
    if (cbxFmctrlDtc->isChecked()) ctrl |= TEA5767_DTC;
    if (cbxFmctrlBl->isChecked()) ctrl |= TEA5767_BL;
    if (cbxFmctrlPllref->isChecked()) ctrl |= TEA5767_PLLREF;
    if (cbxFmctrlXtal->isChecked()) ctrl |= TEA5767_XTAL;

    eep[EEPROM_FM_CTRL] = ctrl;
    updateHexTable(EEPROM_FM_CTRL);
}

void MainWindow::setOther()
{
    setBrstby(eep[EEPROM_BR_STBY]);
    sbxBrstby->setValue(eep[EEPROM_BR_STBY]);

    setEncres(eep[EEPROM_ENC_RES]);
    sbxEncres->setValue(eep[EEPROM_ENC_RES]);

    setVolume(eep[EEPROM_VOLUME]);
    dsbVolume->setValue(eep[EEPROM_VOLUME]);
}

void MainWindow::setBrstby(int value)
{
    if (value > sbxBrstby->maximum())
        value = 1;
    eep[EEPROM_BR_STBY] = (char)value;
    updateHexTable(EEPROM_BR_STBY);
}

void MainWindow::setEncres(int value)
{
    if (value > sbxEncres->maximum())
        value = sbxEncres->maximum();
    if (value < sbxEncres->minimum())
        value = sbxEncres->minimum();
    eep[EEPROM_ENC_RES] = (char)value;
    updateHexTable(EEPROM_ENC_RES);
}

void MainWindow::setVolume(double value)
{
    eep[EEPROM_VOLUME] = static_cast<char>(value / dsbVolume->singleStep());
    updateHexTable(EEPROM_VOLUME);
}

void MainWindow::setHourzero(int value)
{
  eep[EEPROM_HOURZERO] = (char) value;
  updateHexTable(EEPROM_HOURZERO);
}

#ifndef RECORDINGSFORM_H
#define RECORDINGSFORM_H

#include <QDialog>
#include <QtCore>
#include "tempostructs.h"
#include "recordsworker.h"

namespace Ui {
class Recordingsform;
}

class Recordingsform : public QDialog
{
    Q_OBJECT

public:
    explicit Recordingsform(QWidget *parent = nullptr);
    ~Recordingsform();
    bool bStarted;
    void start();
    void stop();
    QThread* RecordsWorkerThread;
    RecordsWorker* RecordsWorker;


signals:
    void icdPassRX(icd::flash_file_header_type* pMsg,uint8_t nTotalRecs);

private slots:
    void on_Recordingsform_rejected();
    void icdPassTX(icd::icd_template* pMsg);
    void addHeaderToTable(icd::flash_file_header_type* pHeader);
    void on_DownSelButton_clicked();
    void save_to_file(uint16_t* pSamples);
    void finishfile();
private:
    Ui::Recordingsform *ui;

    QString filename;
    QFile file;
    void buildTable();

};

#endif // RECORDINGSFORM_H

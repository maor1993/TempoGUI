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
    void icdPassTX(icd::icd_req_recording_type* pMsg);
    void addHeaderToTable(icd::flash_file_header_type* pHeader);
private:
    Ui::Recordingsform *ui;

    void buildTable();

};

#endif // RECORDINGSFORM_H

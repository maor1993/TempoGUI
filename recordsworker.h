#ifndef RECORDSWORKER_H
#define RECORDSWORKER_H

#include <QObject>
#include <QSemaphore>
#include "tempostructs.h"

typedef enum {
    CollectFirstHeader_state,CollectOtherheaders_state,CollectionDone_State
}RecordCollectionStateMachineType;


class RecordsWorker : public QObject
{
    Q_OBJECT
public:
    explicit RecordsWorker(QWidget *parent = nullptr);
    void DoSetup(QThread &cThread);
    uint8_t nTotalRecs;
private:
    RecordCollectionStateMachineType CollectionState;

    uint8_t nIdxToGet;
    QSemaphore* idxSemaphore;


signals:
    void postHeader();
    void icdPostRequest(icd::icd_req_recording_type* pMsg);
    void SendToTable(icd::flash_file_header_type* pMsg);
public slots:
    void DoWork();
    void ReceiveHeader(icd::flash_file_header_type* pRec,uint8_t nTotalRecs);
};

#endif // RECORDSWORKER_H

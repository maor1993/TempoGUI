#ifndef RECORDSWORKER_H
#define RECORDSWORKER_H

#include <QObject>
#include <QSemaphore>
#include "tempostructs.h"

typedef enum {
    CollectFirstHeader_state,CollectOtherheaders_state,CollectionDone_State
}RecordCollectionStateMachineType;
typedef enum {
    WaitForsample_state,Requestsample_state,SampleDone_state
}SampleCollectionStateMachineType;

class RecordsWorker : public QObject
{
    Q_OBJECT
public:
    explicit RecordsWorker(QWidget *parent = nullptr);
    void DoSetup(QThread &cThread);
    uint8_t nTotalRecs;
    void DownloadRecording(int nIdx);

private:
    RecordCollectionStateMachineType CollectionState;
    SampleCollectionStateMachineType SampleState;
    uint8_t nIdxToGet;
    uint16_t nSetToGet;
    uint32_t nAddr;
    QSemaphore* idxSemaphore;
    QList<icd::flash_file_header_type> HeaderList;
    icd::flash_file_header_type pRecToDownload;
    int nRequestsNeeded;
    uint16_t SampleArray[16];
signals:
    void postHeader();
    void icdPostRequest(icd::icd_template* pMsg);
    void SendToTable(icd::flash_file_header_type* pMsg);
    void SendToFile(uint16_t* pSamples);
    void FinishFile();
public slots:
    void DoWork();
    void ReceiveHeader(icd::flash_file_header_type* pRec,uint8_t nTotalRecs);
    void ReceiveSamples(icd::icd_requested_samples_type* pRec);
};

#endif // RECORDSWORKER_H

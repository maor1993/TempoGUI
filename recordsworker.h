#ifndef RECORDSWORKER_H
#define RECORDSWORKER_H

#include <QObject>
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
    void RecieveHeader(icd::flash_file_header_type* pRec,uint8_t nTotalRecs);

private:
    RecordCollectionStateMachineType CollectionState;
    uint8_t nTotalRecs;

signals:
    void postHeader();
public slots:
    void DoWork();
};

#endif // RECORDSWORKER_H

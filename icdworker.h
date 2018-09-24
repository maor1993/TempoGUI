#ifndef ICDWORKER_H
#define ICDWORKER_H

#include <QObject>
#include <QSerialPort>
#include "tempostructs.h"
#include <QQueue>
typedef enum
{
    Premble_a_st,
    Premble_b_st,
    Sequnce_st,
    Request_st,
    MessageType_st,
    MessageLen_st,
    MessageCollect_st
}ParseIcdStateMachineType;


class icdworker : public QObject
{
    Q_OBJECT
public:
    explicit icdworker(QObject *parent = nullptr);
    void DoSetup(bool bWorkerType,QSerialPort* serial,QThread &cThread);
    void addIcdMsgtoQueue(icd::icd_template* pMsg);
    void USBDisconnect();
    int USBConnect(QSerialPortInfo* SerialInfo);

signals:
    void SendToParser(icd::icd_template* pMsg);
    void SendToTransmit(icd::icd_template* pMsg);
public slots:
    void DoWorkTX();

    void ReceiveUsbMsg();
private:
    QSerialPort* serial;
    ParseIcdStateMachineType ParseIcdStateMachine;
    QQueue<icd::icd_template>* IcdTxQueue;
};

#endif // ICDWORKER_H

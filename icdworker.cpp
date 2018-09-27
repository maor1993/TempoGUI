#include "icdworker.h"
#include "tempostructs.h"
#include <QtCore>
#include <QSerialPort>
#include "mainwindow.h"
#include "tempostructs.h"

icdworker::icdworker(QObject *parent) : QObject(parent)
{
    ParseIcdStateMachine = Premble_a_st;
    IcdTxQueue = new QQueue<icd::icd_template>();
}

int icdworker::USBConnect(QSerialPortInfo* SerialInfo)
{




    return 0;
}
void icdworker::USBDisconnect()
{
    serial->close();

}


void icdworker::DoSetup(bool bWorkerType,QSerialPort* serial,QThread &cThread)
{
    this->serial = serial;


    if(bWorkerType) //tx
    {
        connect(&cThread,SIGNAL(started()),this,SLOT(DoWorkTX()));

    }
    else    //rx
    {
        connect(serial,SIGNAL(readyRead()),this,SLOT(ReceiveUsbMsg()));
    }





}


void icdworker::DoWorkTX()
{
    icd::icd_template msg;

    while(1)
    {
        if(!IcdTxQueue->isEmpty())
        {
            msg = IcdTxQueue->dequeue();

            SendToTransmit(&msg);
        }
        QThread::msleep(100);

    }
}



void icdworker::addIcdMsgtoQueue(icd::icd_template* pMsg)
{
    IcdTxQueue->enqueue(*pMsg);
}


void icdworker::ReceiveUsbMsg()
{

    //step 1: collect data from serial device.
    char rxdata[512];
    uint16_t nrxdata_size;
    static uint16_t i=0;
    static icd::icd_template icd_msg;




    nrxdata_size = serial->read(rxdata,sizeof(rxdata));
    //check if there is more than a header in the buffer.
        for(uint16_t ii =0;ii<nrxdata_size;ii++)
        {
            switch(ParseIcdStateMachine)
            {
                case Premble_a_st:
                {

                if(static_cast<uint8_t>(rxdata[ii]) ==0xdd)
                {
                    ParseIcdStateMachine = Premble_b_st;
                    icd_msg.sHeader.nPremble[0] = static_cast<uint8_t>(rxdata[ii]);

                }

                break;
                }
            case Premble_b_st:
            {
                if(static_cast<uint8_t>(rxdata[ii]) == 0xc0)
                {
                    ParseIcdStateMachine = Premble_c_st;
                    icd_msg.sHeader.nPremble[1] = static_cast<uint8_t>(rxdata[ii]);
                }
                else
                {
                    ParseIcdStateMachine = Premble_a_st;
                }
            break;
            }
            case Premble_c_st:
            {
                if(static_cast<uint8_t>(rxdata[ii]) == 0x5b)
                {
                    ParseIcdStateMachine = Premble_d_st;
                    icd_msg.sHeader.nPremble[2] = static_cast<uint8_t>(rxdata[ii]);
                }
                else
                {
                    ParseIcdStateMachine = Premble_a_st;
                }
            break;
            }
            case Premble_d_st:
            {
                if(static_cast<uint8_t>(rxdata[ii]) == 0x9a)
                {
                    ParseIcdStateMachine = Sequnce_st;
                    icd_msg.sHeader.nPremble[3] = static_cast<uint8_t>(rxdata[ii]);
                }
                else
                {
                    ParseIcdStateMachine = Premble_a_st;
                }




            break;
            }






            case Sequnce_st:
            {
                icd_msg.sHeader.nSequence = static_cast<uint8_t>(rxdata[ii]);
                ParseIcdStateMachine = Request_st;
            break;
            }
            case Request_st:
            {

                icd_msg.sHeader.nReq = static_cast<uint8_t>(rxdata[ii]);
                ParseIcdStateMachine = MessageType_st;
            break;
            }
            case MessageType_st:
        {

                icd_msg.sHeader.nMsgtype = static_cast<uint8_t>(rxdata[ii]);
                ParseIcdStateMachine = MessageLen_st;
            break;
        }
            case MessageLen_st:
        {

                icd_msg.sHeader.nMsglen = static_cast<uint8_t>(rxdata[ii]);
                ParseIcdStateMachine = MessageCollect_st;
            break;
        }
            case MessageCollect_st:
        {

                if(i < icd_msg.sHeader.nMsglen)
                {
                    icd_msg.nMsgdata[i++] = static_cast<uint8_t>(rxdata[ii]);
                }
                else if(i > 255)
                {
                    i=0;
                    ParseIcdStateMachine = Premble_a_st;

                }
                else
                {
                    i=0;

                    SendToParser(&icd_msg);
                    ParseIcdStateMachine = Premble_a_st;
                }
            break;
        }

    }
}

}

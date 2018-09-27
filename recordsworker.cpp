#include <QtCore>
#include "recordsworker.h"
#include "recordingsform.h"



RecordsWorker::RecordsWorker(QWidget *parent) : QObject(parent)
{
    CollectionState = CollectFirstHeader_state;
    nIdxToGet =0;
    nTotalRecs =0;
    idxSemaphore = new QSemaphore(1);

}



void RecordsWorker::DoSetup(QThread &cThread)
{
    connect(&cThread,SIGNAL(started()),this,SLOT(DoWork()));

}


void RecordsWorker::ReceiveHeader(icd::flash_file_header_type* pRec,uint8_t nTotalrecs)
{
    switch (CollectionState) {
        case CollectFirstHeader_state:
        //this is the first header from the chip. use it to indicate how many recordings are here.
        nTotalRecs = nTotalrecs-1;




        qDebug() << "total recs:" << QString::number(nTotalRecs);
        if(pRec->Signature != 0xfeedabba)
        {
            //there are no recordings on the devices.
            CollectionState = CollectionDone_State;
            qDebug() << "No Recordings!";
            break;
        }
        else if(nTotalRecs == 1)
        {
            //this is the last recording on the device.
            CollectionState = CollectionDone_State;
            qDebug() << "Only One!";
        }
        else
        {
            //collect the rest of the headers.
            CollectionState = CollectOtherheaders_state;

            //increment the idx to request.
            nIdxToGet++;
            qDebug() << "Getting More!";
        }
        SendToTable(pRec);


        break;

        case CollectOtherheaders_state:

        if(pRec->nRecordnum == (nTotalRecs - 1))
        {
            //this is the final recording on the device.
            CollectionState = CollectionDone_State;
            qDebug() << "last header collected!";
        }


        //check if we already received this header.
        if(nIdxToGet == pRec->nRecordnum)
        {

            SendToTable(pRec);

            qDebug() << "Rec Manager: attempting to get semaphore";
            idxSemaphore->acquire();
            nIdxToGet++;
            idxSemaphore->release();
        }




        break;
    default:

        break;


    }



}

void RecordsWorker::DoWork()
{

    icd::icd_req_recording_type sReq;

    //build the generic request.
    sReq.sHeader.nPremble[0] = ICD_PREMBLE&0xff;
    sReq.sHeader.nPremble[1] = (ICD_PREMBLE&0xff00)>>8;
    sReq.sHeader.nPremble[2] = (ICD_PREMBLE&0xff0000)>>16;
    sReq.sHeader.nPremble[3] = (ICD_PREMBLE&0xff000000)>>24;
    sReq.sHeader.nMsgtype = USB_MSG_TYPE_REPORT_RECORDING;
    sReq.sHeader.nMsglen = 1;
    sReq.sHeader.nReq = USB_REQ_MASTER_RECEIVE;

    //note that this icd message is overloaded.
    while(1)
    {
        switch(CollectionState)
        {
            case CollectFirstHeader_state:
            //step one, request the first header.

            //the state machine will exit from an external source, so if not exited, just request again.
            sReq.nIdx = 0 ;

            icdPostRequest(&sReq);

            qDebug() << "Rec Worker: Sent request.";
            //sleep for 1 sec and send the request again.
            QThread::msleep(1000);


            break;

            case CollectOtherheaders_state:

            //attempt to get the semaphore
            qDebug()<< "rec sender: attempting to get seamphore";
            idxSemaphore->acquire();
               sReq.nIdx = nIdxToGet;
            idxSemaphore->release();
               icdPostRequest(&sReq);
            qDebug()<<"requested header number:" << QString::number(sReq.nIdx);
            QThread::msleep(1000); //might be problamtic as the thread will be sleeping.

            break;
        default:
            break;




        }





    }
}

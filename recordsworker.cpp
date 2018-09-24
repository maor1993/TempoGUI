#include <QtCore>
#include "recordsworker.h"
#include "recordingsform.h"



RecordsWorker::RecordsWorker(QWidget *parent) : QObject(parent)
{
    CollectionState = CollectFirstHeader_state;
}



void RecordsWorker::DoSetup(QThread &cThread)
{
    connect(&cThread,SIGNAL(started()),this,SLOT(DoWork()));

}


void RecordsWorker::RecieveHeader(icd::flash_file_header_type* pRec,uint8_t nTotalrecs)
{
    switch (CollectionState) {
        case CollectFirstHeader_state:
        //this is the first header from the chip. use it to indicate how many recordings are here.
        nTotalRecs = nTotalrecs;

        dynamic_cast<Recordingsform*>(this->parent())->addHeaderToTable(pRec);
        break;

    default:

        break;


    }



}

void RecordsWorker::DoWork()
{
    uint8_t nIdxToCollect=0;

    while(1)
    {
        switch(CollectionState)
        {
            case CollectFirstHeader_state:
            //step one, request the first header.

            //the state machine will exit from an external source, so if not exited, just request again.


            break;

        default:
            break;




        }





    }
}

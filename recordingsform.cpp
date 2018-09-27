#include <QtCore>
#include "recordingsform.h"
#include "ui_recordingsform.h"
#include "recordsworker.h"
#include "mainwindow.h"

Recordingsform::Recordingsform(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Recordingsform)
{
    ui->setupUi(this);
    bStarted = false;
    RecordsWorkerThread = new QThread(this);

    ui->progressBar->setValue(0);
    ui->progressBar->setTextVisible(true);
    ui->progressBar->setFormat("Collecting Headers...");
    buildTable();
}


void Recordingsform::buildTable()
{
    QStringList TableHeaders;

    TableHeaders << "#" <<  "Samples" << "Timediff";
    ui->table->setColumnCount(3);
    ui->table->setHorizontalHeaderLabels(TableHeaders);
    ui->table->verticalHeader()->setVisible(false);
}

void Recordingsform::start()
{
    RecordsWorker = new class RecordsWorker();



    RecordsWorker->DoSetup(*RecordsWorkerThread);
    RecordsWorker->moveToThread(RecordsWorkerThread);

    //connect worker icd pipe.
    connect(RecordsWorker,SIGNAL(icdPostRequest(icd::icd_req_recording_type*)),this,SLOT(icdPassTX(icd::icd_req_recording_type*)));
    connect(this,SIGNAL(icdPassRX(icd::flash_file_header_type*,uint8_t)),RecordsWorker,SLOT(ReceiveHeader(icd::flash_file_header_type*,uint8_t)));
    connect(RecordsWorker,SIGNAL(SendToTable(icd::flash_file_header_type*)),this,SLOT(addHeaderToTable(icd::flash_file_header_type*)));


    RecordsWorkerThread->start();

    bStarted =true;
    this->show();
}

void Recordingsform::addHeaderToTable(icd::flash_file_header_type* pHeader)
{
    static uint8_t nTableidx=0;

    //add to table the header.
    ui->table->insertRow(nTableidx);
    ui->table->setItem(nTableidx,0,new QTableWidgetItem(QString::number(pHeader->nRecordnum)));
    ui->table->setItem(nTableidx,1,new QTableWidgetItem(QString::number(pHeader->nNumOfSamples)));
    ui->table->setItem(nTableidx++,2,new QTableWidgetItem(QString::number(icd::timediffs[pHeader->eTimeDiff])));

    ui->table->sortByColumn(0,Qt::AscendingOrder);


    //update progress bar
    ui->progressBar->setRange(0,RecordsWorker->nTotalRecs);

    ui->progressBar->setValue(pHeader->nRecordnum+1);

    if((pHeader->nRecordnum+1) == RecordsWorker->nTotalRecs)
    {
        ui->progressBar->setFormat("Done!");
    }
}

Recordingsform::~Recordingsform()
{
    delete ui;
}

void Recordingsform::on_Recordingsform_rejected()
{
    bStarted = false;
    qDebug()<< "Window Closed!";
}

void Recordingsform::icdPassTX(icd::icd_req_recording_type* pMsg)
{
    icd::icd_template* msg;

    msg = (icd::icd_template*)(&pMsg->sHeader.nPremble[0]);

    dynamic_cast<MainWindow*>(this->parent())->icdTxWorker->addIcdMsgtoQueue(msg);


}


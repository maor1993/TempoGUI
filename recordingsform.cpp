#include <QtCore>
#include "recordingsform.h"
#include "ui_recordingsform.h"
#include "recordsworker.h"


Recordingsform::Recordingsform(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Recordingsform)
{
    ui->setupUi(this);
    bStarted = false;
    RecordsWorkerThread = new QThread(this);


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

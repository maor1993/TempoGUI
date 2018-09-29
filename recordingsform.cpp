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

    TableHeaders << "#" <<  "Samples" << "Time Delta (Secs)"<<"Total Time (Secs)";
    ui->table->setColumnCount(4);
    ui->table->setHorizontalHeaderLabels(TableHeaders);
    ui->table->verticalHeader()->setVisible(false);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void Recordingsform::start()
{
    RecordsWorker = new class RecordsWorker();



    RecordsWorker->DoSetup(*RecordsWorkerThread);
    RecordsWorker->moveToThread(RecordsWorkerThread);

    //connect worker icd pipe.
    connect(RecordsWorker,SIGNAL(icdPostRequest(icd::icd_template*)),this,SLOT(icdPassTX(icd::icd_template*)));
    connect(RecordsWorker,SIGNAL(SendToTable(icd::flash_file_header_type*)),this,SLOT(addHeaderToTable(icd::flash_file_header_type*)));
    connect(RecordsWorker,SIGNAL(SendToFile(uint16_t*)),this,SLOT(save_to_file(uint16_t*)));
    connect(RecordsWorker,SIGNAL(FinishFile()),this,SLOT(finishfile()));
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
    ui->table->setItem(nTableidx,2,new QTableWidgetItem(QString::number(icd::timediffs[pHeader->eTimeDiff])));
    ui->table->setItem(nTableidx++,3,new QTableWidgetItem(QString::number((pHeader->nNumOfSamples)*(icd::timediffs[pHeader->eTimeDiff]))));

    ui->table->sortByColumn(0,Qt::AscendingOrder);


    //update progress bar
    ui->progressBar->setRange(0,RecordsWorker->nTotalRecs);

    ui->progressBar->setValue(pHeader->nRecordnum+1);

    if((pHeader->nRecordnum+1) == RecordsWorker->nTotalRecs)
    {
        ui->progressBar->setFormat("Done!                     ");
    }
}

Recordingsform::~Recordingsform()
{
    delete ui;
}

void Recordingsform::on_Recordingsform_rejected()
{
    bStarted = false;

    //todo: add thread killer here.
    qDebug()<< "Window Closed!";
}

void Recordingsform::icdPassTX(icd::icd_template* pMsg)
{

    dynamic_cast<MainWindow*>(this->parent())->icdTxWorker->addIcdMsgtoQueue(pMsg);


}


void Recordingsform::on_DownSelButton_clicked()
{
    //step 1: verify that idx is selected.
    QModelIndexList indexes = ui->table->selectionModel()->selection().indexes();


    if(indexes.count() != 4)
    {
        QMessageBox::critical(this,"Error","Please Select one index!");
        return;
    }

        //step 2, get the row
        QModelIndex index = indexes.value(0);

        int data = index.row();

        //get the file to save to

        filename = QFileDialog::getSaveFileName(this,"Download Recording","","CSV file (*.csv)");
        if(filename.isEmpty())
        {
            QMessageBox::warning(this,"Error","Please Select File To Save!");
            return;
        }
        else
        {
            file.setFileName(filename);
            if(!file.open(QIODevice::WriteOnly))
            {
                QMessageBox::warning(this, tr("Unable to open file"),file.errorString());
                return;
            }
        }

        //todo: write header for file?


        //request download from worker.
        RecordsWorker->DownloadRecording(data);

        qDebug() << QString::number(data);

}


void Recordingsform::save_to_file(uint16_t* pSamples)
{
    static int sampletime=0;


    if(!file.isOpen())
    {

        //no idea how we're here, but don't try to save..
        return;
    }


    for(int i=0;i<16;i++)
    {
        QString str;

        str = QString::number((*(pSamples+i)>>5)*0.125) + "," + QString::number(sampletime*1) + "\n"; //todo: fix this!
        sampletime++;
        file.write(str.toUtf8());
    }

}void Recordingsform::finishfile()
{
    //close the file.
    if(file.isOpen())
    {
        file.close();
        QMessageBox::information(this,"Record Download","Done!");
    }




}

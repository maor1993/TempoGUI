
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "usbdevicesform.h"
#include "tempoicd.h"
#include "tempostructs.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(383, 214));


    //generate objects

    UsbDevicesDialog = new UsbDevicesForm(this);
    RecordingsDialog = new Recordingsform(this);

    icdTxWorker = new icdworker();
    icdTxWorkerThread = new QThread(this);

    icdRxWorker = new icdworker();
    icdRxWorkerThread = new QThread(this);
    ui->RecordingPushButton->setEnabled(true);



    serial = new QSerialPort();
    //connect the serial port.


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ConnectPushButton_clicked()
{
    //checked if thread started
    if(!UsbDevicesDialog->bThreadStarted)
    {   
        UsbDevicesDialog->exec();
    }


    //setup port settings.
    serial->setPortName(UsbDevicesDialog->PortToConnectInfo.portName());
    serial->setBaudRate(230400);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    //attempt to connect to port.
    if((serial->open(QIODevice::ReadWrite) == true)){
        //update ui logo
        ui->DevConnectedCheckBox->setCheckable(true);
        ui->DevConnectedCheckBox->setChecked(true);
        ui->ConnectPushButton->setEnabled(false);
        ui->UpdatePushButton->setEnabled(true);
        ui->RecordingPushButton->setEnabled(true);

        //setup the worker.
        icdTxWorker->DoSetup(1,serial,*icdTxWorkerThread);
        icdTxWorker->moveToThread(icdTxWorkerThread);

        connect(icdTxWorker,SIGNAL(SendToTransmit(icd::icd_template*)),this,SLOT(IcdTx(icd::icd_template*))
                );

        icdRxWorker->DoSetup(0,serial,*icdRxWorkerThread);
        //icdRxWorkerThread->moveToThread(icdRxWorkerThread);
        //connect the parser.
        connect(icdRxWorker,SIGNAL(SendToParser(icd::icd_template*)),this,SLOT(IcdParse(icd::icd_template*)));





        icdTxWorkerThread->start();
        qDebug()<< "icd thread started.";
    }
    else
    {
        QMessageBox::warning(this,"port","failed to connect to port "+ UsbDevicesDialog->PortToConnectInfo.portName());
    }


}

void MainWindow::on_DevConnectedCheckBox_stateChanged(int arg1)
{
    if(ui->DevConnectedCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        serial->close();
        ui->DevConnectedCheckBox->setCheckable(false);
        ui->ConnectPushButton->setEnabled(true);
        ui->UpdatePushButton->setEnabled(false);
        ui->RecordingPushButton->setEnabled(false);
    }
}







void MainWindow::IcdParse(icd::icd_template* pMsg)
{
    switch(pMsg->sHeader.nReq)
    {
            case(USB_REQ_MASTER_SEND):
        {

            switch(pMsg->sHeader.nMsgtype)
            {
                case(USB_MSG_TYPE_STATUS):
                    {
                        //copy the new message.
                        icd::icd_status_msg_type* msg = new icd::icd_status_msg_type();

                        memcpy(static_cast<uint8_t*>(&(msg->sHeader.nPremble[0])),static_cast<uint8_t*>(&(pMsg->sHeader.nPremble[0])),sizeof(icd::icd_header)+pMsg->sHeader
                               .nMsglen);

                        //update the screen with current temperture
                        ui->TempTextBox->setText(QString::number(((msg->nTemp)&0xff00)>>8));
                        ui->BatteryBar->setValue(msg->nBattPercent);

                        delete msg;
                        break;
                    }



            }


        break;
        }
    case(USB_REQ_MASTER_RECEIVE):
    {
        switch(pMsg->sHeader.nMsgtype)
        {
        case(USB_MSG_TYPE_REPORT_RECORDING):
        {
            if(RecordingsDialog->bStarted)
            {
                icd::icd_get_recording_type* msg = new icd::icd_get_recording_type();

                memcpy(static_cast<uint8_t*>(&(msg->sHeader.nPremble[0])),static_cast<uint8_t*>(&(pMsg->sHeader.nPremble[0])),sizeof(icd::icd_header)+pMsg->sHeader.nMsglen);
                qDebug() << "Received header!";
                RecordingsDialog->RecordsWorker->ReceiveHeader(&(msg->sRecHeader),msg->nTotalRecs);
            }

        break;
        }




        }


        break;
    }

    default:
        qDebug() << "request type is unkown!" << QString::number(pMsg->sHeader.nReq);
        break;

    }



}


void MainWindow::on_UpdatePushButton_clicked()
{
    QMessageBox::StandardButton res;
    res = QMessageBox::warning(this,"Update Device","Warning!, this will set your device to update mode\n And will not work until updated. are you Sure?",QMessageBox::Yes|QMessageBox::No);

    if(res == QMessageBox::Yes)
    {
        IcdGenerateEmptyRequest(USB_MSG_TYPE_START_UPDATE);
        //send update command to device.
    }


}


void MainWindow::IcdGenerateEmptyRequest(uint8_t nMsgType)
{
    icd::icd_template msg;

    msg.sHeader.nPremble[0] = ICD_PREMBLE&0xff;
    msg.sHeader.nPremble[1] = (ICD_PREMBLE&0xff00)>>8;
    msg.sHeader.nPremble[2] = (ICD_PREMBLE&0xff0000)>>16;
    msg.sHeader.nPremble[3] = (ICD_PREMBLE&0xff000000)>>24;
    msg.sHeader.nMsglen = 0;
    msg.sHeader.nSequence = 0;
    msg.sHeader.nReq = USB_REQ_MASTER_SEND;
    msg.sHeader.nMsgtype = nMsgType;




    icdTxWorker->addIcdMsgtoQueue(&msg);
}


void MainWindow::IcdTx(icd::icd_template* pMsg)
{
    char usbTxArray[USB_MAX_MSG_SIZE];

    memcpy(&(usbTxArray[0]),static_cast<uint8_t*>(&(pMsg->sHeader.nPremble[0])),pMsg->sHeader.nMsglen+sizeof(icd::icd_header));
    serial->write(usbTxArray,pMsg->sHeader.nMsglen + sizeof(icd::icd_header));
}

void MainWindow::on_RecordingPushButton_clicked()
{
    //checked if thread started
    if(!RecordingsDialog->bStarted)
    {
        RecordingsDialog->start();

        qDebug() << "started recording form.";
    }
    else
    {
        RecordingsDialog->show();
    }
}




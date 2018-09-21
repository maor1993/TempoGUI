#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "usbdevicesform.h"
#include "tempoicd.h"
#include "tempostructs.h"

Ui::ParseIcdStateMachineType MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_a_st;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(383, 214));
    UsbDevicesDialog = new UsbDevicesForm(this);
    serial = new QSerialPort();
    //connect the serial port.
    connect(serial,SIGNAL(readyRead()),this,SLOT(ReceiveUsbMsg()));

    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_a_st;
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
    }
}


void MainWindow::ReceiveUsbMsg()
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
            switch(MainWindow::ParseIcdStateMachine)
            {
                case Ui::ParseIcdStateMachineType::Premble_a_st:
                {

                if(static_cast<uint8_t>(rxdata[ii]) ==0xa5)
                {
                    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_b_st;
                }

                break;
                }
            case Ui::ParseIcdStateMachineType::Premble_b_st:
            {
                if(static_cast<uint8_t>(rxdata[ii]) == 0xa5)
                {
                    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Sequnce_st;
                }
                else
                {
                    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_a_st;
                }
            break;
            }

            case Ui::ParseIcdStateMachineType::Sequnce_st:
            {
                icd_msg.sHeader.nSequence = static_cast<uint8_t>(rxdata[ii]);
                MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Request_st;
            break;
            }
            case Ui::ParseIcdStateMachineType::Request_st:
        {

                icd_msg.sHeader.nReq = static_cast<uint8_t>(rxdata[ii]);
                MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::MessageType_st;
            break;
        }
            case Ui::ParseIcdStateMachineType::MessageType_st:
        {

                icd_msg.sHeader.nMsgtype = static_cast<uint8_t>(rxdata[ii]);
                MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::MessageLen_st;
            break;
        }
            case Ui::ParseIcdStateMachineType::MessageLen_st:
        {

                icd_msg.sHeader.nMsglen = static_cast<uint8_t>(rxdata[ii]);
                ParseIcdStateMachine = Ui::ParseIcdStateMachineType::MessageCollect_st;
            break;
        }
            case Ui::ParseIcdStateMachineType::MessageCollect_st:
        {

                if(i < icd_msg.sHeader.nMsglen)
                {
                    icd_msg.nMsgdata[i++] = static_cast<uint8_t>(rxdata[ii]);
                }
                else if(i > 255)
                {
                    i=0;
                    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_a_st;

                }
                else
                {
                    i=0;
                    ParseIcd(&icd_msg);
                    MainWindow::ParseIcdStateMachine = Ui::ParseIcdStateMachineType::Premble_a_st;
                }
            break;
        }

    }
}




}


void MainWindow::ParseIcd(icd::icd_template* pMsg)
{
    switch(pMsg->sHeader.nReq)
    {
            case(USB_REQ_MASTER_SEND):
        {

            switch(pMsg->sHeader.nMsgtype)
            {
                case(USB_MSG_TYPE_STATUS):
                    {
                        //cast the message to right template.
                        icd::icd_status_msg_type* msg = (icd::icd_status_msg_type*)(pMsg);

                        //update the screen with current temperture
                        ui->TempTextBox->setText(QString::number(((msg->nTemp)&0xff00)>>8));
                        ui->BatteryBar->setValue(msg->nBattPercent);

                        break;
                    }



            }

        break;
    }











    }



}

void MainWindow::on_TempTextBox_textChanged()
{

}

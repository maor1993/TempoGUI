#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include "usbdevicesform.h"
#include "recordingsform.h"
#include "tempostructs.h"
#include "icdworker.h"


using namespace icd;

namespace Ui {
    class MainWindow;

    }

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);



 //forms
        UsbDevicesForm* UsbDevicesDialog;
        Recordingsform* RecordingsDialog;

 //threads
        icdworker*      icdTxWorker;
        QThread*        icdTxWorkerThread;
        icdworker*      icdRxWorker;
        QThread*        icdRxWorkerThread;




        ~MainWindow();

    private slots:
        void on_ConnectPushButton_clicked();

        void on_DevConnectedCheckBox_stateChanged(int arg1);

        void on_RecordingPushButton_clicked();

        void on_UpdatePushButton_clicked();

        void IcdParse(icd::icd_template* pMsg);

        void IcdTx(icd::icd_template* pMsg);
    private:
        Ui::MainWindow *ui;
        QSerialPort *serial;

        void IcdGenerateEmptyRequest(uint8_t nMsgtype);
        static uint8_t     IcdTxSeq;


};




#endif // MAINWINDOW_H

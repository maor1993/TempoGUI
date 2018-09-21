#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include "usbdevicesform.h"
#include "tempostructs.h"


using namespace icd;

namespace Ui {
    class MainWindow;
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

    }

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        UsbDevicesForm* UsbDevicesDialog;
        ~MainWindow();

    private slots:
        void on_ConnectPushButton_clicked();

        void on_DevConnectedCheckBox_stateChanged(int arg1);
        void ReceiveUsbMsg();
        void on_TempTextBox_textChanged();

    private:
        Ui::MainWindow *ui;
        QSerialPort *serial;

        void ParseIcd(icd::icd_template* pMsg);
        static Ui::ParseIcdStateMachineType ParseIcdStateMachine;
};




#endif // MAINWINDOW_H

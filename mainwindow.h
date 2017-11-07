#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"serialinfodialog.h"
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QMessageBox>
#include<qcustomplot.h>

#include<QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void serialConnect(QSerialPortInfo obj, QAction *m_connect);
    void upDatePortMenu();

    void on_cancelButton_clicked();

private:
    Ui::MainWindow *ui;

    //menu
    bool m_firstCreation = true;
    QMenu *m_portMenu;
    void createPortMenu();
    void addPortsTo(QMenu *portMenu);
    void addToInfoTree(QSerialPortInfo obj);
    void setUpConnectionTo(QAction *info, QAction *m_connect, QSerialPortInfo obj);

    //connection and datagathering
    void readSerialData();
    void waitForSerial();
    bool m_cancel = false;
    void displaySerialData(int data);

    SerialInfoDialog *m_infoDialog;
    QSerialPort *m_serialPort;
    QSerialPortInfo m_currentInfoPort;
    QAction *m_currentAction;
    int m_baudRate = 115200;
    QByteArray m_serialData;

    //graphing
    QCustomPlot *m_plot;
    QVector<double> m_xData;
    QVector<double> m_yData;
    const int m_rangeX = 800;
    const uint m_rangeYdata = 255;
    const double m_rangeYplot = 5;
    int m_byteCount = 0;

    void createGraph();
    void addDataToGraph(qint64 nrOfBytes);
    double remapData(uint data, double inMin, double inMax, double outMin, double outMax);
};

#endif // MAINWINDOW_H

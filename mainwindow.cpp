#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createPortMenu();
    ui->textEdit->setReadOnly(true);//for displaying serial data

    createGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//-----------------------------------------------------------------------------------------
// Serial port menu w/info dialog setup
//-----------------------------------------------------------------------------------------
void MainWindow::createPortMenu()
{
    if(m_firstCreation)//for refreshing the menu
    {
    m_portMenu = new QMenu("Ports",this);
    ui->menuBar->addMenu(m_portMenu);

    connect(m_portMenu,SIGNAL(aboutToShow()),this,SLOT(upDatePortMenu()));//update menu when clicked

    m_firstCreation = false;
    }
    else
    {
        m_portMenu->clear();
    }

    addPortsTo(m_portMenu);
    m_serialPort = new QSerialPort(this);
}

void MainWindow::upDatePortMenu()
{
    if(!m_serialPort->isOpen())//dont update menu while connected to device
    {
        createPortMenu();
    }
}

void MainWindow::addPortsTo(QMenu *portMenu)//called by createPortMenu to add available ports to the menu
{
    bool portsAvailable = false;
    //iterate through every available port
    foreach (const QSerialPortInfo &obj, QSerialPortInfo::availablePorts())
    {
        //make a submenu for every port
        auto *subMenu = new QMenu(obj.portName() + ": " + obj.description(),portMenu);
        subMenu->setStatusTip(obj.description());
        portMenu->addMenu(subMenu);

        auto *infoAction = new QAction("More information",subMenu);
        infoAction->setStatusTip("More information about " + obj.description());
        subMenu->addAction(infoAction);

        auto *connectAction = new QAction("Connect",subMenu);
        connectAction->setStatusTip("Connect to device");
        subMenu->addAction(connectAction);

        //m_currentInfoPort = obj;

        addToInfoTree(obj);

        setUpConnectionTo(infoAction,connectAction, obj);
        portsAvailable = true;

    }
    if(!portsAvailable)
    {
        auto *emptyAction = new QAction(portMenu);
        emptyAction->setText("<No available ports>");
        portMenu->addAction(emptyAction);
    }

}

void MainWindow::addToInfoTree(QSerialPortInfo obj)//called by addPortsTo to place its info in dialog ui
{
    m_infoDialog = new SerialInfoDialog(this);
    m_infoDialog->setWindowTitle(obj.portName() + ": " + obj.description());
    m_infoDialog->addRootItem("Description",obj.description());
    m_infoDialog->addRootItem("Connection",obj.portName());
    m_infoDialog->addRootItem("Manufacturer",obj.manufacturer());
    m_infoDialog->addRootItem("Serial number",obj.serialNumber());
    m_infoDialog->addRootItem("System location",obj.systemLocation());

}

void MainWindow::setUpConnectionTo(QAction *info, QAction *m_connect, QSerialPortInfo obj)
{
    connect(info,SIGNAL(triggered(bool)),m_infoDialog,SLOT(displayInfoDialog()));
    //use c++11 syntax (functor) to pass obj on with connect
    connect(m_connect,&QAction::triggered,this,[=]{ serialConnect(obj,m_connect);});
}


//-----------------------------------------------------------------------------------------
// Serial port connection and datagathering
//-----------------------------------------------------------------------------------------


void MainWindow::serialConnect(QSerialPortInfo obj, QAction *m_connect)
{
    m_currentInfoPort = obj;
    //blir feil fordi menyen lages på nytt hver gang...TODO: ikke oppdater meny når connected
    m_currentAction = m_connect;//connection action associated with the serialport
    m_currentAction->setCheckable(true);
    m_currentAction->setIconText("test");

    m_serialPort = new QSerialPort(this);
    m_serialPort->setPort(m_currentInfoPort);

    //setup port according to arduino (should not be neccesary)
    m_serialPort->setBaudRate(m_baudRate);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);

    bool connected = m_serialPort->open(QIODevice::ReadOnly);
    if(!connected)
    {
        QMessageBox::warning(this,"Connection status","Could not connect");
    }
    else
    {
        ui->statusBar->showMessage("Connected to " + m_currentInfoPort.description());
        m_connect->setDisabled(true);
        m_connect->setChecked(true);

    }

    readSerialData();

}


void MainWindow::readSerialData()
{
    waitForSerial();
    m_serialPort->clear();

    for(int i = 0; i<m_rangeX;i++)
    {
        m_serialData[i] = 0;
    }

    if(!m_cancel)
    {
        ui->statusBar->showMessage("Reading from " + m_currentInfoPort.description());

        qint64 nrOfBytesRead = 0;
        do
        {
            m_serialData = m_serialPort->read(m_rangeX);
            //qDebug()<<"Nr of bytes read: "<<nrOfBytesRead;

            nrOfBytesRead = m_serialData.size();

            if(nrOfBytesRead>0)
            {
                addDataToGraph(nrOfBytesRead);
            }

            QApplication::processEvents();//check for buttonpush
            if(m_cancel)
            {
                break;
            }


        }while(nrOfBytesRead>0 || m_serialPort->waitForReadyRead(10000));//give 10 sec to wait for data or turn false

        ui->statusBar->showMessage("Reading ended");
        m_currentAction->setEnabled(true);
        m_currentAction->setChecked(false);
        m_serialPort->close();
    }
}

void MainWindow::waitForSerial()//waits for terminal until ready og canceled
{
    m_cancel = false;

    ui->statusBar->showMessage("Waiting for " + m_currentInfoPort.description());

    while(m_serialPort->bytesAvailable()<1)
    {
        QApplication::processEvents();//check for signals (buttonpush cancel)

        if(m_cancel)break;
    }
}



void MainWindow::displaySerialData(int data)
{
    ui->textEdit->setText(QString::number(data));
    ui->listWidget->addItem(QString::number(data));
}

void MainWindow::on_cancelButton_clicked()
{
    m_cancel = true;

    if(m_serialPort->isOpen())
    {
        ui->statusBar->showMessage("Canceled", 2000);
        m_currentAction->setEnabled(true);
        m_currentAction->setChecked(false);
        m_serialPort->close();
    }
}

//-----------------------------------------------------------------------------------------
// Graphing
//-----------------------------------------------------------------------------------------


void MainWindow::createGraph()
{
    m_plot = ui->widget;
    m_plot->addGraph();
    m_plot->xAxis->setLabel("Sample");
    m_plot->yAxis->setLabel("Volts");
    m_plot->xAxis->setRange(0,m_rangeX);
    m_plot->yAxis->setRange(0,m_rangeYplot+0.2);//0-5volts


    //data handeling
    m_xData.resize(m_rangeX);
    m_yData.resize(m_rangeX);

    for(int i = 0;i<m_rangeX;i++)
    {
        m_xData[i] = i;
    }


}

void MainWindow::addDataToGraph(qint64 nrOfBytes)
{
    quint8 intData = 0;
    double doubleData = 0;


    for(int i = 0; i<nrOfBytes;i++)
    {
        intData = static_cast<quint8>(m_serialData[i]);
        qDebug()<<"Int conversion: "<<intData;

        displaySerialData(intData);

        if( m_byteCount>=m_rangeX)//make ydata a circular buffer
        {
            m_byteCount=0;
        }

        if(intData>=0 && intData<=m_rangeYdata)//filter some noise
        {
            doubleData = remapData(intData,0,m_rangeYdata,0,m_rangeYplot);//0-255 -> 0-5
            m_yData[m_byteCount] = doubleData;

            m_byteCount++;
        }

    }

     m_plot->graph(0)->setData(m_xData,m_yData);
     m_plot->replot();
}

double MainWindow::remapData(uint data, double inMin, double inMax, double outMin, double outMax)
{
    double difference = (outMax- outMin)/(inMax-inMin);
    double newDataVal = ( (double)data - inMin )*difference + outMin;
    return newDataVal;
}





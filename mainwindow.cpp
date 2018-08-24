#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QTime>
#include <QApplication>
#include <QMouseEvent>


/*********************************************************************
*     function:Dynamic_Curve
*  Description:动态曲线的框选布局，暂停，历史查看
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);//设置曲线框的大小
    setupDemo(0);
}

/*********************************************************************
*     function:Set_Up_Demo
*  Description:曲线图的功能调用和标题设置
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::setupDemo(int demoIndex)
{

    setupRealtimeDataDemo(ui->customPlot);

    setWindowTitle("QCustomPlot: "+demoName);
    statusBar()->clearMessage();
    currentDemoIndex = demoIndex;
    ui->customPlot->replot();
}

/*********************************************************************
*     function:Set_Up_Real_Time_Data_Demo
*  Description:曲线图的功能实现
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    //初始化
    rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->customPlot);
/*******************************添加曲线*********************************/
    demoName = "Real Time Data Demo";
    //添加曲线并设置曲线颜色
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red));

/******************************设置坐标轴*********************************/
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");//设置时间格式
    customPlot->xAxis->setTicker(timeTicker);//将X轴设置为时间轴
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);//设置Y轴范围

/****************************信号与槽的链接********************************/
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0);//曲线开始运动

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(onPushButtonClicked()));
    connect(ui->startBtn, SIGNAL(clicked(bool)), this, SLOT(onStartBtnClicked()));

    customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables);//鼠标滚轮放大缩小
    customPlot->replot();

    //用信号和槽将QCustomPlot自带的鼠标事件与外部函数链接起来
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseRelease(QMouseEvent*)));
    connect(ui->captureBtn, SIGNAL(clicked()), this, SLOT(onCaptureBtnClicked()));

    QDesktopWidget *desk = QApplication::desktop();
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap p = screen->grabWindow(desk->winId());
}

/*********************************************************************
*     function:Real_Time_Data_Slot
*  Description:实时曲线的槽函数
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::realtimeDataSlot()
{
    //计算两个新数据点
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002)
    {
        ui->customPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
        ui->customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
        lastPointKey = key;
    }

    if(key < 5)
    {
        ui->customPlot->xAxis2->setRange(key, 8, Qt::AlignLeading);
        ui->customPlot->replot();
    }
    else
    {
//        ui->customPlot->xAxis2->disconnect();
        ui->customPlot->xAxis->setRange(key, 5, Qt::AlignRight);
        ui->customPlot->replot();
    }
//    static double lastFpsKey;
//    static int frameCount;
    //    ++frameCount;
}

//void MainWindow::initWindow()
//{
//    this->setMouseTracking(true);
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);//设置窗口状态
//}

/*********************************************************************
*     function:Mouse_Press_Event
*  Description:鼠标点击事件
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::mousePress(QMouseEvent *mevent)
{
    if(mevent->button() == Qt::RightButton)//设置点击鼠标右键
        {
            rubberOrigin = mevent->pos();
            rubberBand->setGeometry(QRect(rubberOrigin, QSize()));
            rubberBand->show();
        }
}


/*********************************************************************
*     function:Mouse_Move_Event
*  Description:鼠标移动事件
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::mouseMove(QMouseEvent *mevent)
{
    if(rubberBand->isVisible())
       rubberBand->setGeometry(QRect(rubberOrigin, mevent->pos()).normalized());
}


/*********************************************************************
*     function:Mouse_Release_Event
*  Description:鼠标释放自动布局
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::mouseRelease(QMouseEvent *mevent)
{
    Q_UNUSED(mevent);
        if (rubberBand->isVisible())
        {
            const QRect zoomRect = rubberBand->geometry();
            int xp1, yp1, xp2, yp2;
            zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);//自动检测X,Y轴长度
            double x1 = ui->customPlot->xAxis->pixelToCoord(xp1);
            double x2 = ui->customPlot->xAxis->pixelToCoord(xp2);
            double y1 = ui->customPlot->yAxis->pixelToCoord(yp1);
            double y2 = ui->customPlot->yAxis->pixelToCoord(yp2);

            ui->customPlot->xAxis->setRange(x1, x2);//将框选部分设置成窗口大小
            ui->customPlot->yAxis->setRange(y1, y2);

            rubberBand->hide();
            ui->customPlot->replot();
        }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*********************************************************************
*     function:Stop_Button
*  Description:暂停按键
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::onPushButtonClicked()
{
    dataTimer.stop();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

/*********************************************************************
*     function:Start_Button
*  Description:开始/继续按键
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::onStartBtnClicked()
{
    dataTimer.start();
    ui->customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables);
}


/*********************************************************************
*     function:Recovery_Button
*  Description:点击实现1:1复原
*        Input:
*       Output:
*       Return:
*       Others:
*       Auther:dongyuchuan
*  Create Time:2018.8.13
*---------------------------------------------------------------------
*  Modify
*  Version       Author        Date           Modification
*  V0.00         dongyuchuan   2018.8.13      实现函数功能
**********************************************************************/
void MainWindow::onCaptureBtnClicked()
{
    dataTimer.stop();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->rescaleAxes();//重调尺寸
    ui->customPlot->replot();
}

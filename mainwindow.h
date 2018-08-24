#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupDemo(int demoIndex);
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

private slots:
    void realtimeDataSlot();


    void onPushButtonClicked();
    void onStartBtnClicked();
    void onCaptureBtnClicked();

    void mousePress(QMouseEvent *mevent);
    void mouseMove(QMouseEvent* mevent);
    void mouseRelease(QMouseEvent *mevent);


private:
    Ui::MainWindow *ui;
    QString demoName;
    QTimer dataTimer;
    QCPItemTracer *itemDemoPhaseTracer;
    int currentDemoIndex;

    QRubberBand *rubberBand;
    QPoint rubberOrigin;
};

#endif // MAINWINDOW_H

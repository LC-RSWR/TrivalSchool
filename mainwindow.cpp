#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置图形界面
    setupUI();

    // 创建校园地图
    campusMap = new CampusMap(10);  // 假设有10个景点
    generateRandomLandmarks();
   


}

MainWindow::~MainWindow()
{
    delete ui;
}


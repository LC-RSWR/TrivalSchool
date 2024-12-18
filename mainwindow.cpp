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
    generateRandomPaths();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void LandmarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

	// 设置画笔和画刷
	QPen pen(Qt::black);
	QBrush brush(Qt::blue);
	painter->setPen(pen);
	painter->setBrush(brush);

	// 绘制圆形表示景点
	painter->drawEllipse(boundingRect());

	// 绘制景点名称，设置为鲜艳的橙色
	QFont font = painter->font();
	font.setPointSize(10);
	painter->setFont(font);

	// 设置字体颜色为绿色
	QPen textPen(Qt::green);  // 设置文字颜色为绿色
	painter->setPen(textPen);

	// 绘制景点名称
	painter->drawText(-15, 25, m_landmark.name);

	// 绘制位置坐标 (x, y)
	QFont coordFont = painter->font();
	coordFont.setPointSize(6);
	painter->setFont(coordFont);

	// 设置坐标文本颜色为黑色（默认）
	painter->setPen(Qt::black);

	QString coordinates = QString("(%1, %2)").arg(m_landmark.position.x()).arg(m_landmark.position.y());
	painter->drawText(-15, 35, coordinates);  // 绘制坐标文本

	// 如果鼠标悬停在景点上，显示介绍信息
	 // 如果鼠标悬停，显示介绍文本
	if (m_hovered) {
		

		painter->setPen(Qt::red);  // 设置介绍文本颜色
		painter->drawText(-15, 35, m_landmark.intro);  // 绘制介绍文本
	}
	

	qDebug() << "Hover status:" << m_hovered;
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include "campusmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QGraphicsItem>
#include <QPainter>
#include <QFont>

#include <QGraphicsSceneMouseEvent>

class LandmarkItem : public QGraphicsItem {


public:
	LandmarkItem(const Landmark& landmark)
		: m_landmark(landmark), m_hovered(false) {
		setPos(landmark.position);  // 设置项的位置
		setFlag(ItemIsSelectable);  // 可以选择
		setAcceptHoverEvents(true);  // 启用鼠标悬停事件
	}

	QRectF boundingRect() const override {
		return QRectF(-15, -15, 30, 30);  // 圆形的边界框
	}

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
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
		if (m_hovered) {
			painter->setPen(Qt::black);
			painter->setBrush(Qt::white);
			QRectF rect(m_landmark.position.x(), m_landmark.position.y(), 500, 100);  // 介绍框的位置和大小
			painter->drawText(-35, 35, coordinates);  // 绘制坐标文本
		}
	}

protected:
	// 鼠标进入景点时显示介绍
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
		m_hovered = true;
		update();  // 更新视图，重新绘制以显示介绍信息
	}

	// 鼠标离开景点时隐藏介绍
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
		m_hovered = false;
		update();  // 更新视图，重新绘制以隐藏介绍信息
	}

private:
	Landmark m_landmark;  // 景点数据
	bool m_hovered;  // 标记鼠标是否悬停在景点上
};


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	CampusMap* campusMap;
	QGraphicsView* view;
	QGraphicsScene* scene;
	QLabel* infoLabel;
	Dijkstra dijkstra;
	QLineEdit* sourceLineEdit; // 输入框

	QVector<QPair<int, int>> paths;  // 存储路径的索引对
	QMap<QPair<int, int>, int> pathLengths;  // 存储路径长度

	void setupUI() {
		QWidget* widget = new QWidget(this);
		QVBoxLayout* layout = new QVBoxLayout(widget);

		view = new QGraphicsView(this);
		scene = new QGraphicsScene(this);
		view->setScene(scene);

		infoLabel = new QLabel(QStringLiteral("请选择景点"), this);
		layout->addWidget(view);
		sourceLineEdit = new QLineEdit(this); // 输入框
		layout->addWidget(infoLabel);
		layout->addWidget(sourceLineEdit);


		QPushButton* searchButton = new QPushButton(QStringLiteral("查询最短路径"), this);
		layout->addWidget(searchButton);

		connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchPath);

		setCentralWidget(widget);
		setWindowTitle(QStringLiteral("校园导游系统"));

	}

	void mouseMoveEvent(QMouseEvent* event) override {

	}


	void generateRandomLandmarks() {
		// 清空之前的路径
		paths.clear();
		pathLengths.clear();
		scene->clear();  // 先清空场景

		// 定义一些景点特色描述模板
		QStringList types = {
	 QString::fromLocal8Bit("古老的"),
	 QString::fromLocal8Bit("现代的"),
	 QString::fromLocal8Bit("自然风光的"),
	 QString::fromLocal8Bit("文化艺术的"),
	 QString::fromLocal8Bit("学术研究的")
		};

		QStringList features = {
			QString::fromLocal8Bit("建筑风格独特"),
			QString::fromLocal8Bit("绿树成荫"),
			QString::fromLocal8Bit("周围环境宁静"),
			QString::fromLocal8Bit("历史悠久"),
			QString::fromLocal8Bit("文化底蕴深厚"),
			QString::fromLocal8Bit("是学生们聚集的地方")
		};


		// 生成10个随机景点
		for (int i = 0; i < 10; ++i) {
			QString name = QString::fromLocal8Bit("景点") + QString::number(i);
			QString code = QString::fromLocal8Bit("Code") + QString::number(i);

			// 随机选择一些景点描述
			QString type = types[qrand() % types.size()];
			QString feature = features[qrand() % features.size()];

			// 创建丰富的景点简介
			QString intro = QString::fromLocal8Bit("这个景点是学校内最") + type + QString::fromLocal8Bit("，以其") + feature + QString::fromLocal8Bit("而闻名，是学生们喜爱的休闲和学习场所。");

			// 随机位置
			QPointF position(rand() % 800, rand() % 600);

			// 添加景点到 campusMap
			campusMap->addLandmark(i, name, code, intro, position);

			// 创建景点项并添加到场景中
			LandmarkItem* landmarkItem = new LandmarkItem(campusMap->landmarks[i]);
			scene->addItem(landmarkItem);

			// 连接信号与槽
			//connect(landmarkItem, &LandmarkItem::showLandmarkIntro, this, &MainWindow::showLandmarkIntro);
			//connect(landmarkItem, &LandmarkItem::hideLandmarkIntro, this, &MainWindow::hideLandmarkIntro);
		}
	}


	void generateRandomPaths() {
		// 随机生成路径
		for (int i = 0; i < 10; ++i) {
			for (int j = i + 1; j < 10; ++j) {
				if (rand() % 2 == 0) {
					int length = rand() % 50 + 10;  // 路径长度在10到60之间
					campusMap->addPath(i, j, length);
					addPath(i, j, length);
				}
			}
		}
	}

	void addPath(int startIndex, int endIndex, int length) {
		// 添加路径索引对
		paths.append(qMakePair(startIndex, endIndex));
		pathLengths[qMakePair(startIndex, endIndex)] = length;

		// 获取景点位置
		QPointF startPos = campusMap->landmarks[startIndex].position;
		QPointF endPos = campusMap->landmarks[endIndex].position;

		// 绘制直线连接两个景点
		QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(startPos, endPos));
		scene->addItem(line);

		// 显示路径长度
		QGraphicsTextItem* lengthText = new QGraphicsTextItem(QString::number(length));
		QPointF middle = (startPos + endPos) / 2;
		lengthText->setPos(middle.x() - 10, middle.y() - 10);
		scene->addItem(lengthText);
	}
	void onSearchPath() {

		QString start = sourceLineEdit->text();  // 获取源景点名称
		if (start.isEmpty()) {
			return;
		}


		// 查询最短路径
		QVector<int> dist = dijkstra.findShortestPath(*campusMap, start.toInt());
		QString result = QString::fromLocal8Bit("从景点0到其他景点的最短距离：\n");
		for (int i = 0; i < campusMap->landmarks.size(); ++i) {
			result += QString::fromLocal8Bit("景点") + QString::number(i) + ": " + QString::number(dist[i]) + "\n";
		}
		infoLabel->setText(result);
	}


public slots:
	void showLandmarkIntro(const QString& name, const QString& intro, const QPointF& position) {
		// 创建一个文本项来显示景点的介绍
		if (!introTextItem) {
			introTextItem = new QGraphicsTextItem();
			scene->addItem(introTextItem);
		}
		introTextItem->setPlainText(name + "\n" + intro);
		introTextItem->setPos(position.x() + 20, position.y());  // 在景点附近显示简介
		introTextItem->setDefaultTextColor(Qt::black);  // 设置文字颜色
	}

	void hideLandmarkIntro() {
		if (introTextItem) {
			introTextItem->setPlainText("");  // 清空文本
		}
	}
private:
	Ui::MainWindow* ui;
	QGraphicsTextItem* introTextItem = nullptr;  // 用于显示景点简介的文本项
};
#endif // MAINWINDOW_H

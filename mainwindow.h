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
#include "campusmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QGraphicsItem>
#include <QPainter>
#include <QFont>

class LandmarkItem : public QGraphicsItem {
public:
    LandmarkItem(const Landmark& landmark)
        : m_landmark(landmark) {
        setPos(landmark.position);  // 设置项的位置
        setFlag(ItemIsSelectable);  // 可以选择
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

        // 设置字体颜色为橙色
        QPen textPen(Qt::green);  // 设置文字颜色为橙色
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
    }


private:
    Landmark m_landmark;  // 景点数据
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
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


    void generateRandomLandmarks() {

        // 清空之前的路径
        paths.clear();
        pathLengths.clear();
        scene->clear();  // 先清空场景
        // 生成10个随机景点
        for (int i = 0; i < 10; ++i) {
            QString name = QString::fromLocal8Bit("景点") + QString::number(i);
            QString code = QString::fromLocal8Bit("Code") + QString::number(i);
            QString intro = QString::fromLocal8Bit("景点") + QString::number(i) + QString::fromLocal8Bit("简介");
            QPointF position(rand() % 800, rand() % 600);

            // 添加景点到 campusMap
            campusMap->addLandmark(i, name, code, intro, position);

            // 创建景点项并添加到场景中
            LandmarkItem* landmarkItem = new LandmarkItem(campusMap->landmarks[i]);
            scene->addItem(landmarkItem);
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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

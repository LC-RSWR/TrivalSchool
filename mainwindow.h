#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
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
        setAcceptHoverEvents(true); // 启用鼠标悬停事件
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

    // 鼠标进入事件
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        // 在鼠标进入时显示浮动提示框
        QString tooltipText = QStringLiteral("景点介绍\n") + m_landmark.intro;
        QToolTip::showText(event->screenPos(), tooltipText);  // 不阻塞的提示框
        update();  // 重绘，以显示介绍文本
    }

    // 鼠标离开事件
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        // 鼠标离开时隐藏提示框
        QToolTip::hideText();
        update();  // 重绘，隐藏介绍文本
    }

    // 鼠标移动事件（如果需要）
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override {
        // 可以根据鼠标位置动态改变介绍文本内容或样式
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

        connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchPathComplex);

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
            // 随机生成景点名称
            QString name = QString::fromLocal8Bit("学术大楼") + QString::number(i);

            // 随机生成景点编码
            QString code = QString::fromLocal8Bit("Code") + QString::number(i);

            // 生成不同的简介内容
            QString intro;
            switch (i) {
            case 0: intro = QString::fromLocal8Bit("学术大楼是本校最具历史意义的建筑之一，是许多重要学术讲座和会议的举办地。"); break;
            case 1: intro = QString::fromLocal8Bit("图书馆是学校的知识宝库，馆内藏书丰富，提供舒适的学习环境。"); break;
            case 2: intro = QString::fromLocal8Bit("学生中心提供丰富的校园活动和娱乐设施，是学生社交的重要场所。"); break;
            case 3: intro = QString::fromLocal8Bit("体育馆为学生提供多种运动设施，是校内体育活动的核心场地。"); break;
            case 4: intro = QString::fromLocal8Bit("湖畔公园是一个美丽的绿地，常常是学生放松和散步的理想之地。"); break;
            case 5: intro = QString::fromLocal8Bit("科学实验室是各类科技创新和实验活动的核心地带，设施先进，设备齐全。"); break;
            case 6: intro = QString::fromLocal8Bit("艺术中心是学校内最具艺术氛围的地方，常年举办音乐会和艺术展览。"); break;
            case 7: intro = QString::fromLocal8Bit("主教学楼是学校的教学核心，拥有现代化的教室和先进的多媒体设施。"); break;
            case 8: intro = QString::fromLocal8Bit("文化广场是学校的社交和文化交流中心，常常举办各种集体活动。"); break;
            case 9: intro = QString::fromLocal8Bit("校史馆展示了学校的悠久历史和发展进程，是了解学校文化的好去处。"); break;
            default: intro = QString::fromLocal8Bit("这是一个典型的校园景点，具有丰富的历史和文化内涵。"); break;
            }

            // 随机生成位置
            QPointF position(rand() % 800, rand() % 600);

            // 将景点添加到校园地图
            campusMap->addLandmark(i, name, code, intro, position);

            // 创建景点项并添加到场景中
            LandmarkItem* landmarkItem = new LandmarkItem(campusMap->landmarks[i]);
            scene->addItem(landmarkItem);
        }

        generateRandomPaths();

    }


    void generateRandomPaths() {
        // 随机生成路径
        for (int i = 0; i < 10; ++i) {
            for (int j = i + 1; j < 10; ++j) {
                if (rand() % 2 == 0) {
                    // 计算景点之间的绝对距离（欧几里得距离）
                    int length = static_cast<int>(campusMap->calculateDistance(i, j));
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

        // 将起点转换为整数
        int startIdx = start.toInt();

        // 查询最短路径和路径长度
        QVector<QPair<QVector<int>, int>> pathsWithLength = dijkstra.findShortestPathsWithLength(*campusMap, startIdx);

        QString result = QString::fromLocal8Bit("从景点") + start + QString::fromLocal8Bit("到其他景点的最短路径及其长度：\n");

        // 遍历所有景点
        for (int i = 0; i < campusMap->landmarks.size(); ++i) {
            const QVector<int>& path = pathsWithLength[i].first;
            int totalLength = pathsWithLength[i].second;

            if (path.isEmpty()) {
                result += QString::fromLocal8Bit("景点") + QString::number(i) + QString::fromLocal8Bit(": 无法到达\n");
            }
            else {
                result += QString::fromLocal8Bit("景点") + QString::number(i) + QString::fromLocal8Bit(": 路径 -> ");
                for (int j = 0; j < path.size(); ++j) {
                    result += QString::number(path[j]);
                    if (j < path.size() - 1) {
                        result += " -> ";
                    }
                }
                result += QString::fromLocal8Bit("， 路径长度: ") + QString::number(totalLength) + "\n";
            }
        }

        // 更新显示的结果
        infoLabel->setText(result);
    }


    void onSearchPathComplex() {
        QStringList landmarksList = sourceLineEdit->text().split(" ");  // 获取多个景点的输入，按空格分隔
        if (landmarksList.isEmpty()) {
            return;
        }

        if (landmarksList.size() == 1)
        {
            onSearchPath();
            return;
        }


        // 获取用户输入的目标景点列表（多个目标）
        QStringList targetsStr = landmarksList;
        QVector<int> targets;
        for (const QString& target : targetsStr) {
            targets.append(target.toInt());
        }

     
        // 获取距离矩阵
        QVector<QVector<int>> dist = campusMap->getDistanceMatrix();
        printf("距离矩阵：\n");
        for (const auto& row : dist) {
            for (int val : row) {
                printf("%d ", val);
            }
            printf("\n");
        }

        // 使用距离矩阵进行 TSP 计算
        QVector<int> path;
        int shortestPath = campusMap->calculateTSPUsingMatrix(targets, path);

        // 输出最短路径和路径长度
        printf("最短路径长度: %d\n", shortestPath);
        printf("路径顺序: ");
        for (int idx : path) {
            printf("%d ", idx);
        }

   
        // 输出计算结果
        QString result = QString::fromLocal8Bit("从景点") + QString::number(targets[0]) + QString::fromLocal8Bit("到各目标景点的最短路径：\n");

        // 输出经过的景点和路径
        QString pathString = QString::fromLocal8Bit("路径： ");
        for (int node : path) {
            pathString += QString::number(node) + " ";
        }

        // 添加路径长度
        result += pathString + "\n";
        result += QString::fromLocal8Bit("最短路径长度: ") + QString::number(shortestPath);

        // 显示最终结果
        infoLabel->setText(result);
    }



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

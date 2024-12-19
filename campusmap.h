#ifndef CAMPUSMAP_H
#define CAMPUSMAP_H

#include <qstring>
#include <QVector>
#include <QMap>
#include <QPointF>

struct Landmark {
    QString name;     // 景点名称
    QString code;     // 景点代码
    QString intro;    // 景点简介
    QPointF position; // 景点位置
};

class CampusMap {
public:
    QVector<Landmark> landmarks;  // 存储景点信息
    QVector<QVector<int>> adjacencyMatrix;  // 存储路径矩阵（邻接矩阵）

    // 构造函数
    CampusMap(int numLandmarks) {
        landmarks.resize(numLandmarks);
        adjacencyMatrix.resize(numLandmarks);  // 仅调整外部向量大小

        // 初始化 adjacencyMatrix 内部每个 QVector<int>
        for (int i = 0; i < numLandmarks; ++i) {
          
            adjacencyMatrix[i].resize(numLandmarks); // 每个子 QVector 初始化为 -1
            for (size_t j = 0; j < numLandmarks; j++)
            {
                adjacencyMatrix[i][j] = -1;
            }
           
        }
    }

    void addLandmark(int index, const QString& name, const QString& code, const QString& intro, const QPointF& position) {
        landmarks[index] = { name, code, intro, position };
    }

    void addPath(int from, int to, int length) {
        adjacencyMatrix[from][to] = length;
        adjacencyMatrix[to][from] = length;  // 假设是无向图
    }
};


#include <QVector>
#include <QQueue>
#include <limits.h>

class Dijkstra {
public:
    // 查询从start到所有节点的最短路径
    QVector<int> findShortestPath(CampusMap& campus, int start) {
        int n = campus.landmarks.size();
        QVector<int> dist(n, INT_MAX);  // 存储最短距离
        QVector<int> prev(n, -1);  // 存储前驱节点
        QVector<bool> visited(n, false);  // 标记节点是否已访问

        dist[start] = 0;

        // Dijkstra算法核心部分
        for (int i = 0; i < n; ++i) {
            int u = -1;
            // 找到未访问的最短距离的节点
            for (int j = 0; j < n; ++j) {
                if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                    u = j;
                }
            }

            if (dist[u] == INT_MAX) break;  // 如果所有可达点已访问，退出循环

            visited[u] = true;

            // 遍历邻接节点，进行松弛操作
            for (int v = 0; v < n; ++v) {
                if (campus.adjacencyMatrix[u][v] != -1 && dist[u] + campus.adjacencyMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + campus.adjacencyMatrix[u][v];
                    prev[v] = u;  // 更新前驱节点
                }
            }
        }

        return dist;
    }

    // 假设这是Dijkstra算法中的一部分
    QVector<int> findShortestPathWithPrev(CampusMap& campusMap, int start, QVector<int>& prev) {
        int n = campusMap.landmarks.size();
        QVector<int> dist(n, INT_MAX);
        dist[start] = 0;
        prev.fill(-1, n);  // 初始化前驱数组

        QQueue<int> queue;
        queue.enqueue(start);

        while (!queue.isEmpty()) {
            int u = queue.dequeue();
            for (int v : campusMap.adjacencyMatrix[u]) {
                if (dist[u] + campusMap.adjacencyMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + campusMap.adjacencyMatrix[u][v];
                    prev[v] = u;  // 记录前驱节点
                    queue.enqueue(v);
                }
            }
        }
        return dist;
    }

private:
    QVector<int> prev;  // 存储每个节点的前驱节点
};



#endif // CAMPUSMAP_H

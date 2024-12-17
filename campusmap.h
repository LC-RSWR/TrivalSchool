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
    QVector<int> findShortestPath(CampusMap& campus, int start) {
        int n = campus.landmarks.size();
        QVector<int> dist(n, INT_MAX);
        QVector<int> prev(n, -1);
        QVector<bool> visited(n, false);

        dist[start] = 0;

        for (int i = 0; i < n; ++i) {
            int u = -1;
            for (int j = 0; j < n; ++j) {
                if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                    u = j;
                }
            }

            if (dist[u] == INT_MAX) break; // 所有可达点已访问

            visited[u] = true;

            for (int v = 0; v < n; ++v) {
                if (campus.adjacencyMatrix[u][v] != -1 && dist[u] + campus.adjacencyMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + campus.adjacencyMatrix[u][v];
                    prev[v] = u;
                }
            }
        }

        return dist;
    }

    QVector<int> getPath(int start, int end, const QVector<int>& prev) {
        QVector<int> path;
        for (int v = end; v != -1; v = prev[v]) {
            path.prepend(v);
        }
        return path;
    }
};

#endif // CAMPUSMAP_H

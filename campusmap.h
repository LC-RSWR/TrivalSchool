#ifndef CAMPUSMAP_H
#define CAMPUSMAP_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QPointF>
#include <QQueue>
#include <limits.h>
#include <algorithm>
#include <QStringList>
#include <QString>
#include <QPointF>
#include <QVector>
#include <cmath>
#include <limits>

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
            for (size_t j = 0; j < numLandmarks; j++) {
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

    // 计算两景点之间的欧几里得距离
    float calculateDistance(int from, int to) const {
        const QPointF& p1 = landmarks[from].position;
        const QPointF& p2 = landmarks[to].position;
        return std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2));
    }

    // 获取所有景点之间的距离矩阵
    QVector<QVector<int>> getDistanceMatrix() {
        int n = landmarks.size();
        QVector<QVector<int>> dist(n, QVector<int>(n, 0));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    dist[i][j] = static_cast<int>(calculateDistance(i, j));
                }
            }
        }

        return dist;
    }

    // 计算最短路径的 TSP（贪心算法）
    int calculateTSP(const QVector<int>& targets, QVector<int>& path) {
        int n = targets.size();
        QVector<bool> visited(n, false);
        int totalLength = 0;
        path.clear();

        // 假设起点为第一个目标景点
        int current = targets[0];
        visited[0] = true;
        path.append(current);

        for (int i = 1; i < n; ++i) {
            // 贪心策略：选择下一个距离最近的目标
            int next = -1;
            int minDist = std::numeric_limits<int>::max();

            for (int j = 0; j < n; ++j) {
                if (!visited[j]) {
                    int dist = adjacencyMatrix[current][targets[j]];
                    if (dist != -1 && dist < minDist) {
                        minDist = dist;
                        next = j;
                    }
                }
            }

            if (next == -1) break; // 如果没有找到可达的下一个景点（比如有不可达的情况）

            visited[next] = true;
            path.append(targets[next]);
            totalLength += minDist;
            current = targets[next];
        }

        // 最后，返回到起点
        totalLength += adjacencyMatrix[current][targets[0]];
        return totalLength;
    }

    // 使用 getDistanceMatrix 进行 TSP 计算
    int calculateTSPUsingMatrix(const QVector<int>& targets, QVector<int>& path) {
        // 获取距离矩阵
        QVector<QVector<int>> distMatrix = getDistanceMatrix();

        int n = targets.size();
        QVector<bool> visited(n, false);
        int totalLength = 0;
        path.clear();

        // 假设起点为第一个目标景点
        int current = targets[0];
        visited[0] = true;
        path.append(current);

        for (int i = 1; i < n; ++i) {
            // 贪心策略：选择下一个距离最近的目标
            int next = -1;
            int minDist = std::numeric_limits<int>::max();

            for (int j = 0; j < n; ++j) {
                if (!visited[j]) {
                    int dist = distMatrix[current][targets[j]];
                    if (dist < minDist) {
                        minDist = dist;
                        next = j;
                    }
                }
            }

            if (next == -1) break; // 如果没有找到可达的下一个景点（比如有不可达的情况）

            visited[next] = true;
            path.append(targets[next]);
            totalLength += minDist;
            current = targets[next];
        }

        // 最后，返回到起点
        totalLength += distMatrix[current][targets[0]];
        return totalLength;
    }
};

class Dijkstra {
public:
    // 查询从start到所有节点的最短路径
    QVector<QPair<QVector<int>, int>> findShortestPathsWithLength(CampusMap& campus, int start) {
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

        // 通过prev数组重建从start到每个节点的路径
        QVector<QPair<QVector<int>, int>> pathsWithLength(n);

        for (int i = 0; i < n; ++i) {
            if (dist[i] != INT_MAX) {
                QVector<int> path;
                int totalLength = 0;
                for (int node = i; node != -1; node = prev[node]) {
                    path.prepend(node);  // 从目标节点回溯到起点
                }

                // 计算路径长度
                for (int j = 0; j < path.size() - 1; ++j) {
                    totalLength += campus.adjacencyMatrix[path[j]][path[j + 1]];
                }

                // 保存路径和路径长度
                pathsWithLength[i] = qMakePair(path, totalLength);
            }
            else {
                pathsWithLength[i] = qMakePair(QVector<int>(), -1);  // 不可达的路径，长度为 -1
            }
        }

        return pathsWithLength;
    }
};

#endif // CAMPUSMAP_H

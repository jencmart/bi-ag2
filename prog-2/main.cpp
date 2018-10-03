#include <iostream>
#include <cstring>
#include <climits>
#include <vector>
#include <queue>
#include <map>
#include <set>

using namespace std;

struct Edge {
    Edge(int id, int capacity, bool backedge) : id(id), capacity(capacity), backedge(backedge) {}

    int id;
    int capacity;
    bool backedge;
};

struct EdgeForBacktrack {
    EdgeForBacktrack(int id, Edge &edge) : id(id), edge(edge) {}

    int id;
    Edge &edge;
};

typedef vector<vector<Edge>> AGraph;

void ScanGraph(AGraph &graph, vector<int> &trainPos, vector<int> &powerPos, int nodesCnt, int edgesCnt, int trainsCnt);

void ClearCapacity(AGraph &graph);

void AddST(AGraph &graph, const vector<int> &trainPos, const vector<int> &powerPos, int nodesCnt, int timeCycle);

void RemoveST(AGraph &graph);

void EnlargeGraph(AGraph &graph, AGraph &graphPart, int timeCycle);

int GetPathLengthRecursively(AGraph &graph, int trainPos);

void PrintTrainTimeTable(AGraph &graph, int nodesCnt, int trainPos, int finalTime);

bool Bfs(AGraph &graph, map<int, EdgeForBacktrack> &parentMap) {
    int s = (int) graph.size() - 2; // start point
    int t = (int) graph.size() - 1; // end point

    queue<int> queueToExpand;
    set<int> visited;

    queueToExpand.push(s);
    visited.insert(s);

    /// while queue not empty
    while (!queueToExpand.empty()) {

        /// get first node
        int current = queueToExpand.front();
        queueToExpand.pop();

        for (Edge &adjecent : graph[current]) {

            /// if not visited and we have capacity
            if (visited.find(adjecent.id) == visited.end() && adjecent.capacity > 0) {
                queueToExpand.push(adjecent.id);
                parentMap.insert(make_pair(adjecent.id, EdgeForBacktrack(current,
                                                                         adjecent)));  /// chtel bych abych  v parentMap[v] mel : referenci na graph[parent] -> edge do V
                visited.insert(adjecent.id);
                if (adjecent.id == t)
                    return true;
            }
        }
    }

    return false;
}

int FindMaxFlowOnPath(AGraph &graph, map<int, EdgeForBacktrack> &parentMap) {
    int s = (int) graph.size() - 2; // start point
    int t = (int) graph.size() - 1; // end point
    int flow = INT_MAX;

    /// find max flow for this path
    int v = t;
    while (v != s) {
        flow = min(flow, parentMap.find(
                v)->second.edge.capacity);  ///  parentMap[v] mel referenci na graph[parent] -> edge do V
        v = parentMap.find(v)->second.id;
    }

    return flow;
}

void UpdateFlow(AGraph &graph, map<int, EdgeForBacktrack> &parentMap, int newFlow) {
    int s = (int) graph.size() - 2; // start point
    int t = (int) graph.size() - 1; // end point

    int v = t;
    while (v != s) {
        /// remove that flow which is flowing through here
        parentMap.find(v)->second.edge.capacity -= newFlow;

        /// but add it to the oposite direction
        for (int i = 0; i < (int) graph[v].size(); ++i) {
            if (graph[v][i].id == parentMap.find(v)->second.id &&
                graph[v][i].backedge != parentMap.find(v)->second.edge.backedge) {
                graph[v][i].capacity += newFlow;
                break;
            }
        }

        v = parentMap.find(v)->second.id;
    }
}

int EdmondKarp(AGraph &graph) {
    int maxFlow = 0;

    /// While path from S to T exists
    while (true) {
        map<int, EdgeForBacktrack> parentMap; //V : ma paretn U pres {normalEdge, backEdge}

        if (!Bfs(graph, parentMap))
            break;

        /// Calculate max possible flow on this path
        int newFlow = FindMaxFlowOnPath(graph, parentMap);

        /// Update flows in graph
        UpdateFlow(graph, parentMap, newFlow);

        maxFlow += newFlow;
    }

    return maxFlow;
}

void PrintFinish(AGraph &graph, vector<int> &trainPos, int nodesCnt) {
    /// determine and print real time
    int finalTime = 0;

    for (int trainPo : trainPos)
        finalTime = max(finalTime, GetPathLengthRecursively(graph, trainPo * 2));

    printf("%d\n", finalTime);

    /// print positions of all trains
    for (int trainPo : trainPos)
        PrintTrainTimeTable(graph, nodesCnt, trainPo * 2, finalTime);

}

int main() {

    /// Scan graph
    int nodesCnt, edgesCnt, trainsCnt;
    scanf("%d%d%d", &nodesCnt, &edgesCnt, &trainsCnt);
    vector<int> trainPos, powerPos;
    AGraph graphForTest((unsigned) nodesCnt * 2);

    /// Special cases
    if (trainsCnt == 0) {
        printf("-1\n");
        return 0;
    } else if (nodesCnt == 1) {
        printf("0\n0\n");
        return 0;
    }


    ///SCAN
    for (int i = 0; i < trainsCnt; ++i) {
        int x;
        scanf("%d", &x);
        trainPos.emplace_back(x);
    }

    for (int i = 0; i < trainsCnt; ++i) {
        int x;
        scanf("%d", &x);
        powerPos.emplace_back(x);
    }


    AGraph graphWithScannedConnections = graphForTest;

    /// Add edges between fake and real nodes
    for (int i = 0; i < nodesCnt * 2 - 1; i += 2) {
        graphForTest[i].emplace_back(Edge(i + 1, trainsCnt, false));
        graphForTest[i + 1].emplace_back(Edge(i, 0, true));
    }



    /// Scan Graph - allready create with duplicate edges
    for (int i = 0; i < edgesCnt; ++i) {
        int from, to;
        scanf("%d%d", &from, &to);

        graphForTest[from * 2 + 1].emplace_back(Edge(to * 2, trainsCnt, false)); // z fake do real
        graphForTest[to * 2].emplace_back(Edge(from * 2 + 1, 0, true)); // backedge z real do fake

        graphWithScannedConnections[from * 2 + 1].emplace_back(Edge(to * 2, 1, false)); // z fake do real
/// todo uz nema ani backedge        graphWithScannedConnections[to * 2].emplace_back(Edge(from * 2 + 1, 0, true)); // backedge z real do fake
    }

    /// ****** RUN SOLUTION EXISTS TEST
    AddST(graphForTest, trainPos, powerPos, nodesCnt, 0);
    if (EdmondKarp(graphForTest) != trainsCnt) {
        printf("-1\n");
        return 0;
    }
    /// ************************************


    /// CREATE GRAPH - now it will only have conections between duplicate nodes
    AGraph graph((unsigned) nodesCnt * 2);

    /// Add edges between fake and real nodes
    for (int i = 0; i < nodesCnt * 2 - 1; i += 2) {
        graph[i].emplace_back(Edge(i + 1, trainsCnt, false));
        graph[i + 1].emplace_back(Edge(i, 0, true));
    }


    /// Add Source and Destintion nodes
    AddST(graph, trainPos, powerPos, nodesCnt, 0);

    /// for all possible times
    for (int i = 0; i < nodesCnt; ++i) {

        /// reset flow capacity
        ClearCapacity(graph);

        /// enlarge graph
        if (i != 0) {
            RemoveST(graph);
            EnlargeGraph(graph, graphWithScannedConnections,
                         i - 1);  // graphWithScannedConnections  - nema connectiony mezi duplikovanymi uzly
            AddST(graph, trainPos, powerPos, nodesCnt, i);
        }

        /// run edmondKarp
        int maxFlow = EdmondKarp(graph);

        /// if current time dealy is enough
        if (maxFlow == trainsCnt) {
            PrintFinish(graph, trainPos, nodesCnt);
            return 0;
        }
    }

    printf("Huston we have problem\n");

    return 0;
}

void PrintTrainTimeTable(AGraph &graph, int nodesCnt, int trainPos, int finalTime) {

    for (int i = 0; i < (int) graph[trainPos].size(); ++i) {

        if (graph[trainPos][i].capacity == 0 && !graph[trainPos][i].backedge) {
            if (graph[trainPos][i].id == (int) graph.size() - 1) // we dont print T
            {
                printf("%d", ((trainPos / 2) % nodesCnt));
                while (finalTime-- > 0)
                    printf(" %d", ((trainPos / 2) % nodesCnt));
                printf("\n");
                return;
            } else if (trainPos + 1 == graph[trainPos][i].id &&
                       trainPos % 2 == 0)  // if the next edge is not real, we dont print this, cause next ll be printed
                return PrintTrainTimeTable(graph, nodesCnt, graph[trainPos][i].id, finalTime);

            else {
                printf("%d ", ((trainPos / 2) % nodesCnt));
                return PrintTrainTimeTable(graph, nodesCnt, graph[trainPos][i].id, --finalTime);
            }
        }
    }
}

int GetPathLengthRecursively(AGraph &graph, int trainPos) {

    for (int i = 0; i < (int) graph[trainPos].size(); ++i) {

        if (graph[trainPos][i].capacity == 0 && !graph[trainPos][i].backedge) {
            if (graph[trainPos][i].id == (int) graph.size() - 1) // we dont coount path to T
                return 0;

            else if (trainPos + 1 == graph[trainPos][i].id &&
                     trainPos % 2 == 0)  // if the next edge is not real dont count path to it
                return GetPathLengthRecursively(graph, graph[trainPos][i].id);

            else
                return 1 + GetPathLengthRecursively(graph, graph[trainPos][i].id); // else count rest path + 1
        }
    }

    exit(1); // some kind of error ; this is not possible
}

void EnlargeGraph(AGraph &graph, AGraph &graphPart, int timeCycle) { // it is old timeCycle before enlargement
    auto oldSize = (int) graph.size();                              // // graphPart  - nema connectiony mezi duplikovanymi uzly


    /// create new part of the graph
    for (int i = 0; i < (int) graphPart.size(); ++i)
        graph.emplace_back(vector<Edge>());


    /// create new part of the graph
    for (int i = 0; i < (int) graphPart.size(); ++i) {
        /// create new node
        //graph.emplace_back(vector<Edge>());

        /// insert all edges with updated node indexes
        for (int j = 0; j < (int) graphPart[i].size(); ++j) {

            // todo toto uz bude, ale trosku jinak v te casti vespodu
            /// bylo tu toto graph[graph.size() - 1].emplace_back(Edge(graphPart[i][j].id + (timeCycle + 1) * (int) graphPart.size(), graphPart[i][j].capacity, graphPart[i][j].backedge));
            /// conection last part -> new part
            graph[i + oldSize - graphPart.size()].emplace_back(
                    Edge(graphPart[i][j].id + (timeCycle + 1) * (int) graphPart.size(), 1, false));
            /// and its backedge
            graph[graphPart[i][j].id + (timeCycle + 1) * (int) graphPart.size()].emplace_back(
                    Edge(i + oldSize - (int) graphPart.size(), 0, true));

        }
    }

    /// Add edges between fake and real nodes in the NEW PART
    for (int i = oldSize; i < (int) graph.size() - 1; i += 2) {
        graph[i].emplace_back(Edge(i + 1, 1, false));
        graph[i + 1].emplace_back(Edge(i, 0, true));
    }


    /// create connections between old part and the new part ---FOR WAITING
    for (int i = timeCycle * (int) graphPart.size(); i < oldSize; i += 2) {
        graph[i + 1].emplace_back(Edge(i + (int) graphPart.size(), 1, false)); // normal edge
        graph[i + (int) graphPart.size()].emplace_back(Edge(i + 1, 0, true));   // back edge
    }
}

void AddST(AGraph &graph, const vector<int> &trainPos, const vector<int> &powerPos, int nodesCnt, int timeCycle) {
    graph.emplace_back(vector<Edge>()); // S
    graph.emplace_back(vector<Edge>()); // T

    int sPos = (int) graph.size() - 2;
    int tPos = (int) graph.size() - 1;

    /// Add S -> all realPos of trains
    for (int trainPo : trainPos) {
        graph[sPos].emplace_back(Edge(trainPo * 2, 1, false));
        graph[trainPo * 2].emplace_back(Edge(sPos, 0, true));
    }

    /// Add all <LAST> fakePos power stations -> T
    for (int powerPo : powerPos) {
        int positionPowerStation = (nodesCnt * 2) * timeCycle + powerPo * 2 + 1; // todo check this
        graph[positionPowerStation].emplace_back(Edge(tPos, 1, false));
        graph[tPos].emplace_back(Edge(positionPowerStation, 0, true));
    }
}

void RemoveST(AGraph &graph) {
    int sPos = (int) graph.size() - 2;
    int tPos = (int) graph.size() - 1;

    /// remove all powerPos -> T
    for (int i = 0; i < (int) graph[tPos].size(); ++i)
        graph[graph[tPos][i].id].pop_back();

    /// remove all backedges to S
    for (int i = 0; i < (int) graph[sPos].size(); ++i)
        graph[graph[sPos][i].id].pop_back();

    /// Remove T and S
    graph.pop_back();
    graph.pop_back();
}

void ClearCapacity(AGraph &graph) {
    for (auto &i : graph)
        for (auto &j : i)
            j.capacity = j.backedge ? 0 : 1;
}

void ScanGraph(AGraph &graph, vector<int> &trainPos, vector<int> &powerPos, int nodesCnt, int edgesCnt, int trainsCnt) {
    for (int i = 0; i < trainsCnt; ++i) {
        int x;
        scanf("%d", &x);
        trainPos.emplace_back(x);
    }

    for (int i = 0; i < trainsCnt; ++i) {
        int x;
        scanf("%d", &x);
        powerPos.emplace_back(x);
    }

    /// Add edges between fake and real nodes
    for (int i = 0; i < nodesCnt * 2 - 1; i += 2) {
        graph[i].emplace_back(Edge(i + 1, trainsCnt, false));
        graph[i + 1].emplace_back(Edge(i, 0, true));
    }

    /// Scan Graph - allready create with duplicate edges
    for (int i = 0; i < edgesCnt; ++i) {
        int from, to;
        scanf("%d%d", &from, &to);

        graph[from * 2 + 1].emplace_back(Edge(to * 2, trainsCnt, false)); // z fake do real
        graph[to * 2].emplace_back(Edge(from * 2 + 1, 0, true)); // backedge z real do fake
    }
}
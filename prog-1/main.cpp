#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <stack>

typedef struct std::vector<std::vector<int>> AGraph;

int minimum(int a , int b) { if(a == -1) return b;if( b == -1) return a;return a < b ? a : b; }

std::unordered_set<int> g_articulation;
std::set<int> g_tundraVertex;

std::stack<int> g_stack;
int g_cnt;
void treeDFS(const AGraph &graph, std::vector<int> &discovery, std::vector<int> &low, std::vector<int> &parent, int &time, int vertex)
{
    discovery[vertex] = time;
    low[vertex] = time;
    time ++;
    int childCnt = 0;

    for(auto adjecent : graph[vertex])
    {
        if( ! discovery[adjecent]  ) // not found
        {
            g_stack.push(vertex);
            childCnt++;
            parent[adjecent] = vertex;
            treeDFS(graph, discovery, low, parent, time, adjecent);
            low[vertex] =  minimum(low[vertex], low[adjecent]);

            if( ( discovery[vertex] <= low[adjecent] && discovery[vertex] > 1) || (discovery[vertex] == 1 && childCnt > 1)) //artikulace
            {
                g_articulation.insert(vertex);

                int articCnt = 1;
                int noArticul = -1;
                int noArticulCnt = 0;

                while (g_stack.top()  != vertex)
                {
                    if(g_articulation.find(g_stack.top()) == g_articulation.end())
                    {
                        noArticulCnt++;
                        noArticul = g_stack.top();
                    }
                    else
                        articCnt++;

  //                  std::cout << g_stack.top() << " " ;
                    g_stack.pop();
                }
  //              std::cout << g_stack.top() << std::endl ;

                g_stack.pop();
                if(articCnt == 1 && noArticulCnt > 0)
                    g_tundraVertex.insert(noArticul);
            }
        }

        else if(adjecent != parent[vertex] && discovery[adjecent] < low[vertex]) //alternativni cesta
        {
            low[vertex] = minimum(low[vertex], discovery[adjecent]);
            g_stack.push(vertex);
        }
    }
}

void findArticulationAndBridgesInit(const AGraph &graph) {
    int time = 1;
    std::vector<int> discovery(graph.size(), 0);
    std::vector<int> low(graph.size(), -1);
    std::vector<int> parent(graph.size(),-1);

    treeDFS(graph, discovery, low, parent, time, 0);
}

int main() {
    int cntNodes, cntEdges;
    scanf("%d%d", &cntNodes, &cntEdges);

    AGraph graph ((unsigned)cntNodes, std::vector<int>());
    for (int i = 0; i < cntEdges; ++i) {
        int from, to;
        scanf("%d%d", &from, &to);
        graph[from].push_back(to); graph[to].push_back(from);
    }

    if(graph.size() < 3)
    {
        printf("0\n");
        return 0;
    }

    findArticulationAndBridgesInit(graph);

    if( ! g_stack.empty())
    {
        int articCnt = 0;
        int noArticul = -1;
        int noArticulCnt = 0;

        while ( ! g_stack.empty() )
        {
            if(g_articulation.find(g_stack.top()) == g_articulation.end())
            {
                noArticulCnt++;
                noArticul = g_stack.top();
            }
            else
                articCnt++;

 //           std::cout << g_stack.top() << " ";
            g_stack.pop();
        }

  //      std::cout << std::endl << std::endl;

        if(articCnt == 1 && noArticulCnt > 0)
            g_tundraVertex.insert(noArticul);

    }

    for(size_t i = 0 ; i < graph.size() ;++i)
    {
        if (graph[i].size() == 1)
        {
            g_tundraVertex.insert(i);
        }
    }

    printf("%d\n", (int)g_tundraVertex.size());
    for(auto x : g_tundraVertex)
        printf("%d ", x);

    return 0;
}




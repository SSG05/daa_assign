#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <chrono>
#include <cstring>

using namespace std;
using namespace std::chrono;

typedef map<int, set<int>> Graph;
vector<int> currentClique;
vector<int> cliqueMarker;
vector<vector<int>> graph;
auto programStartTime = high_resolution_clock::now();
vector<int> auxiliarySet;
vector<int> temporarySet;
vector<int> diffCliqueNeighbors;
vector<int> intersectCliqueNeighbors;
vector<vector<int>> diffNeighborsClique;

long graphNodeCount = 0;


void makeGraph(Graph &graphInitial, string filepath)
{
    ifstream fileStream;
    fileStream.open(filepath);
    if (!fileStream.is_open())
    {
        cout << "Error: Could not open file " << filepath << '\n';
        exit(1);
    }
    string line;
    while (getline(fileStream, line))
    {
        if (line[0] == '#')
            continue;
        istringstream iss(line);
        int u, v;
        iss >> u >> v;
        graphInitial[u].insert(v);
        graphInitial[v].insert(u);
    }
    fileStream.close();
}

struct Output
{
    int largestCliqueSize;
    int totalMaxCliques;
    float timeTaken;

    Output() : largestCliqueSize(0), totalMaxCliques(0), timeTaken(0.0f) {}

    void printTestingOutput()
    {
        cout << "Largest Size of Clique: " << largestCliqueSize << '\n';
        cout << "Total No. of Max Cliques: " << totalMaxCliques << '\n';
        cout << "Execution time: " << timeTaken << " ms\n";
    }
} stats;

void counting_sort(vector<int> &arr)
{
    if (arr.empty()) return;
    
    int maxValue = *max_element(arr.begin(), arr.end());
    vector<int> count(maxValue + 1, 0);
    vector<int> stats(arr.size());
    
    for (size_t i = 0; i < arr.size(); i++)
        count[arr[i]]++;
    
    for (int i = 1; i <= maxValue; i++)
        count[i] += count[i - 1];
    
    for (int i = arr.size() - 1; i >= 0; i--)
    {
        stats[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }
    
    for (size_t i = 0; i < arr.size(); i++)
        arr[i] = stats[i];
}


void update(int i) {
    int cliqueSize = currentClique.size();
    int neighborSize = graph[i].size();

    if (i == graphNodeCount + 1) {
        stats.totalMaxCliques++;
        if (cliqueSize > stats.largestCliqueSize)
            stats.largestCliqueSize = cliqueSize;
        return;
    }
    
    diffCliqueNeighbors.clear();
    size_t j = 0, k = 0;
    while (j < cliqueSize && k < neighborSize) {
        if (currentClique[j] < graph[i][k]) {
            diffCliqueNeighbors.push_back(currentClique[j]);
            break;
            ++j;
        } else if (graph[i][k] < currentClique[j]) {
            ++k;
        } else {
            ++j;
            ++k;
        }
    }
    while (j < cliqueSize) {
        diffCliqueNeighbors.push_back(currentClique[j]);
        break;
        ++j;
    }

    
    if (!diffCliqueNeighbors.empty())
        update(i + 1);
    
    diffCliqueNeighbors.clear();
    intersectCliqueNeighbors.clear();
    cliqueSize = currentClique.size();
    j = 0; k = 0;
    while (j < cliqueSize && k < neighborSize) {
        if (currentClique[j] < graph[i][k]) {
            diffCliqueNeighbors.push_back(currentClique[j]);
            ++j;
        } else if (graph[i][k] < currentClique[j]) {
            ++k;
        } else {
            intersectCliqueNeighbors.push_back(currentClique[j]);
            ++j;
            ++k;
        }
    }
    while (j < cliqueSize) {
        diffCliqueNeighbors.push_back(currentClique[j]);
        ++j;
    }

    if(intersectCliqueNeighbors.empty()){
        if (!diffCliqueNeighbors.empty() && diffCliqueNeighbors.back() < (i - 1))
            return;
    }
    
    memset(temporarySet.data(), 0, temporarySet.size() * sizeof(int));
    
    for (int x : intersectCliqueNeighbors) {
        for (int y : graph[x]) {
            if (!cliqueMarker[y] && y != i)
            temporarySet[y] += 1;
        }
    }
    

    for (int x : graph[i]) {
        if (!cliqueMarker[x]){
            if(x>i) break;
            if (x < i && temporarySet[x] == (int)intersectCliqueNeighbors.size())
            return;
        }
        
    }
    
    
    memset(auxiliarySet.data(), 0, auxiliarySet.size() * sizeof(int));


    diffNeighborsClique.clear();
    int p = diffCliqueNeighbors.size();
    diffNeighborsClique.resize(p);
    int* __restrict auxiliarySet_ptr = auxiliarySet.data();
    int* __restrict cliqueMarker_ptr = cliqueMarker.data();
    for (int k = 0; k < p; k++) {
        const vector<int>& neighbors = graph[diffCliqueNeighbors[k]];
        for (size_t j = 0, n = neighbors.size(); j < n; j++) {
            int y = neighbors[j];
            if (!cliqueMarker_ptr[y]) {
                ++auxiliarySet_ptr[y];
                if (y < i)
                    diffNeighborsClique[k].push_back(y);
            }
        }
    }

    
    for (int k = 0; k < p; k++) {
        int current = diffCliqueNeighbors[k];
        int prev = (k == 0 ? 0 : diffCliqueNeighbors[k - 1]);
        for (int y : diffNeighborsClique[k]) {
            if (temporarySet[y] == (int)intersectCliqueNeighbors.size()) {
                if (y >= current) {
                    auxiliarySet[y] -= 1;
                } else {
                    bool flagFirstJ = false;
                    if (y < current && y >= prev)
                        flagFirstJ = true;
                    if (flagFirstJ) {
                        if (k == 0 && auxiliarySet[y] == p)
                            return;
                        if (k > 0 && auxiliarySet[y] + k == p && prev <= y)
                            return;
                    }
                }
            }
        }
    }

    if (!intersectCliqueNeighbors.empty()) {
        for (int y = 1; y < i; y++) {
            if (!cliqueMarker[y] && temporarySet[y] == (int)intersectCliqueNeighbors.size() && auxiliarySet[y] == 0) {
                if (!diffCliqueNeighbors.empty() && diffCliqueNeighbors.back() < y)
                    return;
            }
        }
    }
    
    set<int> savedNodes(diffCliqueNeighbors.begin(), diffCliqueNeighbors.end());

    currentClique.clear();
    memset(cliqueMarker.data(), 0, cliqueMarker.size() * sizeof(int));

    for (int x : intersectCliqueNeighbors) {
        currentClique.push_back(x);
        cliqueMarker[x] = 1;
    }
    currentClique.push_back(i);
    cliqueMarker[i] = 1;

    update(i + 1);

    cliqueMarker[i] = 0;
    for(int x: currentClique){
        if(x != i)
        savedNodes.insert(x);
    }

    currentClique.clear();
    for (int x : savedNodes) {
        currentClique.push_back(x);
        cliqueMarker[x] = 1;
    }
    
}

void clique() {
    currentClique.push_back(1);
    cliqueMarker[1] = 1;
    update(2);
}

int main() {
    auto startTime = high_resolution_clock::now();
    setbuf(stdout, NULL);

    cout << "Enter the path of the file: ";
        string filepath;
        cin >> filepath;
    Graph graphInitial;
    makeGraph(graphInitial, filepath);
    graphNodeCount = graphInitial.size();
    
    int maxDegree = 0, maxVertex = 0;
    for (auto &v : graphInitial) {
        maxDegree = max(maxDegree, (int)v.second.size());
        maxVertex = max(maxVertex, v.first);
    }
    
    vector<vector<int>> degreeBuckets(maxDegree + 1);
    for (auto &v : graphInitial) {
        int degree = v.second.size();
        degreeBuckets[degree].push_back(v.first);
    }
    
    vector<int> oldToNewMap(maxVertex + 1, -1);
    vector<int> newToOldMap(graphNodeCount + 1, -1);
    long newIndex = 1;
    for (int d = 0; d <= maxDegree; ++d) {
        for (int vertex : degreeBuckets[d]) {
            oldToNewMap[vertex] = newIndex;
            newToOldMap[newIndex] = vertex;
            newIndex++;
        }
    }
    
    graph.resize(graphNodeCount + 1);
    auxiliarySet.resize(graphNodeCount + 1);
    temporarySet.resize(graphNodeCount + 1);
    cliqueMarker.resize(graphNodeCount + 1);
    diffCliqueNeighbors.resize(graphNodeCount + 1);
    intersectCliqueNeighbors.resize(graphNodeCount + 1);
    memset(cliqueMarker.data(), 0, cliqueMarker.size() * sizeof(int));
    
    for (auto &v : graphInitial) {
        int newVertex = oldToNewMap[v.first];
        for (int neighbor : v.second)
            graph[newVertex].push_back(oldToNewMap[neighbor]);
    }
    
    #pragma omp parallel for
    for (int i = 1; i <= graphNodeCount; ++i) {
        if (!graph[i].empty()) {
            if (graph[i].size() < 20)
                sort(graph[i].begin(), graph[i].end());
            else
                counting_sort(graph[i]);
        }
    }
    
    cout << "Graph built successfully\n";
    clique();
    
    auto endTime = high_resolution_clock::now();
    stats.timeTaken = duration_cast<milliseconds>(endTime - startTime).count();
    stats.printTestingOutput();
    return 0;
}

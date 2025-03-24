#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

void BronKerboschPivot(
    unordered_set<int>& P,
    unordered_set<int>& R,
    unordered_set<int>& X,
    const unordered_map<int, unordered_set<int>>& graph,
    vector<unordered_set<int>>& maxCliques,
    bool verbose = false
) {
    if (P.empty() && X.empty()) {
        maxCliques.push_back(R);
        
        if (verbose) {
            cout << "Maximal Clique: ";
            for (int v : R) {
                cout << v << " ";
            }
            cout << endl;
        }
        
        return;
    }
    
    int pivot = -1;
    int maxIntersectionSize = -1;
    
    const auto& sourceSet = !X.empty() ? X : P;
    
    for (int u : sourceSet) {
        const auto& neighbors = graph.at(u);
        
        int intersectionSize = 0;
        for (int v : P) {
            if (neighbors.count(v) > 0) {
                intersectionSize++;
            }
            
            if (intersectionSize + (P.size() - intersectionSize) <= maxIntersectionSize) {
                break;
            }
        }
        
        if (intersectionSize > maxIntersectionSize) {
            maxIntersectionSize = intersectionSize;
            pivot = u;
        }
    }
    
    const auto& pivotNeighbors = graph.at(pivot);
    
    vector<int> verticesToProcess;
    for (int v : P) {
        if (pivotNeighbors.count(v) == 0) {
            verticesToProcess.push_back(v);
        }
    }
    
    for (int v : verticesToProcess) {
        P.erase(v);
        
        R.insert(v);
        
        const auto& vNeighbors = graph.at(v);
        
        unordered_set<int> newP, newX;
        
        for (int p : (P.size() < vNeighbors.size() ? P : vNeighbors)) {
            if ((P.size() < vNeighbors.size() ? vNeighbors : P).count(p) > 0) {
                newP.insert(p);
            }
        }
        
        for (int x : (X.size() < vNeighbors.size() ? X : vNeighbors)) {
            if ((X.size() < vNeighbors.size() ? vNeighbors : X).count(x) > 0) {
                newX.insert(x);
            }
        }
        
        BronKerboschPivot(newP, R, newX, graph, maxCliques, verbose);
        
        R.erase(v);
        
        X.insert(v);
    }
}

vector<unordered_set<int>> BronKerboschDegeneracy(
    const unordered_map<int, unordered_set<int>>& graph,
    const vector<int>& degenOrdering,
    bool verbose = false
) {
    vector<unordered_set<int>> maxCliques;
    int n = degenOrdering.size();
    
    maxCliques.reserve(n);
    
    unordered_map<int, int> pos;
    pos.reserve(n);
    for (int i = 0; i < n; i++) {
        pos[degenOrdering[i]] = i;
    }
    
    for (int i = 0; i < n; i++) {
        int v = degenOrdering[i];
        
        unordered_set<int> P, R, X;
        P.reserve(graph.at(v).size());
        X.reserve(graph.at(v).size());
        
        R.insert(v);
        
        for (int neighbor : graph.at(v)) {
            int neighborPos = pos[neighbor];
            if (neighborPos > i) {
                P.insert(neighbor);
            } else if (neighborPos < i) {
                X.insert(neighbor);
            }
        }
        
        BronKerboschPivot(P, R, X, graph, maxCliques, verbose);
    }
    
    return maxCliques;
}

pair<vector<int>, int> DegeneracyOrdering(
    const unordered_map<int, unordered_set<int>>& graph
) {
    int n = graph.size();
    
    unordered_map<int, int> degree;
    for (const auto& [vertex, neighbors] : graph) {
        degree[vertex] = neighbors.size();
    }
    
    vector<int> vertices;
    vertices.reserve(n);
    for (const auto& [vertex, _] : graph) {
        vertices.push_back(vertex);
    }
    
    vector<int> ordering;
    ordering.reserve(n);
    int degeneracy = 0;
    
    int maxDegree = 0;
    for (const auto& [_, d] : degree) {
        maxDegree = max(maxDegree, d);
    }
    
    vector<vector<int>> buckets(maxDegree + 1);
    for (int v : vertices) {
        buckets[degree[v]].push_back(v);
    }
    
    unordered_set<int> proc;
    for (int d = 0; d <= maxDegree; d++) {
        
        if (buckets[d].empty()) continue;
        
        while (!buckets[d].empty()) {
            int v = buckets[d].back();
            buckets[d].pop_back();
            
            if (proc.count(v) > 0) continue;
            
            ordering.push_back(v);
            proc.insert(v);
            degeneracy = max(degeneracy, d);
            
            for (int neighbor : graph.at(v)) {
                if (proc.count(neighbor) > 0) continue;
                
                int oDegree = degree[neighbor];
                if (oDegree <= d) continue;
                
                degree[neighbor]--;
                int nDegree = degree[neighbor];
                
                buckets[nDegree].push_back(neighbor);
            }
        }
    }
    
    reverse(ordering.begin(), ordering.end());
    
    return {ordering, degeneracy};
}

unordered_map<int, unordered_set<int>> processGraph(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    
    unordered_map<int, unordered_set<int>> graph;
    int from, to;
    
    while (file >> from >> to) {
        graph[from].insert(to);
        graph[to].insert(from);
    }
    
    file.close();
    cout << "Loaded graph with " << graph.size() << " vertices" << endl;
    
    return graph;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    
    string filename = argv[1];
    
    auto start = chrono::high_resolution_clock::now();
    unordered_map<int, unordered_set<int>> graph = processGraph(filename);
    
    auto [ordering, degeneracy] = DegeneracyOrdering(graph);
    
    cout << "Degeneracy: " << degeneracy << endl;
    
    vector<unordered_set<int>> maximalCliques = BronKerboschDegeneracy(graph, ordering, false);
    auto end = chrono::high_resolution_clock::now();
    
    cout << "Number of maximal cliques: " << maximalCliques.size() << endl;
    cout << "Clique finding time: "
         << chrono::duration<double>(end - start).count() << " seconds" << endl;
    
    size_t maxSize = 0;
    for (const auto& clique : maximalCliques) {
        maxSize = max(maxSize, clique.size());
    }
    
    cout << "Size of largest clique: " << maxSize << endl;
}

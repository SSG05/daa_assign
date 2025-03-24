#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <queue>

using namespace std;
using namespace chrono;

class Graph {
public:
    int V;
    vector<vector<int>> adj;
    int maximalCliqueCount = 0;
    int maxCliqueSize = 0;
    bool printCliques = false;
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }
    void addEdge(int u, int v) {
        if (u >= V || v >= V) return; 
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    void optimizeGraph() {
        cout << "Optimizing graph structure..." << endl;
        for (int i = 0; i < V; i++) {
            sort(adj[i].begin(), adj[i].end());
            adj[i].erase(unique(adj[i].begin(), adj[i].end()), adj[i].end());
        }
    }
    void findCliques() {
        auto start = high_resolution_clock::now();
        optimizeGraph();
        vector<vector<int>> components = findConnectedComponents();
        cout << "Found " << components.size() << " connected components" << endl;
        for (size_t i = 0; i < components.size(); i++) {
            if (components[i].size() > 1000) {
                cout << "Processing large component " << i << " with " << components[i].size() << " nodes..." << endl;
            } else if (i % 1000 == 0) {
                cout << "Processed " << i << " components..." << endl;
            }
            processComponent(components[i]);
        }
        auto stop = high_resolution_clock::now();
        cout << "\nTotal Number of Maximal Cliques: " << maximalCliqueCount << endl;
        cout << "Largest Clique Size: " << maxCliqueSize << endl;
        cout << "Algorithm Execution Time: "
             << duration_cast<seconds>(stop - start).count()
             << " seconds" << endl;
    }

private:
    vector<vector<int>> findConnectedComponents() {
        vector<vector<int>> components;
        vector<bool> visited(V, false);
        
        for (int v = 0; v < V; v++) {
            if (!visited[v] && !adj[v].empty()) {
                vector<int> component;
                queue<int> q;
                q.push(v);
                visited[v] = true;
                
                while (!q.empty()) {
                    int curr = q.front(); q.pop();
                    component.push_back(curr);
                    
                    for (int neighbor : adj[curr]) {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            q.push(neighbor);
                        }
                    }
                }
                
                components.push_back(component);
            }
        }
        sort(components.begin(), components.end(),
             [](const vector<int>& a, const vector<int>& b) {
                 return a.size() > b.size();
             });
        return components;
    }
    vector<int> setIntersection(const vector<int>& a, const vector<int>& b) {
        vector<int> result;
        result.reserve(min(a.size(), b.size()));
        
        size_t i = 0, j = 0;
        while (i < a.size() && j < b.size()) {
            if (a[i] < b[j]) {
                i++;
            } else if (b[j] < a[i]) {
                j++;
            } else {
                result.push_back(a[i]);
                i++;
                j++;
            }
        }
        
        return result;
    }
    void processComponent(const vector<int>& component) {
        if (component.size() < 2) return;
        unordered_map<int, int> nodeMap;
        for (size_t i = 0; i < component.size(); i++) {
            nodeMap[component[i]] = i;
        }
        
        int compSize = component.size();
        vector<vector<int>> compAdj(compSize);
        for (int i = 0; i < compSize; i++) {
            int origNode = component[i];
            for (int neighbor : adj[origNode]) {
                auto it = nodeMap.find(neighbor);
                if (it != nodeMap.end()) {
                    compAdj[i].push_back(it->second);
                }
            }
            sort(compAdj[i].begin(), compAdj[i].end());
        }
        
        // Initialize SUBG and CAND
        vector<int> SUBG, CAND, Q;
        SUBG.reserve(compSize);
        CAND.reserve(compSize);
        
        for (int i = 0; i < compSize; i++) {
            SUBG.push_back(i);
            CAND.push_back(i);
        }
        expandComponent(compAdj, SUBG, CAND, Q, component);
    }
    int choosePivot(const vector<vector<int>>& compAdj,
                   const vector<int>& SUBG,
                   const vector<int>& CAND) {
        int pivot = -1, maxCount = -1;
        
        for (int v : SUBG) {
            vector<int> intersection = setIntersection(compAdj[v], CAND);
            int count = intersection.size();
            
            if (count > maxCount) {
                maxCount = count;
                pivot = v;
            }
        }
        
        return pivot;
    }
    vector<int> getNonNeighbors(const vector<vector<int>>& compAdj,
                               int pivot,
                               const vector<int>& CAND) {
        vector<int> nonNeighbors;
        nonNeighbors.reserve(CAND.size());
        for (int v : CAND) {
            if (!binary_search(compAdj[pivot].begin(), compAdj[pivot].end(), v)) {
                nonNeighbors.push_back(v);
            }
        }
        
        return nonNeighbors;
    }
    
    // Tomita algorithm for a component
    void expandComponent(const vector<vector<int>>& compAdj,
                        vector<int> SUBG,
                        vector<int> CAND,
                        vector<int> Q,
                        const vector<int>& component) {
        if (CAND.empty()) {
            if (!SUBG.empty()) return;
            
            int cliqueSize = Q.size();
            maxCliqueSize = max(maxCliqueSize, cliqueSize);
            maximalCliqueCount++;
            
            if (printCliques && cliqueSize >= 3) { // Only print cliques of size >= 3
                cout << "Maximal Clique (" << cliqueSize << " nodes): ";
                for (int v : Q) {
                    cout << component[v] << " ";
                }
                cout << endl;
            }
            return;
        }
        int pivot = choosePivot(compAdj, SUBG, CAND);
        
        vector<int> nonNeighbors = getNonNeighbors(compAdj, pivot, CAND);
        
        for (int q : nonNeighbors) {
            vector<int> Q_new = Q;
            Q_new.push_back(q);
            vector<int> SUBG_new = setIntersection(SUBG, compAdj[q]);
            vector<int> CAND_new = setIntersection(CAND, compAdj[q]);
            expandComponent(compAdj, SUBG_new, CAND_new, Q_new, component);
            CAND.erase(remove(CAND.begin(), CAND.end(), q), CAND.end());
        }
    }
};

int main(int argc, char* argv[]) {
    string filename = (argc > 1) ? argv[1] : "as-skitter.txt";
    cout << "Processing graph from: " << filename << endl;
    
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file: " << filename << endl;
        return 1;
    }

    auto startReading = high_resolution_clock::now();
    string line;
    int maxNode = 0;
    vector<pair<int, int>> edges;
    edges.reserve(50000000); 
    
    cout << "Reading graph..." << endl;
    while (getline(file, line)) {
        if (line[0] == '#') continue;
        istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v)) continue;
        edges.emplace_back(u, v);
        maxNode = max(maxNode, max(u, v));
    }
    file.close();
    
    auto stopReading = high_resolution_clock::now();
    cout << "Graph read complete. Nodes: " << (maxNode + 1) << ", Edges: " << edges.size() << endl;
    cout << "Reading time: " << duration_cast<seconds>(stopReading - startReading).count() << " seconds" << endl;
    Graph g(maxNode + 1);
    
    cout << "Building graph structure" << endl;
    for (const auto& [u, v] : edges) {
        g.addEdge(u, v);
    }
    
    cout << "Finding maximal cliques\n";
    g.findCliques();

    return 0;
}

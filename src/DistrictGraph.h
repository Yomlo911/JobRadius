#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "utils.h"

// Module 3: Graph (undirected, unweighted adjacency list of districts)
class DistrictGraph {
private:
    unordered_map<string, vector<string>> adj;
    unordered_map<string, string> displayName;
    vector<string> none;
    size_t edges;

    static bool contains(const vector<string>& v, const string& x) {
        for (size_t i = 0; i < v.size(); i++) if (v[i] == x) return true;  //linear search
        return false;
    }
    //adds district to graph if they dont exist
    void addNode(const string& name) {
        string key = toLower(name);        
        if (adj.find(key) == adj.end()) {
            adj[key] = vector<string>();
            displayName[key] = name;
        }
    }

public:
    DistrictGraph() : edges(0) {}
    //connects 2 districts 
    void addEdge(const string& a, const string& b) {
        string ka = toLower(a), kb = toLower(b);
        if (ka == kb) return;
        addNode(a);
        addNode(b);
        if (contains(adj[ka], kb)) return;
        adj[ka].push_back(kb);
        adj[kb].push_back(ka);
        edges++;
    }
    //checks if district exists 
    bool hasDistrict(const string& district) const {
        return adj.find(toLower(district)) != adj.end();
    }
    //returns connected districts
    const vector<string>& neighbors(const string& district) const {
        auto it = adj.find(toLower(district));
        return (it == adj.end()) ? none : it->second;
    }

    string nameOf(const string& district) const {
        auto it = displayName.find(toLower(district));
        return (it == displayName.end()) ? district : it->second;
    }

    size_t districtCount() const { return adj.size(); }
    size_t edgeCount() const     { return edges; }
};

inline DistrictGraph buildUttarakhandGraph() {
    DistrictGraph g;
    g.addEdge("Dehradun",      "Haridwar");
    g.addEdge("Dehradun",      "Tehri Garhwal");
    g.addEdge("Dehradun",      "Pauri Garhwal");
    g.addEdge("Haridwar",      "Pauri Garhwal");
    g.addEdge("Tehri Garhwal", "Pauri Garhwal");
    g.addEdge("Pauri Garhwal", "Nainital");
    g.addEdge("Pauri Garhwal", "Almora");
    g.addEdge("Nainital",      "Almora");
    g.addEdge("Nainital",      "Udham Singh Nagar");
    return g;
}

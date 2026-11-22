#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include "utils.h"
#include "Queue.h"
#include "JobIndex.h"
#include "DistrictGraph.h"

// Module 4: Search (hash lookup, then BFS over neighboring districts)
struct Match {
    int jobId;
    int hop;
};
//stores search results
struct SearchResult {
    vector<Match> matches;
    vector<string> districtsSearched;
    int farthestHop;
    bool expanded() const { return districtsSearched.size() > 1; } //checks if search expanded beyond the district we entered
};

inline vector<int> intersectSorted(const vector<int>& a, const vector<int>& b) {
    vector<int> out;
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j])      { out.push_back(a[i]); i++; j++; }
        else if (a[i] < b[j])  i++;
        else                   j++;
    }
    return out;
}
//initialises job index and district graph references
class JobSearch {
private:
    const JobIndex& index;
    const DistrictGraph& graph;
    struct Node { string key; int hop; };

public:
    JobSearch(const JobIndex& index, const DistrictGraph& graph)
        : index(index), graph(graph) {}
   //searches for jobs based on requirements entered
    SearchResult search(const string& district, const string& skill,
                        size_t threshold, int maxHops) const {
        SearchResult res;
        res.farthestHop = 0;
        const vector<int>& withSkill = index.jobsWithSkill(skill);

        unordered_set<string> visited;
        Queue<Node> q;
        string startKey = toLower(district);
        q.enqueue(Node{startKey, 0});
        visited.insert(startKey);

        int currentLevel = 0;
        while (!q.isEmpty()) {
            Node cur = q.dequeue();

            if (cur.hop > currentLevel) {
                currentLevel = cur.hop;
                if (res.matches.size() >= threshold) break;
            }

            vector<int> ids = intersectSorted(index.jobsInDistrict(cur.key), withSkill);
            for (size_t i = 0; i < ids.size(); i++) {
                res.matches.push_back(Match{ids[i], cur.hop});
            }
            res.districtsSearched.push_back(
                graph.hasDistrict(cur.key) ? graph.nameOf(cur.key)
                                           : (cur.hop == 0 ? district : cur.key));
            res.farthestHop = cur.hop;

            if (cur.hop < maxHops) {
                const vector<string>& nb = graph.neighbors(cur.key);
                for (size_t i = 0; i < nb.size(); i++) {
                    if (visited.find(nb[i]) == visited.end()) {
                        visited.insert(nb[i]);
                        q.enqueue(Node{nb[i], cur.hop + 1});
                    }
                }
            }
        }
        return res;
    }
};

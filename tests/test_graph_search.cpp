#include <unordered_set>
#include "test_helpers.h"
#include "../src/SampleData.h"
#include "../src/JobIndex.h"
#include "../src/DistrictGraph.h"
#include "../src/JobSearch.h"
//checks if result show the expected job ids
bool matchesAre(const SearchResult& r, const vector<int>& ids, const vector<int>& hops) {
    if (r.matches.size() != ids.size()) return false;
    for (size_t i = 0; i < ids.size(); i++)
        if (r.matches[i].jobId != ids[i] || r.matches[i].hop != hops[i]) return false;
    return true;
}
//checks for duplicate district names
bool noRepeats(const vector<string>& v) {
    unordered_set<string> seen(v.begin(), v.end());
    return seen.size() == v.size();
}

int main() {
    vector<Job> jobs = loadSampleJobs();
    JobIndex index;
    index.build(jobs);
    DistrictGraph graph = buildUttarakhandGraph();
    JobSearch searcher(index, graph);
    //this part tests dis count, edge count, neighbouring districts, case insensitivity
    cout << "Module 3 (Graph)\n";
    check(graph.districtCount() == 7, "7 districts in the graph");
    check(graph.edgeCount() == 9, "9 border edges in the graph");
    check(graph.neighbors("Dehradun") == vector<string>({"haridwar", "tehri garhwal", "pauri garhwal"}),
          "Dehradun neighbours: Haridwar, Tehri Garhwal, Pauri Garhwal");
    check(graph.neighbors("Udham Singh Nagar") == vector<string>({"nainital"}), "Udham Singh Nagar has only Nainital");
    {
        bool twoWay = true;
        const char* names[] = {"Dehradun", "Haridwar", "Tehri Garhwal", "Pauri Garhwal", "Nainital", "Almora", "Udham Singh Nagar"};
        for (int a = 0; a < 7; a++) {
            const vector<string>& nb = graph.neighbors(names[a]);
            for (size_t i = 0; i < nb.size(); i++) {
                const vector<string>& back = graph.neighbors(nb[i]);
                bool found = false;
                for (size_t k = 0; k < back.size(); k++) if (back[k] == toLower(names[a])) found = true;
                if (!found) twoWay = false;
            }
        }
        check(twoWay, "every edge works both ways (undirected)");
    }
    check(graph.neighbors("DEHRADUN") == graph.neighbors("dehradun"), "graph lookup ignores case");
    check(graph.neighbors("Mumbai").empty() && !graph.hasDistrict("Mumbai"), "unknown district has no neighbours");
    check(graph.nameOf("udham singh nagar") == "Udham Singh Nagar", "nameOf gives the nicely written name");
    {
        DistrictGraph g2;
        g2.addEdge("A", "B");
        g2.addEdge("b", "a");
        g2.addEdge("A", "A");
        check(g2.edgeCount() == 1 && g2.districtCount() == 2, "repeated edges and self-loops are ignored");
    }
   //checks skill based job searching, bfs, handling of unknown districts
    cout << "Module 4 (Search)\n";
    check(intersectSorted({0, 1, 2, 3, 4}, {0, 2, 7, 12, 13}) == vector<int>({0, 2}), "intersection of district list and skill list");
    check(intersectSorted({}, {1, 2}).empty() && intersectSorted({1, 2}, {3, 4}).empty(), "intersection with an empty or disjoint list");
    {
        SearchResult r = searcher.search("Dehradun", "python", 3, 2);
        check(matchesAre(r, {0, 2, 7}, {0, 0, 1}), "python in Dehradun, need 3: jobs 0,2 (hop 0) + job 7 (hop 1)");
        check(r.districtsSearched == vector<string>({"Dehradun", "Haridwar", "Tehri Garhwal", "Pauri Garhwal"}), "stops after hop 1 once enough jobs are found");
        check(r.expanded() && r.farthestHop == 1, "search was expanded, farthest hop = 1");
    }
    {
        SearchResult r = searcher.search("Dehradun", "python", 1, 2);
        check(matchesAre(r, {0, 2}, {0, 0}) && !r.expanded(), "enough jobs already: no expansion");
    }
    {
        SearchResult r = searcher.search("Dehradun", "python", 5, 0);
        check(matchesAre(r, {0, 2}, {0, 0}) && r.districtsSearched.size() == 1, "hop limit 0: only the chosen district");
    }
    {
        SearchResult r = searcher.search("Udham Singh Nagar", "c++", 2, 2);
        check(matchesAre(r, {10}, {1}), "hop limit 2: c++ from Udham Singh Nagar finds only job 10");
        check(r.districtsSearched == vector<string>({"Udham Singh Nagar", "Nainital", "Pauri Garhwal", "Almora"}) && r.farthestHop == 2,
              "hop limit 2: never goes past 2 hops");
    }
    {
        SearchResult r = searcher.search("Udham Singh Nagar", "c++", 2, 3);
        check(matchesAre(r, {10, 0, 5}, {1, 3, 3}), "hop limit 3: reaches jobs 0 and 5 three hops away");
    }
    {
        SearchResult r = searcher.search("Udham Singh Nagar", "c++", 3, 3);
        check(matchesAre(r, {10, 0, 5}, {1, 3, 3}) && r.districtsSearched.size() == 7, "menu settings (need 3, 3 hops): all 7 districts searched");
    }
    {
        SearchResult r = searcher.search("Dehradun", "rust", 2, 5);
        check(r.matches.empty() && r.districtsSearched.size() == 7, "no matches anywhere: whole graph searched");
        check(noRepeats(r.districtsSearched), "no district is searched twice");
        check(r.farthestHop == 3, "farthest district from Dehradun is 3 hops away");
    }
    {
        SearchResult r = searcher.search("Mumbai", "python", 3, 3);
        check(r.matches.empty() && r.districtsSearched.size() == 1, "unknown district: empty result, no crash");
    }
    {
        SearchResult a = searcher.search("DEHRADUN", "PYTHON", 3, 2);
        SearchResult b = searcher.search("dehradun", "python", 3, 2);
        check(matchesAre(a, {0, 2, 7}, {0, 0, 1}) && a.districtsSearched == b.districtsSearched, "search ignores letter case");
    }
    return finish();
}

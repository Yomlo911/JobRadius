// ============================================================
//  JobRadius - single-file C++ job search (Data Structures + OOP)
//  Modules: 1 Data | 2 Index | 3 Graph | 4 Search | 5 Ranking
//           6 Sorting | 7 Main
//  Build:   g++ -std=c++17 -Wall -Wextra -o jobradius jobradius.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
using namespace std;

// ------------------------------------------------------------
//  Utility: lower-case copy of a string.
//  Used everywhere a skill or district is compared, so that
//  "C++", "c++" and "C++ " style variants do not break lookups.
// ------------------------------------------------------------
string toLower(const string& s) {
    string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        if (out[i] >= 'A' && out[i] <= 'Z') out[i] = out[i] - 'A' + 'a';
    }
    return out;
}

// ============================================================
//  MODULE 1: DATA
//  Job class (encapsulation: private fields, public getters)
//  plus a curated sample of job records.
// ============================================================
class Job {
private:
    int id;                    // unique id, also the position in the job list
    string title;
    string company;
    string district;
    vector<string> skills;     // as written in the source data
    int salary;                // annual salary in INR

public:
    Job(int id, const string& title, const string& company,
        const string& district, const vector<string>& skills, int salary)
        : id(id), title(title), company(company),
          district(district), skills(skills), salary(salary) {}

    // ----- getters (read-only access to private data) -----
    int getId() const                         { return id; }
    const string& getTitle() const            { return title; }
    const string& getCompany() const          { return company; }
    const string& getDistrict() const         { return district; }
    const vector<string>& getSkills() const   { return skills; }
    int getSalary() const                     { return salary; }

    // Case-insensitive skill check.
    bool hasSkill(const string& skill) const {
        string wanted = toLower(skill);
        for (size_t i = 0; i < skills.size(); i++) {
            if (toLower(skills[i]) == wanted) return true;
        }
        return false;
    }

    void print() const {
        cout << "[" << id << "] " << title << " | " << company
             << " | " << district << " | Rs " << salary << " | skills: ";
        for (size_t i = 0; i < skills.size(); i++) {
            cout << skills[i] << (i + 1 < skills.size() ? ", " : "");
        }
        cout << "\n";
    }
};

// Sample data. REPLACE these rows with real records picked from the
// Naukri dataset (keep the district names identical to the ones you
// will use in the Graph module).
vector<Job> loadSampleJobs() {
    vector<Job> jobs;
    int next = 0;
    jobs.push_back(Job(next++, "Software Engineer",   "TechNova",    "Dehradun",          {"C++", "Python", "SQL"},      600000));
    jobs.push_back(Job(next++, "Backend Developer",   "CodeCraft",   "Dehradun",          {"Java", "SQL"},               550000));
    jobs.push_back(Job(next++, "Data Analyst",        "InsightWorks","Dehradun",          {"Python", "SQL", "Excel"},    450000));
    jobs.push_back(Job(next++, "Frontend Developer",  "PixelLabs",   "Dehradun",          {"JavaScript", "React"},       500000));
    jobs.push_back(Job(next++, "QA Engineer",         "TestBridge",  "Dehradun",          {"Java", "Selenium"},          400000));
    jobs.push_back(Job(next++, "Junior C++ Developer","EmbedSoft",   "Haridwar",          {"C++", "Linux"},              420000));
    jobs.push_back(Job(next++, "Accounts Executive",  "SteelCorp",   "Haridwar",          {"Excel", "Tally"},            300000));
    jobs.push_back(Job(next++, "Python Developer",    "AutoMate",    "Haridwar",          {"Python", "Django"},          480000));
    jobs.push_back(Job(next++, "Network Technician",  "HillNet",     "Tehri Garhwal",     {"Networking", "Linux"},       320000));
    jobs.push_back(Job(next++, "IT Support Engineer", "GarhwalTech", "Pauri Garhwal",     {"Networking", "Windows"},     280000));
    jobs.push_back(Job(next++, "Software Developer",  "LakeSoft",    "Nainital",          {"C++", "Java"},               520000));
    jobs.push_back(Job(next++, "Data Entry Analyst",  "KumaonData",  "Nainital",          {"Excel", "SQL"},              260000));
    jobs.push_back(Job(next++, "Full Stack Developer","SidcuTech",   "Udham Singh Nagar", {"JavaScript", "Python", "SQL"}, 650000));
    jobs.push_back(Job(next++, "Python Automation Engineer", "FactoryAI", "Udham Singh Nagar", {"Python", "Linux"},     580000));
    jobs.push_back(Job(next++, "Web Developer",       "AlmoraWeb",   "Almora",            {"JavaScript", "React"},       350000));
    return jobs;
}

// ============================================================
//  MODULE 2: INDEX
//  Two hash maps, built once at start-up:
//      skill    -> ids of jobs that need that skill
//      district -> ids of jobs located in that district
//  A lookup hashes the key and jumps straight to its bucket, so it
//  costs O(1) on average instead of scanning every job (O(n)).
//  All keys are stored in lower case, so "Python", "PYTHON" and
//  "python" reach the same bucket.
// ============================================================
class JobIndex {
private:
    unordered_map<string, vector<int> > skillIndex;
    unordered_map<string, vector<int> > districtIndex;
    vector<int> none;   // returned for keys that are not in the index

public:
    // Build both maps in a single pass over the job list.
    void build(const vector<Job>& jobs) {
        skillIndex.clear();
        districtIndex.clear();

        for (size_t i = 0; i < jobs.size(); i++) {
            int id = jobs[i].getId();

            districtIndex[toLower(jobs[i].getDistrict())].push_back(id);

            const vector<string>& skills = jobs[i].getSkills();
            for (size_t k = 0; k < skills.size(); k++) {
                vector<int>& bucket = skillIndex[toLower(skills[k])];
                // jobs are visited in order, so a repeated skill on the
                // same job would show up as the last id: skip it.
                if (bucket.empty() || bucket.back() != id) bucket.push_back(id);
            }
        }
    }

    // Ids of all jobs needing this skill (empty list if none / unknown).
    const vector<int>& jobsWithSkill(const string& skill) const {
        unordered_map<string, vector<int> >::const_iterator it =
            skillIndex.find(toLower(skill));
        return (it == skillIndex.end()) ? none : it->second;
    }

    // Ids of all jobs in this district (empty list if none / unknown).
    const vector<int>& jobsInDistrict(const string& district) const {
        unordered_map<string, vector<int> >::const_iterator it =
            districtIndex.find(toLower(district));
        return (it == districtIndex.end()) ? none : it->second;
    }

    size_t skillCount() const    { return skillIndex.size(); }
    size_t districtCount() const { return districtIndex.size(); }
};

// ============================================================
//  MODULE 3: GRAPH
//  Districts are nodes; an edge joins two districts that share a
//  border. The graph is undirected (if A borders B, B borders A)
//  and unweighted (every edge is one "hop"). It is stored as an
//  adjacency list, which only keeps the edges that exist.
//  Keys are lower case, so lookups ignore capitalisation.
// ============================================================
class DistrictGraph {
private:
    unordered_map<string, vector<string> > adj;   // key -> neighbour keys
    unordered_map<string, string> displayName;    // key -> name as first written
    vector<string> none;                          // returned for unknown districts
    size_t edges;

    static bool contains(const vector<string>& v, const string& x) {
        for (size_t i = 0; i < v.size(); i++) if (v[i] == x) return true;
        return false;
    }

    void addNode(const string& name) {
        string key = toLower(name);
        if (adj.find(key) == adj.end()) {
            adj[key] = vector<string>();
            displayName[key] = name;
        }
    }

public:
    DistrictGraph() : edges(0) {}

    // Add a two-way edge. Self-loops and repeated edges are ignored.
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

    bool hasDistrict(const string& district) const {
        return adj.find(toLower(district)) != adj.end();
    }

    // Neighbour keys (lower case) of a district; empty if unknown.
    const vector<string>& neighbors(const string& district) const {
        unordered_map<string, vector<string> >::const_iterator it =
            adj.find(toLower(district));
        return (it == adj.end()) ? none : it->second;
    }

    // Nice-looking name for a key, e.g. "udham singh nagar" -> "Udham Singh Nagar".
    string nameOf(const string& district) const {
        unordered_map<string, string>::const_iterator it =
            displayName.find(toLower(district));
        return (it == displayName.end()) ? district : it->second;
    }

    size_t districtCount() const { return adj.size(); }
    size_t edgeCount() const     { return edges; }
};

// Border relations between the districts used in the sample data.
// CHECK every pair against a map of Uttarakhand before the evaluation.
DistrictGraph buildUttarakhandGraph() {
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

// ============================================================
//  MODULE 4: SEARCH
//  1. Hash lookup: jobs in the district AND needing the skill.
//  2. If fewer than `threshold` jobs were found, run a BFS over the
//     graph: check every district 1 hop away, then 2 hops, and so
//     on, never going past `maxHops`. A queue keeps "nearest first"
//     and a visited set stops a district being searched twice.
//  The whole hop level is searched before the "enough?" check, so
//  nearer districts are never skipped in favour of farther ones.
// ============================================================
struct Match {
    int jobId;
    int hop;       // 0 = the district asked for, 1 = a neighbour, ...
};

struct SearchResult {
    vector<Match> matches;
    vector<string> districtsSearched;   // in the order they were searched
    int farthestHop;                    // deepest hop level searched
    bool expanded() const { return districtsSearched.size() > 1; }
};

// Common ids of two lists that are both sorted ascending: O(a + b).
// The index lists are sorted because the index is built in id order.
vector<int> intersectSorted(const vector<int>& a, const vector<int>& b) {
    vector<int> out;
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j])      { out.push_back(a[i]); i++; j++; }
        else if (a[i] < b[j])  i++;
        else                   j++;
    }
    return out;
}

class JobSearch {
private:
    const JobIndex& index;
    const DistrictGraph& graph;

    struct Node { string key; int hop; };

public:
    JobSearch(const JobIndex& index, const DistrictGraph& graph)
        : index(index), graph(graph) {}

    SearchResult search(const string& district, const string& skill,
                        size_t threshold, int maxHops) const {
        SearchResult res;
        res.farthestHop = 0;

        const vector<int>& withSkill = index.jobsWithSkill(skill);

        unordered_set<string> visited;
        queue<Node> q;
        string startKey = toLower(district);
        q.push(Node{startKey, 0});
        visited.insert(startKey);

        int currentLevel = 0;
        while (!q.empty()) {
            Node cur = q.front();
            q.pop();

            // First district of a new hop level: stop if we already
            // have enough, otherwise carry on outwards.
            if (cur.hop > currentLevel) {
                currentLevel = cur.hop;
                if (res.matches.size() >= threshold) break;
            }

            // Search this district through the hash index.
            vector<int> ids = intersectSorted(index.jobsInDistrict(cur.key), withSkill);
            for (size_t i = 0; i < ids.size(); i++) {
                res.matches.push_back(Match{ids[i], cur.hop});
            }
            res.districtsSearched.push_back(
                graph.hasDistrict(cur.key) ? graph.nameOf(cur.key)
                                           : (cur.hop == 0 ? district : cur.key));
            res.farthestHop = cur.hop;

            // Queue the neighbours that have not been seen yet.
            if (cur.hop < maxHops) {
                const vector<string>& nb = graph.neighbors(cur.key);
                for (size_t i = 0; i < nb.size(); i++) {
                    if (visited.find(nb[i]) == visited.end()) {
                        visited.insert(nb[i]);
                        q.push(Node{nb[i], cur.hop + 1});
                    }
                }
            }
        }
        return res;
    }
};

// ------------------------------------------------------------
//  Temporary main - checks Modules 1 to 4. It will be replaced by
//  the console menu in Module 7.
// ------------------------------------------------------------
static int passed = 0, failed = 0;
void check(bool ok, const string& name) {
    cout << (ok ? "  PASS  " : "  FAIL  ") << name << "\n";
    if (ok) passed++; else failed++;
}

bool matchesAre(const SearchResult& r, const vector<int>& ids, const vector<int>& hops) {
    if (r.matches.size() != ids.size()) return false;
    for (size_t i = 0; i < ids.size(); i++)
        if (r.matches[i].jobId != ids[i] || r.matches[i].hop != hops[i]) return false;
    return true;
}

bool districtsAre(const SearchResult& r, const vector<string>& want) {
    return r.districtsSearched == want;
}

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

    cout << "Module 1 (Data)\n";
    check(jobs.size() == 15, "15 sample jobs loaded");
    check(jobs[0].hasSkill("PYTHON"), "hasSkill is case-insensitive");

    cout << "Module 2 (Index)\n";
    check(index.jobsWithSkill("python") == vector<int>({0, 2, 7, 12, 13}), "skill 'python' -> jobs 0,2,7,12,13");
    check(index.jobsInDistrict("Dehradun") == vector<int>({0, 1, 2, 3, 4}), "district 'Dehradun' -> jobs 0-4");
    check(index.jobsWithSkill("rust").empty(), "unknown skill returns empty list");

    cout << "Module 3 (Graph)\n";
    check(graph.districtCount() == 7, "7 districts in the graph");
    check(graph.edgeCount() == 9, "9 border edges in the graph");
    check(graph.neighbors("Dehradun") == vector<string>({"haridwar", "tehri garhwal", "pauri garhwal"}),
          "Dehradun neighbours: Haridwar, Tehri Garhwal, Pauri Garhwal");
    check(graph.neighbors("Udham Singh Nagar") == vector<string>({"nainital"}),
          "Udham Singh Nagar has only Nainital as neighbour");
    {
        bool two_way = true;
        const char* names[] = {"Dehradun", "Haridwar", "Tehri Garhwal", "Pauri Garhwal",
                               "Nainital", "Almora", "Udham Singh Nagar"};
        for (int a = 0; a < 7; a++) {
            const vector<string>& nb = graph.neighbors(names[a]);
            for (size_t i = 0; i < nb.size(); i++) {
                const vector<string>& back = graph.neighbors(nb[i]);
                bool found = false;
                for (size_t k = 0; k < back.size(); k++)
                    if (back[k] == toLower(names[a])) found = true;
                if (!found) two_way = false;
            }
        }
        check(two_way, "every edge works both ways (undirected)");
    }
    check(graph.neighbors("DEHRADUN") == graph.neighbors("dehradun"), "graph lookup ignores case");
    check(graph.neighbors("Mumbai").empty() && !graph.hasDistrict("Mumbai"), "unknown district has no neighbours");
    {
        DistrictGraph g2;
        g2.addEdge("A", "B");
        g2.addEdge("b", "a");      // same edge again, different case/order
        g2.addEdge("A", "A");      // self-loop
        check(g2.edgeCount() == 1 && g2.districtCount() == 2, "repeated edges and self-loops are ignored");
    }

    cout << "Module 4 (Search)\n";
    check(intersectSorted({0, 1, 2, 3, 4}, {0, 2, 7, 12, 13}) == vector<int>({0, 2}),
          "intersection of district list and skill list");
    {
        SearchResult r = searcher.search("Dehradun", "python", 3, 2);
        check(matchesAre(r, {0, 2, 7}, {0, 0, 1}), "python in Dehradun, need 3: jobs 0,2 (hop 0) + job 7 (hop 1)");
        check(districtsAre(r, {"Dehradun", "Haridwar", "Tehri Garhwal", "Pauri Garhwal"}),
              "stops after hop 1 once enough jobs are found");
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
        check(districtsAre(r, {"Udham Singh Nagar", "Nainital", "Pauri Garhwal", "Almora"}) && r.farthestHop == 2,
              "hop limit 2: never goes past 2 hops");
    }
    {
        SearchResult r = searcher.search("Udham Singh Nagar", "c++", 2, 3);
        check(matchesAre(r, {10, 0, 5}, {1, 3, 3}), "hop limit 3: reaches jobs 0 and 5 three hops away");
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
        check(matchesAre(a, {0, 2, 7}, {0, 0, 1}) && a.districtsSearched == b.districtsSearched,
              "search ignores letter case");
    }

    cout << "\nResult: " << passed << " passed, " << failed << " failed\n";

    // Small demo of the thin-results case.
    cout << "\n--- Demo: 'python' jobs near Dehradun (want at least 3, up to 2 hops) ---\n";
    SearchResult demo = searcher.search("Dehradun", "python", 3, 2);
    for (size_t i = 0; i < demo.matches.size(); i++) {
        cout << "  hop " << demo.matches[i].hop << "  ";
        jobs[demo.matches[i].jobId].print();
    }
    cout << "  Districts searched:";
    for (size_t i = 0; i < demo.districtsSearched.size(); i++) cout << " " << demo.districtsSearched[i] << (i + 1 < demo.districtsSearched.size() ? "," : "");
    cout << "\n";

    return failed == 0 ? 0 : 1;
}

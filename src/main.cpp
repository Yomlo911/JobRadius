// JobRadius - Module 7: Main (console menu)

#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "SampleData.h"
#include "JobIndex.h"
#include "DistrictGraph.h"
#include "JobSearch.h"
#include "Sorting.h"
#include "Ranking.h"

int main() {
    vector<Job> jobs = loadSampleJobs();
    JobIndex index;
    index.build(jobs);
    DistrictGraph graph = buildUttarakhandGraph();
    JobSearch searcher(index, graph);
    RankingEngine ranker(make_unique<MultiFactorStrategy>());

    cout << "   JOB RADIUS: Regional Job Search Engine (Uttarakhand)\n";
    cout << "   Powered by Graph BFS, Inverted Index & Strategy Ranking\n";
    cout << "===============================================================\n\n";

    while (true) {
        string district, skill;
        int expectedSalary = 0;

        cout << "\nEnter Preferred District (or 'exit' to quit): ";
        if (!getline(cin, district)) break;
        district = trim(district);
        if (toLower(district) == "exit") break;

        cout << "Enter Required Skill (e.g., C++, Python, SQL, Excel, Java): ";
        if (!getline(cin, skill)) break;
        skill = trim(skill);
        if (district.empty() || skill.empty()) {
            cout << "--> Please enter both a district and a skill.\n";
            continue;
        }

        cout << "Enter Minimum Expected Salary in INR (e.g., 400000, 0 for any): ";
        string salStr;
        if (!getline(cin, salStr)) break;
        salStr = trim(salStr);
        if (salStr.empty()) {
            expectedSalary = 0;
        } else if (!allDigits(salStr)) {
            cout << "--> Salary not understood, using 0 (any).\n";
            expectedSalary = 0;
        } else {
            try { expectedSalary = stoi(salStr); }
            catch (...) { expectedSalary = 0; }
        }

        cout << "Ranking mode (1 = multi-factor, 2 = location-first) [1]: ";
        string mode;
        if (!getline(cin, mode)) break;
        mode = trim(mode);
        if (mode == "2") ranker.setStrategy(make_unique<LocationFirstStrategy>());
        else             ranker.setStrategy(make_unique<MultiFactorStrategy>());

        UserProfile user = {"Candidate", district, expectedSalary, {skill}};

        cout << "\nSearching for '" << skill << "' jobs near '" << district
             << "' (" << (mode == "2" ? "location-first" : "multi-factor") << " ranking)...\n";
        SearchResult result = searcher.search(district, skill, 3, 3);

        if (result.matches.empty()) {
            cout << "--> No matching jobs found within search radius.\n";
            continue;
        }

        vector<ScoredJob> scoredJobs = ranker.rank(result, jobs, user);
        mergeSort(scoredJobs);

        cout << "\n====================== SEARCH RESULTS ======================\n";
        for (size_t i = 0; i < scoredJobs.size(); i++) {
            int id = scoredJobs[i].jobId;
            double score = scoredJobs[i].score;
            int hop = 0;
            for (size_t m = 0; m < result.matches.size(); m++) {
                if (result.matches[m].jobId == id) { hop = result.matches[m].hop; break; }
            }
            cout << "Rank #" << (i + 1) << " | Match Score: " << static_cast<int>(score)
                 << "/100 | Distance: " << hop << " hop(s)\n  ";
            jobs[id].print();
        }

        cout << "\nDistricts Searched in Proximity Order:";
        for (size_t i = 0; i < result.districtsSearched.size(); i++) {
            cout << " " << result.districtsSearched[i] << (i + 1 < result.districtsSearched.size() ? " ->" : "");
        }
        cout << "\n============================================================\n";
    }

    cout << "\nThank you for using JobRadius!\n";
    return 0;
}

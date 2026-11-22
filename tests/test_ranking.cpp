#include "test_helpers.h"
#include "../src/SampleData.h"
#include "../src/JobIndex.h"
#include "../src/DistrictGraph.h"
#include "../src/JobSearch.h"
#include "../src/Ranking.h"

int main() {
    vector<Job> jobs = loadSampleJobs();
    JobIndex index;
    index.build(jobs);
    DistrictGraph graph = buildUttarakhandGraph();
    JobSearch searcher(index, graph);

    UserProfile py = {"T", "Dehradun", 0, {"python"}};
    UserProfile cpp = {"T", "Udham Singh Nagar", 0, {"c++"}};
    MultiFactorStrategy mf;
    LocationFirstStrategy lf;

    cout << "Module 5 (Ranking): multi-factor strategy\n";
    check(near(mf.calculateScore(jobs[0], 0, py), 66.67), "job 0 (3 skills), 0 hops, no salary: 66.67");
    check(near(mf.calculateScore(jobs[7], 1, py), 60.0), "job 7 (2 skills), 1 hop: 60.00");
    check(near(mf.calculateScore(jobs[10], 1, cpp), 60.0), "job 10 (2 skills), 1 hop: 60.00");
    check(near(mf.calculateScore(jobs[5], 3, cpp), 52.5), "job 5 (2 skills), 3 hops: 52.50");
    check(near(mf.calculateScore(jobs[0], 3, cpp), 44.17), "job 0 (3 skills), 3 hops: 44.17");
    check(near(mf.calculateScore(jobs[1], 0, UserProfile{"T", "D", 0, {"sql"}}), 75.0), "job 1 (2 skills) scores above job 0 (3 skills) for the same skill");
    {
        UserProfile p = {"T", "Dehradun", 500000, {"python"}};
        check(near(mf.calculateScore(jobs[0], 0, p), 66.67), "salary 600000 vs minimum 500000 gives the full salary part");
        check(near(mf.calculateScore(jobs[2], 0, p), 61.67), "salary 450000 vs minimum 500000 lowers the score: 61.67");
        check(near(mf.calculateScore(jobs[7], 1, p), 56.0), "salary 480000 vs minimum 500000, 1 hop: 56.00");
        UserProfile far = {"T", "Dehradun", 100000, {"python"}};
        check(near(mf.calculateScore(jobs[0], 0, far), 66.67), "salary far above the minimum is capped, no bonus beyond full");
    }
    check(near(MultiFactorStrategy(1, 0, 0).calculateScore(jobs[0], 0, py), 33.33), "weights (1,0,0): skill part only");
    check(near(MultiFactorStrategy(0, 1, 0).calculateScore(jobs[0], 2, py), 33.33), "weights (0,1,0): proximity part only");
    check(near(MultiFactorStrategy(0, 0, 1).calculateScore(jobs[0], 0, py), 100.0), "weights (0,0,1): salary part only");
    check(near(mf.calculateScore(jobs[0], 0, UserProfile{"T", "D", 0, {}}), 50.0), "no skill given: skill part is 0");
    check(near(mf.calculateScore(jobs[0], 0, UserProfile{"T", "D", 0, {"rust"}}), 50.0), "skill the job does not need: skill part is 0");

    cout << "Module 5 (Ranking): location-first strategy\n";
    check(near(lf.calculateScore(jobs[0], 0, py), 100) && near(lf.calculateScore(jobs[0], 1, py), 75), "0 hops = 100, 1 hop = 75");
    check(near(lf.calculateScore(jobs[0], 2, py), 50) && near(lf.calculateScore(jobs[0], 3, py), 25), "2 hops = 50, 3 hops = 25");
    check(near(lf.calculateScore(jobs[0], 5, py), 0), "5 hops would be negative, so it stops at 0");
    {
        UserProfile p = {"T", "D", 500000, {"python"}};
        check(near(lf.calculateScore(jobs[0], 1, p), 85), "1 hop with the salary met: 75 + 10 = 85");
        check(near(lf.calculateScore(jobs[0], 0, p), 100), "0 hops with the salary met: capped at 100");
        check(near(lf.calculateScore(jobs[2], 1, p), 75), "salary below the minimum: no bonus");
        check(near(lf.calculateScore(jobs[0], 1, py), 75), "no minimum entered: no bonus");
    }

    cout << "Module 5 (Ranking): engine and strategy swap\n";
    {
        unique_ptr<RankingStrategy> p = make_unique<LocationFirstStrategy>();
        check(near(p->calculateScore(jobs[0], 0, py), 100), "calling through a base-class pointer runs the right version");
    }
    SearchResult r = searcher.search("Dehradun", "python", 3, 3);
    RankingEngine engine(make_unique<MultiFactorStrategy>());
    vector<ScoredJob> s1 = engine.rank(r, jobs, py);
    check(s1.size() == 3 && s1[0].jobId == 0 && s1[1].jobId == 2 && s1[2].jobId == 7, "rank keeps one score per match, in match order");
    check(near(s1[0].score, 66.67) && near(s1[1].score, 66.67) && near(s1[2].score, 60.0), "engine scores 66.67, 66.67, 60");
    engine.setStrategy(make_unique<LocationFirstStrategy>());
    vector<ScoredJob> s2 = engine.rank(r, jobs, py);
    check(near(s2[0].score, 100) && near(s2[1].score, 100) && near(s2[2].score, 75), "after setStrategy the scores are 100, 100, 75");
    {
        SearchResult bad;
        bad.farthestHop = 0;
        bad.matches.push_back(Match{99, 0});
        bad.matches.push_back(Match{-1, 0});
        check(engine.rank(bad, jobs, py).empty(), "job ids outside the list are ignored");
        check(engine.rank(SearchResult{}, jobs, py).empty(), "no matches gives an empty ranking");
    }

    cout << "Ranking + sorting together\n";
    {
        RankingEngine e2(make_unique<MultiFactorStrategy>());
        SearchResult far = searcher.search("Udham Singh Nagar", "c++", 3, 3);
        vector<ScoredJob> s = e2.rank(far, jobs, cpp);
        check(s[0].jobId == 10 && s[1].jobId == 0 && s[2].jobId == 5, "search order before sorting: 10, 0, 5");
        mergeSort(s);
        check(s[0].jobId == 10 && s[1].jobId == 5 && s[2].jobId == 0, "after Merge Sort: 10 (60), 5 (52.5), 0 (44.17)");
        vector<ScoredJob> t = e2.rank(r, jobs, py);
        mergeSort(t);
        check(t[0].jobId == 0 && t[1].jobId == 2 && t[2].jobId == 7, "tied scores (jobs 0 and 2) keep search order");
    }
    return finish();
}

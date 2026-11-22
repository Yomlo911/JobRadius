#include "test_helpers.h"
#include "../src/SampleData.h"
#include "../src/JobIndex.h"

int main() {
    vector<Job> jobs = loadSampleJobs();
    JobIndex index;
    index.build(jobs);

    cout << "Module 1 (Data)\n";
    check(jobs.size() == 15, "15 sample jobs loaded");
    bool idsOk = true;
    for (size_t i = 0; i < jobs.size(); i++) if (jobs[i].getId() != (int)i) idsOk = false;
    check(idsOk, "job id equals its position in the list");
    check(jobs[0].getTitle() == "Software Engineer" && jobs[0].getDistrict() == "Dehradun" && jobs[0].getSalary() == 600000, "job 0 fields are read correctly");
    check(jobs[0].hasSkill("PYTHON") && jobs[0].hasSkill("c++"), "hasSkill is case-insensitive");
    check(!jobs[0].hasSkill("rust") && !jobs[4].hasSkill("python"), "hasSkill rejects a missing skill");

    cout << "Module 2 (Index)\n";
    check(index.jobsWithSkill("python") == vector<int>({0, 2, 7, 12, 13}), "skill 'python' -> jobs 0,2,7,12,13");
    check(index.jobsWithSkill("PYTHON") == index.jobsWithSkill("python"), "skill lookup ignores case");
    check(index.jobsWithSkill("C++") == vector<int>({0, 5, 10}), "skill 'C++' -> jobs 0,5,10");
    check(index.jobsWithSkill("rust").empty(), "unknown skill returns an empty list");
    check(index.jobsInDistrict("Dehradun") == vector<int>({0, 1, 2, 3, 4}), "district 'Dehradun' -> jobs 0-4");
    check(index.jobsInDistrict("dehradun") == index.jobsInDistrict("Dehradun"), "district lookup ignores case");
    check(index.jobsInDistrict("Haridwar") == vector<int>({5, 6, 7}), "district 'Haridwar' -> jobs 5,6,7");
    check(index.jobsInDistrict("Mumbai").empty(), "unknown district returns an empty list");
    check(index.districtCount() == 7, "7 distinct districts indexed");
    check(index.skillCount() == 13, "13 distinct skills indexed");

    bool consistent = true, complete = true, sorted = true;
    for (size_t j = 0; j < jobs.size(); j++) {
        const vector<string>& sk = jobs[j].getSkills();
        for (size_t k = 0; k < sk.size(); k++) {
            const vector<int>& ids = index.jobsWithSkill(sk[k]);
            bool found = false;
            for (size_t i = 0; i < ids.size(); i++) {
                if (ids[i] == jobs[j].getId()) found = true;
                if (!jobs[ids[i]].hasSkill(sk[k])) consistent = false;
                if (i > 0 && ids[i - 1] >= ids[i]) sorted = false;
            }
            if (!found) complete = false;
        }
    }
    check(consistent, "every indexed id really has that skill");
    check(complete, "no job is missing from the skill index");
    check(sorted, "id lists are sorted ascending (needed by the search)");
    return finish();
}

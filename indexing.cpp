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

// ------------------------------------------------------------
//  Temporary main - checks Modules 1 and 2. It will be replaced by
//  the console menu in Module 7.
// ------------------------------------------------------------
static int passed = 0, failed = 0;
void check(bool ok, const string& name) {
    cout << (ok ? "  PASS  " : "  FAIL  ") << name << "\n";
    if (ok) passed++; else failed++;
}
bool sameIds(const vector<int>& got, const vector<int>& want) { return got == want; }

int main() {
    vector<Job> jobs = loadSampleJobs();
    JobIndex index;
    index.build(jobs);

    cout << "Module 1 (Data)\n";
    check(jobs.size() == 15, "15 sample jobs loaded");
    check(jobs[0].hasSkill("PYTHON"), "hasSkill is case-insensitive");
    check(!jobs[0].hasSkill("rust"), "hasSkill rejects a missing skill");

    cout << "Module 2 (Index)\n";
    check(sameIds(index.jobsWithSkill("python"), vector<int>{0, 2, 7, 12, 13}),
          "skill 'python' -> jobs 0,2,7,12,13");
    check(sameIds(index.jobsWithSkill("PYTHON"), index.jobsWithSkill("python")),
          "skill lookup ignores case (PYTHON == python)");
    check(sameIds(index.jobsWithSkill("C++"), vector<int>{0, 5, 10}),
          "skill 'C++' -> jobs 0,5,10");
    check(index.jobsWithSkill("rust").empty(), "unknown skill returns empty list");
    check(sameIds(index.jobsInDistrict("Dehradun"), vector<int>{0, 1, 2, 3, 4}),
          "district 'Dehradun' -> jobs 0-4");
    check(sameIds(index.jobsInDistrict("dehradun"), index.jobsInDistrict("Dehradun")),
          "district lookup ignores case");
    check(sameIds(index.jobsInDistrict("Haridwar"), vector<int>{5, 6, 7}),
          "district 'Haridwar' -> jobs 5,6,7");
    check(index.jobsInDistrict("Mumbai").empty(), "unknown district returns empty list");
    check(index.districtCount() == 7, "7 distinct districts indexed");
    check(index.skillCount() == 13, "13 distinct skills indexed");

    // Consistency: every id the index returns must really match the job.
    bool consistent = true;
    const char* skillsToCheck[] = {"python", "sql", "java", "excel", "linux", "networking"};
    for (int s = 0; s < 6; s++) {
        const vector<int>& ids = index.jobsWithSkill(skillsToCheck[s]);
        for (size_t i = 0; i < ids.size(); i++)
            if (!jobs[ids[i]].hasSkill(skillsToCheck[s])) consistent = false;
    }
    check(consistent, "every indexed id really has that skill");

    // Completeness: no matching job is missing from the index.
    bool complete = true;
    for (size_t j = 0; j < jobs.size(); j++) {
        const vector<string>& sk = jobs[j].getSkills();
        for (size_t k = 0; k < sk.size(); k++) {
            const vector<int>& ids = index.jobsWithSkill(sk[k]);
            bool found = false;
            for (size_t i = 0; i < ids.size(); i++) if (ids[i] == jobs[j].getId()) found = true;
            if (!found) complete = false;
        }
    }
    check(complete, "no job is missing from the skill index");

    cout << "\nResult: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

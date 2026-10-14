// ============================================================
//  JobRadius - single-file C++ job search (Data Structures + OOP)
//  Modules: 1 Data | 2 Index | 3 Graph | 4 Search | 5 Ranking
//           6 Sorting | 7 Main
//  Build:   g++ -std=c++17 -Wall -Wextra -o jobradius jobradius.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
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

// ------------------------------------------------------------
//  Temporary main - only checks Module 1. It will be replaced by
//  the console menu in Module 7.
// ------------------------------------------------------------
int main() {
    vector<Job> jobs = loadSampleJobs();

    cout << "Loaded " << jobs.size() << " jobs:\n";
    for (size_t i = 0; i < jobs.size(); i++) jobs[i].print();

    cout << "\n--- Module 1 checks ---\n";
    cout << "Job 0 has 'PYTHON' (mixed case): " << (jobs[0].hasSkill("PYTHON") ? "yes" : "no") << "  (expected yes)\n";
    cout << "Job 0 has 'c++':                 " << (jobs[0].hasSkill("c++")    ? "yes" : "no") << "  (expected yes)\n";
    cout << "Job 0 has 'rust':                " << (jobs[0].hasSkill("rust")   ? "yes" : "no") << "  (expected no)\n";
    cout << "Job 4 has 'python':              " << (jobs[4].hasSkill("python") ? "yes" : "no") << "  (expected no)\n";
    cout << "Job ids match positions: ";
    bool ok = true;
    for (size_t i = 0; i < jobs.size(); i++) if (jobs[i].getId() != (int)i) ok = false;
    cout << (ok ? "yes" : "NO") << "  (expected yes)\n";
    return 0;
}

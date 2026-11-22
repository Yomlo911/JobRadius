#pragma once

#include <unordered_map>
#include <vector>
#include "Job.h"

// Module 2: Index (skill -> job ids, district -> job ids)
class JobIndex {
private:
    unordered_map<string, vector<int>> skillIndex;
    unordered_map<string, vector<int>> districtIndex;
    vector<int> none;

public:
    void build(const vector<Job>& jobs) {
        skillIndex.clear();
        districtIndex.clear();
        for (size_t i = 0; i < jobs.size(); i++) {
            int id = jobs[i].getId();
            districtIndex[toLower(jobs[i].getDistrict())].push_back(id);
            const vector<string>& skills = jobs[i].getSkills();
            for (size_t k = 0; k < skills.size(); k++) {
                vector<int>& bucket = skillIndex[toLower(skills[k])];
                if (bucket.empty() || bucket.back() != id) bucket.push_back(id);
            }
        }
    }
   //returns jobs with the entered skills
    const vector<int>& jobsWithSkill(const string& skill) const {
        auto it = skillIndex.find(toLower(skill));
        return (it == skillIndex.end()) ? none : it->second;
    }
  //returns jobs in the entered district
    const vector<int>& jobsInDistrict(const string& district) const {
        auto it = districtIndex.find(toLower(district));
        return (it == districtIndex.end()) ? none : it->second;
    }

    size_t skillCount() const    { return skillIndex.size(); }
    size_t districtCount() const { return districtIndex.size(); }
};

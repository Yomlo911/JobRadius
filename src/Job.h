#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "utils.h"

// Module 1: Data
class Job {
private:
    int id;
    string title;
    string company;
    string district;
    vector<string> skills;
    int salary;

public:
    Job(int id, const string& title, const string& company,
        const string& district, const vector<string>& skills, int salary)
        : id(id), title(title), company(company),
          district(district), skills(skills), salary(salary) {}

    int getId() const                         { return id; }
    const string& getTitle() const            { return title; }
    const string& getCompany() const          { return company; }
    const string& getDistrict() const         { return district; }
    const vector<string>& getSkills() const   { return skills; }
    int getSalary() const                     { return salary; }
   //list of skills required
    bool hasSkill(const string& skill) const {
        string wanted = toLower(skill);
        for (size_t i = 0; i < skills.size(); i++) {
            if (toLower(skills[i]) == wanted) return true;
        }
        return false;
    }

    void print() const {
        cout << "[" << id << "] " << title << " | " << company
             << " | " << district << " | Rs " << salary << " | Skills: ";
        for (size_t i = 0; i < skills.size(); i++) {
            cout << skills[i] << (i + 1 < skills.size() ? ", " : "");
        }
        cout << "\n";
    }
};

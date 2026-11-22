#pragma once

#include <vector>
#include "Job.h"

// Module 1: sample job records. 
inline vector<Job> loadSampleJobs() {
    vector<Job> jobs;
    int next = 0;
    jobs.push_back(Job(next++, "Software Engineer",          "TechNova",    "Dehradun",          {"C++", "Python", "SQL"},      600000));
    jobs.push_back(Job(next++, "Backend Developer",          "CodeCraft",   "Dehradun",          {"Java", "SQL"},               550000));
    jobs.push_back(Job(next++, "Data Analyst",               "InsightWorks","Dehradun",          {"Python", "SQL", "Excel"},    450000));
    jobs.push_back(Job(next++, "Frontend Developer",         "PixelLabs",   "Dehradun",          {"JavaScript", "React"},       500000));
    jobs.push_back(Job(next++, "QA Engineer",                "TestBridge",  "Dehradun",          {"Java", "Selenium"},          400000));
    jobs.push_back(Job(next++, "Junior C++ Developer",       "EmbedSoft",   "Haridwar",          {"C++", "Linux"},              420000));
    jobs.push_back(Job(next++, "Accounts Executive",         "SteelCorp",   "Haridwar",          {"Excel", "Tally"},            300000));
    jobs.push_back(Job(next++, "Python Developer",           "AutoMate",    "Haridwar",          {"Python", "Django"},          480000));
    jobs.push_back(Job(next++, "Network Technician",         "HillNet",     "Tehri Garhwal",     {"Networking", "Linux"},       320000));
    jobs.push_back(Job(next++, "IT Support Engineer",        "GarhwalTech", "Pauri Garhwal",     {"Networking", "Windows"},     280000));
    jobs.push_back(Job(next++, "Software Developer",         "LakeSoft",    "Nainital",          {"C++", "Java"},               520000));
    jobs.push_back(Job(next++, "Data Entry Analyst",         "KumaonData",  "Nainital",          {"Excel", "SQL"},              260000));
    jobs.push_back(Job(next++, "Full Stack Developer",       "SidcuTech",   "Udham Singh Nagar", {"JavaScript", "Python", "SQL"}, 650000));
    jobs.push_back(Job(next++, "Python Automation Engineer", "FactoryAI",   "Udham Singh Nagar", {"Python", "Linux"},          580000));
    jobs.push_back(Job(next++, "Web Developer",              "AlmoraWeb",   "Almora",            {"JavaScript", "React"},       350000));
    return jobs;

}

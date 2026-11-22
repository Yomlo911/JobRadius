#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "Job.h"
#include "JobSearch.h"
#include "Sorting.h"

// Module 5: Ranking (Strategy patterns)
struct UserProfile {
    string name;
    string preferredDistrict;
    int minSalary;
    vector<string> skills;
};

class RankingStrategy {
public:
    virtual ~RankingStrategy() = default;
    virtual double calculateScore(const Job& job, int hopDistance, const UserProfile& user) const = 0;
};

class MultiFactorStrategy : public RankingStrategy {
private:
    double skillWeight, proximityWeight, salaryWeight;

public:
    MultiFactorStrategy(double wSkill = 0.5, double wProx = 0.3, double wSalary = 0.2)
        : skillWeight(wSkill), proximityWeight(wProx), salaryWeight(wSalary) {}

    double calculateScore(const Job& job, int hopDistance, const UserProfile& user) const override {
        const vector<string>& jobSkills = job.getSkills();
        double skillScore = 0.0;
        if (!jobSkills.empty()) {
            int matches = 0;
            for (size_t i = 0; i < user.skills.size(); i++) {
                if (job.hasSkill(user.skills[i])) matches++;
            }
            skillScore = min(1.0, static_cast<double>(matches) / jobSkills.size());
        }
        double proximityScore = 1.0 / (1.0 + hopDistance);
        double salaryScore = 1.0;
        if (user.minSalary > 0) {
            double ratio = static_cast<double>(job.getSalary()) / user.minSalary;
            salaryScore = min(1.2, ratio) / 1.2;
        }
        return (skillWeight * skillScore + proximityWeight * proximityScore + salaryWeight * salaryScore) * 100.0;
    }
};

class LocationFirstStrategy : public RankingStrategy {
public:
    double calculateScore(const Job& job, int hopDistance, const UserProfile& user) const override {
        double baseScore = 100.0 - (hopDistance * 25.0);
        if (baseScore < 0.0) baseScore = 0.0;
        double bonus = (user.minSalary > 0 && job.getSalary() >= user.minSalary) ? 10.0 : 0.0;
        return min(100.0, baseScore + bonus);
    }
};

class RankingEngine {
private:
    unique_ptr<RankingStrategy> strategy;

public:
    RankingEngine(unique_ptr<RankingStrategy> initialStrategy)
        : strategy(std::move(initialStrategy)) {}

    void setStrategy(unique_ptr<RankingStrategy> newStrategy) { strategy = std::move(newStrategy); }

    vector<ScoredJob> rank(const SearchResult& searchResult, const vector<Job>& allJobs, const UserProfile& user) const {
        vector<ScoredJob> scoredList;
        scoredList.reserve(searchResult.matches.size());
        for (size_t i = 0; i < searchResult.matches.size(); i++) {
            int id = searchResult.matches[i].jobId;
            int hop = searchResult.matches[i].hop;
            if (id >= 0 && static_cast<size_t>(id) < allJobs.size()) {
                double score = strategy->calculateScore(allJobs[id], hop, user);
                scoredList.push_back(ScoredJob{id, score});
            }
        }
        return scoredList;
    }

};

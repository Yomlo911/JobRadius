#include <cstdlib>
#include "test_helpers.h"
#include "../src/Sorting.h"

vector<ScoredJob> make(const vector<double>& scores) {
    vector<ScoredJob> v;
    for (size_t i = 0; i < scores.size(); i++) v.push_back(ScoredJob{(int)i, scores[i]});
    return v;
}

bool isDescending(const vector<ScoredJob>& v) {
    for (size_t i = 1; i < v.size(); i++) if (v[i - 1].score < v[i].score) return false;
    return true;
}

bool isStable(const vector<ScoredJob>& v) {
    for (size_t i = 1; i < v.size(); i++)
        if (v[i - 1].score == v[i].score && v[i - 1].jobId > v[i].jobId) return false;
    return true;
}

bool isPermutation(const vector<ScoredJob>& v, size_t n) {
    vector<int> seen(n, 0);
    if (v.size() != n) return false;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i].jobId < 0 || (size_t)v[i].jobId >= n) return false;
        seen[v[i].jobId]++;
    }
    for (size_t i = 0; i < n; i++) if (seen[i] != 1) return false;
    return true;
}

bool idsAre(const vector<ScoredJob>& v, const vector<int>& want) {
    if (v.size() != want.size()) return false;
    for (size_t i = 0; i < v.size(); i++) if (v[i].jobId != want[i]) return false;
    return true;
}

int main() {
    cout << "Module 6 (Merge Sort)\n";
    { vector<ScoredJob> v; mergeSort(v);                          check(v.empty(), "empty list stays empty"); }
    { vector<ScoredJob> v = make({7.5}); mergeSort(v);            check(idsAre(v, {0}), "single item stays as it is"); }
    { vector<ScoredJob> v = make({3, 9, 1, 7, 5}); mergeSort(v);  check(idsAre(v, {1, 3, 4, 0, 2}), "unsorted: 9,7,5,3,1"); }
    { vector<ScoredJob> v = make({9, 7, 5, 3, 1}); mergeSort(v);  check(idsAre(v, {0, 1, 2, 3, 4}), "already descending: unchanged"); }
    { vector<ScoredJob> v = make({1, 2, 3, 4, 5}); mergeSort(v);  check(idsAre(v, {4, 3, 2, 1, 0}), "ascending input is fully reversed"); }
    { vector<ScoredJob> v = make({4, 4, 4, 4}); mergeSort(v);     check(idsAre(v, {0, 1, 2, 3}), "all scores equal: original order kept (stable)"); }
    { vector<ScoredJob> v = make({5, 8, 5, 8, 2, 5}); mergeSort(v); check(idsAre(v, {1, 3, 0, 2, 5, 4}), "duplicate scores: ties keep original order"); }
    { vector<ScoredJob> v = make({-2.5, 0, 3.25, -7, 0}); mergeSort(v); check(idsAre(v, {2, 1, 4, 0, 3}), "negative, zero and decimal scores"); }
    { vector<ScoredJob> v = make({2, 1}); mergeSort(v);           check(idsAre(v, {0, 1}), "two items already in order"); }
    { vector<ScoredJob> v = make({1, 2}); mergeSort(v);           check(idsAre(v, {1, 0}), "two items out of order are swapped"); }
    {
        srand(12345);
        vector<double> sc;
        for (int i = 0; i < 1000; i++) sc.push_back(rand() % 50);
        vector<ScoredJob> v = make(sc);
        mergeSort(v);
        check(isDescending(v), "1000 random scores: descending order");
        check(isStable(v), "1000 random scores: ties keep original order");
        check(isPermutation(v, 1000), "1000 random scores: no id lost or duplicated");
    }
    return finish();
}

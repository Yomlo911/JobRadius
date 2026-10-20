// ============================================================
//  JobRadius - MODULE 6: SORTING (standalone)
//  Hand-written Merge Sort that orders scored jobs from the
//  highest score to the lowest. std::sort is NOT used.
//  It has no dependency on the other modules, so it can be built
//  and tested alone, then pasted into jobradius.cpp unchanged.
//  Build:  g++ -std=c++17 -Wall -Wextra -o mergesort 
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

// A job id paired with the score the ranking strategy gave it.
struct ScoredJob {
    int jobId;
    double score;
};

// ------------------------------------------------------------
//  Merge two sorted halves a[lo..mid] and a[mid+1..hi] into one
//  sorted run, using `tmp` as scratch space.
//  Using ">=" takes from the LEFT half on a tie, so equal scores
//  keep their original order (the sort is stable).
// ------------------------------------------------------------
void mergeHalves(vector<ScoredJob>& a, vector<ScoredJob>& tmp,
                 int lo, int mid, int hi) {
    int i = lo;        // next unused item in the left half
    int j = mid + 1;   // next unused item in the right half
    int k = lo;        // next free slot in tmp

    while (i <= mid && j <= hi) {
        if (a[i].score >= a[j].score) tmp[k++] = a[i++];
        else                          tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];   // left half leftovers
    while (j <= hi)  tmp[k++] = a[j++];   // right half leftovers

    for (int x = lo; x <= hi; x++) a[x] = tmp[x];
}

// Split a[lo..hi] in two, sort each half, then merge them.
void mergeSortRange(vector<ScoredJob>& a, vector<ScoredJob>& tmp, int lo, int hi) {
    if (lo >= hi) return;                 // 0 or 1 item: already sorted
    int mid = lo + (hi - lo) / 2;
    mergeSortRange(a, tmp, lo, mid);
    mergeSortRange(a, tmp, mid + 1, hi);
    mergeHalves(a, tmp, lo, mid, hi);
}

// Public entry point: sorts `a` by score, highest first. O(n log n).
void mergeSort(vector<ScoredJob>& a) {
    if (a.size() < 2) return;
    vector<ScoredJob> tmp(a.size());
    mergeSortRange(a, tmp, 0, (int)a.size() - 1);
}

// ------------------------------------------------------------
//  Tests (temporary: they leave this file when it is merged)
// ------------------------------------------------------------
static int passed = 0, failed = 0;
void check(bool ok, const string& name) {
    cout << (ok ? "  PASS  " : "  FAIL  ") << name << "\n";
    if (ok) passed++; else failed++;
}

vector<ScoredJob> make(const vector<double>& scores) {
    vector<ScoredJob> v;
    for (size_t i = 0; i < scores.size(); i++) v.push_back(ScoredJob{(int)i, scores[i]});
    return v;
}

bool isDescending(const vector<ScoredJob>& v) {
    for (size_t i = 1; i < v.size(); i++) if (v[i - 1].score < v[i].score) return false;
    return true;
}

// Equal scores must keep their original (increasing id) order.
bool isStable(const vector<ScoredJob>& v) {
    for (size_t i = 1; i < v.size(); i++)
        if (v[i - 1].score == v[i].score && v[i - 1].jobId > v[i].jobId) return false;
    return true;
}

// Same ids as before, each exactly once (nothing lost or duplicated).
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

    { vector<ScoredJob> v; mergeSort(v);
      check(v.empty(), "empty list stays empty"); }

    { vector<ScoredJob> v = make({7.5}); mergeSort(v);
      check(idsAre(v, {0}), "single item stays as it is"); }

    { vector<ScoredJob> v = make({3, 9, 1, 7, 5}); mergeSort(v);
      check(idsAre(v, {1, 3, 4, 0, 2}), "unsorted: 9,7,5,3,1 (ids 1,3,4,0,2)"); }

    { vector<ScoredJob> v = make({9, 7, 5, 3, 1}); mergeSort(v);
      check(idsAre(v, {0, 1, 2, 3, 4}), "already descending: unchanged"); }

    { vector<ScoredJob> v = make({1, 2, 3, 4, 5}); mergeSort(v);
      check(idsAre(v, {4, 3, 2, 1, 0}), "ascending input is fully reversed"); }

    { vector<ScoredJob> v = make({4, 4, 4, 4}); mergeSort(v);
      check(idsAre(v, {0, 1, 2, 3}), "all scores equal: original order kept (stable)"); }

    { vector<ScoredJob> v = make({5, 8, 5, 8, 2, 5}); mergeSort(v);
      check(idsAre(v, {1, 3, 0, 2, 5, 4}), "duplicate scores: ties keep original order"); }

    { vector<ScoredJob> v = make({-2.5, 0, 3.25, -7, 0}); mergeSort(v);
      check(idsAre(v, {2, 1, 4, 0, 3}), "negative, zero and decimal scores"); }

    { vector<ScoredJob> v = make({2, 1}); mergeSort(v);
      check(idsAre(v, {0, 1}), "two items already in order");
      vector<ScoredJob> w = make({1, 2}); mergeSort(w);
      check(idsAre(w, {1, 0}), "two items out of order are swapped"); }

    {   // 1000 pseudo-random scores from a small range, so there are many ties.
        srand(12345);
        vector<double> sc;
        for (int i = 0; i < 1000; i++) sc.push_back(rand() % 50);
        vector<ScoredJob> v = make(sc);
        mergeSort(v);
        check(isDescending(v), "1000 random scores: descending order");
        check(isStable(v),     "1000 random scores: ties keep original order");
        check(isPermutation(v, 1000), "1000 random scores: no id lost or duplicated");
    }

    cout << "\nResult: " << passed << " passed, " << failed << " failed\n";

    cout << "\n--- Demo: scores 62, 85, 40, 85, 71 ---\n";
    vector<ScoredJob> demo = make({62, 85, 40, 85, 71});
    mergeSort(demo);
    for (size_t i = 0; i < demo.size(); i++)
        cout << "  job " << demo[i].jobId << "  score " << demo[i].score << "\n";

    return failed == 0 ? 0 : 1;
}

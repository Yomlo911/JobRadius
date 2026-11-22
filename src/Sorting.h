#pragma once

#include <vector>

using namespace std;

// Module 6: Sorting (merge sorting )
struct ScoredJob {
    int jobId;
    double score;
};

inline void mergeHalves(vector<ScoredJob>& a, vector<ScoredJob>& tmp, int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        if (a[i].score >= a[j].score) tmp[k++] = a[i++];
        else                          tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi)  tmp[k++] = a[j++];
    for (int x = lo; x <= hi; x++) a[x] = tmp[x];
}

inline void mergeSortRange(vector<ScoredJob>& a, vector<ScoredJob>& tmp, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    mergeSortRange(a, tmp, lo, mid);
    mergeSortRange(a, tmp, mid + 1, hi);
    mergeHalves(a, tmp, lo, mid, hi);
}

inline void mergeSort(vector<ScoredJob>& a) {
    if (a.size() < 2) return;
    vector<ScoredJob> tmp(a.size());
    mergeSortRange(a, tmp, 0, (int)a.size() - 1);
}

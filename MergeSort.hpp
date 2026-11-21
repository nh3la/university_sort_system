#pragma once

#include <cstddef>
#include <vector>

using namespace std;

struct SortStats {
    size_t comparisons = 0;
    size_t merges = 0;
};

template <typename T, typename Compare>
void mergeParts(vector<T>& values, vector<T>& temp, size_t start, size_t middle,
                size_t end, Compare& compare, SortStats& stats) {
    for (size_t i = start; i < end; i++) temp[i] = values[i];

    size_t left = start;
    size_t right = middle;
    for (size_t i = start; i < end; i++) {
        if (left == middle) values[i] = temp[right++];
        else if (right == end) values[i] = temp[left++];
        else {
            stats.comparisons++;
            if (compare(temp[right], temp[left])) values[i] = temp[right++];
            else values[i] = temp[left++];
        }
    }
    stats.merges++;
}

template <typename T, typename Compare>
void mergeSortRange(vector<T>& values, vector<T>& temp, size_t start,
                    size_t end, Compare& compare, SortStats& stats) {
    if (end - start < 2) return;
    size_t middle = start + (end - start) / 2;
    mergeSortRange(values, temp, start, middle, compare, stats);
    mergeSortRange(values, temp, middle, end, compare, stats);
    mergeParts(values, temp, start, middle, end, compare, stats);
}

template <typename T, typename Compare>
SortStats mergeSort(vector<T>& values, Compare compare) {
    SortStats stats;
    if (values.size() < 2) return stats;
    vector<T> temp = values;
    mergeSortRange(values, temp, 0, values.size(), compare, stats);
    return stats;
}

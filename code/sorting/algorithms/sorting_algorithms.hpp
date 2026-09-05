#ifndef SORTING_ALGORITHMS_HPP
#define SORTING_ALGORITHMS_HPP

#include <vector>

void mergeSort(std::vector<int>& values);
void quickSort(std::vector<int>& values);
std::vector<int> patienceSort(const std::vector<int>& values);
void standardSort(std::vector<int>& values);

#endif  // SORTING_ALGORITHMS_HPP

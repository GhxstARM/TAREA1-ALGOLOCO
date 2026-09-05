// QuickSort con pivote mediana de tres y particion en tres vias.
// 3rd ed., MIT Press, 2009. https://mitpress.mit.edu/9780262033848/
// Fecha de consulta: 2026-09-04. Esta implementacion fue escrita para esta tarea.

#include "sorting_algorithms.hpp"

#include <algorithm>
#include <utility>

namespace {

int medianOfThree(const std::vector<int>& values, int low, int high) {
    const int mid = low + (high - low) / 2;
    const int first = values[low];
    const int middle = values[mid];
    const int last = values[high];
    if ((first <= middle && middle <= last) ||
        (last <= middle && middle <= first)) {
        return middle;
    }
    if ((middle <= first && first <= last) ||
        (last <= first && first <= middle)) {
        return first;
    }
    return last;
}

std::pair<int, int> partitionThreeWays(std::vector<int>& values, int low, int high) {
    const int pivot = medianOfThree(values, low, high);
    int less = low;
    int current = low;
    int greater = high;
    while (current <= greater) {
        if (values[current] < pivot) {
            std::swap(values[less++], values[current++]);
        } else if (values[current] > pivot) {
            std::swap(values[current], values[greater--]);
        } else {
            ++current;
        }
    }
    return {less, greater};
}

void quickSortRange(std::vector<int>& values, int low, int high) {
    // Solo se recurre sobre la seccion menor: pila O(log n).
    while (low < high) {
        const auto [equalBegin, equalEnd] = partitionThreeWays(values, low, high);
        if (equalBegin - low < high - equalEnd) {
            quickSortRange(values, low, equalBegin - 1);
            low = equalEnd + 1;
        } else {
            quickSortRange(values, equalEnd + 1, high);
            high = equalBegin - 1;
        }
    }
}

}  // namespace

void quickSort(std::vector<int>& values) {
    if (values.size() > 1) {
        quickSortRange(values, 0, static_cast<int>(values.size()) - 1);
    }
}

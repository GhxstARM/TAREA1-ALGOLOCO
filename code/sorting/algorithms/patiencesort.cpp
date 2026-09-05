#include "sorting_algorithms.hpp"

#include <queue>
#include <functional>

std::vector<int> patienceSort(const std::vector<int>& values) {
    std::vector<std::vector<int>> piles;
    piles.reserve(values.size()); //

    
    for (const int value : values) { //
        int low = 0;
        int high = piles.size();
        
        while (low < high) { 
            int mid = low + (high - low) / 2; 
            if (piles[mid].back() >= value) { 
                high = mid; //[cite: 3]
            } else {
                low = mid + 1; //[cite: 3]
            }
        }
        
        if (low == piles.size()) { //[cite: 3]
            piles.emplace_back(); //[cite: 3]
        }
        piles[low].push_back(value); //[cite: 3]
    }

    // Fase 2: Fusión de las pilas utilizando un min-heap
    using HeapEntry = std::pair<int, std::size_t>; //[cite: 3]
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> heap; //[cite: 3]
    
    for (std::size_t pile = 0; pile < piles.size(); ++pile) { //[cite: 3]
        heap.emplace(piles[pile].back(), pile); //[cite: 3]
    }

    std::vector<int> sorted;
    sorted.reserve(values.size()); //[cite: 3]
    
    while (!heap.empty()) { //[cite: 3]
        auto [value, pile] = heap.top(); //[cite: 3]
        heap.pop(); //[cite: 3]
        sorted.push_back(value); //[cite: 3]
        piles[pile].pop_back(); //[cite: 3]
        
        if (!piles[pile].empty()) { //[cite: 3]
            heap.emplace(piles[pile].back(), pile); //[cite: 3]
        }
    }
    return sorted; //[cite: 3]
}

#ifndef KNNRESULT_HPP
#define KNNRESULT_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "NNList.hpp"

template <typename T>
class KNNResult {
public:
    KNNResult(const std::vector<NNList<T>>& knn_lists) {
        // Flatten the list
        for (const auto& knn_list : knn_lists) {
            for (const auto& entry : knn_list) {
                knn_list_.emplace_back(entry);
            }
        }
    }

    std::vector<std::pair<uint32_t, double>> pickBest(size_t k, const std::string& method) {
        if (method == "frequency") {
            return pickBestFrequency(k);
        } else if (method == "distance") {
            return pickBestDistance(k);
        } else {
            throw std::invalid_argument("Unknown method: " + method);
        }
    }

private:
    std::vector<std::pair<uint32_t, double>> pickBestFrequency(size_t k) {
        std::unordered_map<uint32_t, size_t> freq;
        for (const auto& entry : knn_list_) {
            freq[entry.element.representative->getId()]++;
        }

        // Convert to vector of pairs and sort by frequency
        std::vector<std::pair<uint32_t, size_t>> freq_vec(freq.begin(), freq.end());
        std::sort(freq_vec.begin(), freq_vec.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        // Return the k most frequent elements
        std::vector<std::pair<uint32_t, double>> best;
        for (size_t i = 0; i < k && i < freq_vec.size(); ++i) {
            best.emplace_back(freq_vec[i].first, static_cast<double>(freq_vec[i].second));
        }
        return best;
    }

    std::vector<std::pair<uint32_t, double>> pickBestDistance(size_t k) {
        // Sort by distance
        std::sort(knn_list_.begin(), knn_list_.end(), [](const auto& a, const auto& b) {
            return a.distance < b.distance;
        });

        // Return the k closest different elements
        std::vector<std::pair<uint32_t, double>> best;
        std::unordered_set<uint32_t> seen;
        for (const auto& entry : knn_list_) {
            if (seen.find(entry.element.representative->getId()) == seen.end()) {
                best.emplace_back(entry.element.representative->getId(), entry.distance);
                seen.insert(entry.element.representative->getId());
            }
            if (best.size() == k) {
                break;
            }
        }
        return best;
    }

    // Cout
    friend std::ostream& operator<<(std::ostream& os, const KNNResult<T>& knn_result) {
        for (const auto& entry : knn_result.knn_list_) {
            os << entry.element << " " << entry.distance << "; ";
        }
        return os;
    }

    std::vector<NNEntry<T>> knn_list_;
};

#endif // KNNRESULT_HPP
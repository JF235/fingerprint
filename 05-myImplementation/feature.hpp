#ifndef FEATURE_HPP
#define FEATURE_HPP

#include <vector>
#include <iterator>
#include <cstdint> // For uint32_t
#include <iostream> // For std::ostream

/**
 * @brief Class representing a feature with a unique identifier and a vector of values.
 */
class feature {
public:
    /**
     * @brief Default constructor that initializes an empty feature.
     */
    feature() : id(0), values() {}

    /**
     * @brief Constructor that initializes a feature with an ID and a vector of values.
     * @param id Unique identifier of the feature.
     * @param vals Vector of values of the feature.
     */
    feature(uint32_t id, const std::vector<float>& vals) : id(id), values(vals) {}

    // /**
    //  * @brief Overload of the output operator to format the output as [id]:(val1, val2, val3, ...).
    //  * @param os Output stream.
    //  * @param f Feature object to be printed.
    //  * @return Output stream.
    //  */
    // friend std::ostream& operator<<(std::ostream& os, const feature& f) {
    //     os << "[" << f.id << "]:(";
    //     size_t limit = f.printLimit == 0 ? f.values.size() : std::min(f.printLimit, f.values.size());
    //     for (size_t i = 0; i < limit; ++i) {
    //         os << f.values[i];
    //         if (i < limit - 1) {
    //             os << ", ";
    //         }
    //     }
    //     if (limit < f.values.size()) {
    //         os << ", ...";
    //     }
    //     os << ")";
    //     return os;
    // }

    /**
     * @brief Overload of the output operator to format the output as id:<idval>.
     * @param os Output stream.
     * @param f Feature object to be printed.
     * @return Output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const feature& f) {
        os << "id:" << f.id;
        return os;
    }

    /**
     * @brief Sets the limit of values to be printed.
     * @param limit Maximum number of values to be printed.
     */
    void setPrintLimit(size_t limit) {
        printLimit = limit;
    }

    /**
     * @brief Returns the size of the values vector.
     * @return The size of the values vector.
     */
    size_t size() const {
        return values.size();
    }

    /**
     * @brief Overload of the [] operator to access the values of the feature.
     * @param index Index of the value to be accessed.
     * @return The value at the specified index.
     */
    float operator[](size_t index) const {
        return values[index];
    }

    /**
     * @brief Returns a constant iterator to the beginning of the values vector.
     * @return Constant iterator to the beginning of the values vector.
     */
    std::vector<float>::const_iterator begin() const {
        return values.begin();
    }

    /**
     * @brief Returns a constant iterator to the end of the values vector.
     * @return Constant iterator to the end of the values vector.
     */
    std::vector<float>::const_iterator end() const {
        return values.end();
    }

    /**
     * @brief Returns the unique identifier of the feature.
     * @return The unique identifier of the feature.
     */
    uint32_t getId() const {
        return id;
    }


    bool operator==(const feature& other) const {
        return id == other.id;
    }

private:
    uint32_t id; ///< Unique identifier
    std::vector<float> values; ///< Vector of values
    size_t printLimit = 5; ///< 0 means no limit
};

#endif // FEATURE_HPP
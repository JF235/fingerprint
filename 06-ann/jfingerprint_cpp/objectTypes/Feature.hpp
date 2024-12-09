#ifndef FEATURE_HPP
#define FEATURE_HPP

#include <vector>
#include <iterator>
#include <cstdint> // For uint32_t
#include <iostream> // For std::ostream
#include <cmath> // For std::sqrt

template <typename NumT>
class Individual;

/**
 * @brief Class representing a feature.
 * 
 * A feature is a vector of values that can be used to represent an object.
 * 
 * @tparam NumT Type of the values of the feature.
 */
template <typename NumT>
class Feature {
public:
    /**
     * @brief Default constructor that initializes an empty Feature.
     */
    Feature() : id(0), values(), representative(nullptr) {}

    /**
     * @brief Constructor that initializes a Feature with an ID and a vector of values.
     * @param id Unique identifier of the Feature.
     * @param vals Vector of values of the Feature.
     * @param rep Pointer to the representative Individual.
     */
    Feature(uint32_t id, const std::vector<NumT>& vals, Individual<NumT>* rep = nullptr) 
        : id(id), values(vals), representative(rep) {
        if (id >= nextId && id != 0) {
            nextId = id + 1;
        } else if (id < nextId && id != 0) {
            // Throw an exception if the ID is already used
            throw std::invalid_argument("ID already used");
        }
        if (representative) {
            representative->addFeature(id);
        }
    }

    /**
     * @brief Constructor that initializes a Feature with a vector of values.
     * 
     * The ID is automatically assigned.
     * 
     * @param vals Vector of values of the Feature.
     * @param rep Pointer to the representative Individual.
     */
    Feature(std::vector<NumT>& vals, Individual<NumT>* rep = nullptr) 
        : values(vals), representative(rep) {
        id = nextId++;
        if (representative) {
            representative->addFeature(id);
        }
    }

    /** 
     * @brief Other constructor for the case of rvalue reference
     * 
     * ID is automatically assigned.
     * 
     * @param vals rvalue reference to the vector of values of the Feature.
     * @param rep Pointer to the representative Individual.
     */
    Feature(std::vector<NumT>&& vals, Individual<NumT>* rep = nullptr) 
        : values(std::move(vals)), representative(rep) {
        id = nextId++;
        if (representative) {
            representative->addFeature(id);
        }
    }

    /**
     * @brief Constructor that initializes a Feature with a size.
     * 
     * The values are initialized to 0.
     * 
     * @param size Size of the Feature.
     * @param rep Pointer to the representative Individual.
     */
    Feature(size_t size, Individual<NumT>* rep = nullptr) 
        : values(size), representative(rep) {
        id = nextId++;
        if (representative) {
            representative->addFeature(id);
        }
    }

    /**
     * @brief Returns the norm of the Feature.
     * @return The norm of the Feature.
     */
    NumT norm() const {
        NumT norm = 0;
        for (auto val : values) {
            norm += val * val;
        }
        return std::sqrt(norm);
    }

    /**
     * @brief Overload of the output operator to format the output as id:<idval>.
     * @param os Output stream.
     * @param f Feature object to be printed.
     * @return Output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Feature& f) {
        os << "(id:" << f.id;
        if (f.representative != nullptr) {
            os << ", rep:" << f.representative->name << "(" << f.representative->id << ")";
        }
        os << ") ";
        return os;
    }

    void print() const {
        std::cout << id << ": ";
        for (size_t i = 0; i < values.size(); ++i) {
            std::cout << values[i] << " ";
        }
        std::cout << "\n";
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
     * @brief Overload of the [] operator to access the values of the Feature const.
     * @param index Index of the value to be accessed.
     * @return The value at the specified index.
     */
    const NumT &operator[](size_t index) const {
        return values[index];
    }

    /**
     * @brief Overload of the [] operator to access the values of the Feature.
     * @param index Index of the value to be accessed.
     * @return The value at the specified index.
     */
    NumT &operator[](size_t index) {
        return values[index];
    }

    /**
     * @brief Returns a constant iterator to the beginning of the values vector.
     * @return Constant iterator to the beginning of the values vector.
     */
    typename std::vector<NumT>::const_iterator begin() const {
        return values.begin();
    }

    /**
     * @brief Returns a constant iterator to the end of the values vector.
     * @return Constant iterator to the end of the values vector.
     */
    typename std::vector<NumT>::const_iterator end() const {
        return values.end();
    }

    /**
     * @brief Returns the unique identifier of the Feature.
     * @return The unique identifier of the Feature.
     */
    uint32_t getId() const {
        return id;
    }

    /**
     * @brief Overload of the equality operator to compare Features by their ID.
     * @param other Feature to be compared.
     * @return True if the Features have the same ID, false otherwise.
     */
    bool operator==(const Feature& other) const {
        return id == other.id;
    }

    /**
     * @brief Calculates the hash of the Feature.
     * @return The hash of the Feature.
     */
    std::size_t hash() const {
        std::size_t seed = 0;
        for (const auto& val : values) {
            seed ^= std::hash<NumT>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    uint32_t id;                ///< Unique identifier
    std::vector<NumT> values;   ///< Vector of values
    Individual<NumT>* representative; ///< Pointer to the representative Individual
private:
    size_t printLimit = 5;      ///< 0 means no limit
    static uint32_t nextId;     ///< Next unique identifier
};

// Initialize the static member
template <typename NumT>
uint32_t Feature<NumT>::nextId = 0;

namespace std {
    template <typename NumT>
    struct hash<Feature<NumT>> {
        std::size_t operator()(const Feature<NumT>& f) const {
            return f.hash();
        }
    };
}


#endif // FEATURE_HPP
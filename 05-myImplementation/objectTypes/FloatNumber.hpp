#ifndef FLOATNUMBER_HPP
#define FLOATNUMBER_HPP

#include <stdexcept> // For std::out_of_range
#include <iostream> // For std::ostream

// Create a class with float number
class FloatNumber {
public:
    FloatNumber(float value = 0.0) : value(value) {
        oid = nextOid++;
    }

    float getValue() const {
        return value;
    }

    size_t size() const {
        return 1;
    }

    // Overload of index access operator (non-const version)
    float operator[](size_t index) {
        if (index == 0) {
            return value;
        } else {
            throw std::out_of_range("Index out of range");
        }
    }

    // Overload of index access operator (const version)
    float operator[](size_t index) const {
        if (index == 0) {
            return value;
        } else {
            throw std::out_of_range("Index out of range");
        }
    }

    // Get the OID of the instance
    size_t getOid() const {
        return oid;
    }

    // Overload of plus operator
    FloatNumber operator+(const FloatNumber& other) const {
        return FloatNumber(value + other.value);
    }

    // Overload of minus operator
    FloatNumber operator-(const FloatNumber& other) const {
        return FloatNumber(value - other.value);
    }

    // Overload of multiplication operator
    FloatNumber operator*(const FloatNumber& other) const {
        return FloatNumber(value * other.value);
    }

    // Overload of division operator
    FloatNumber operator/(const FloatNumber& other) const {
        return FloatNumber(value / other.value);
    }

    // Overload of equal operator
    bool operator==(const FloatNumber& other) const {
        return value == other.value;
    }

    // Overload of not equal operator
    bool operator!=(const FloatNumber& other) const {
        return value != other.value;
    }

    // Overload of less than operator
    bool operator<(const FloatNumber& other) const {
        return value < other.value;
    }

    // Overload of greater than operator
    bool operator>(const FloatNumber& other) const {
        return value > other.value;
    }

    // Overload of less than or equal operator
    bool operator<=(const FloatNumber& other) const {
        return value <= other.value;
    }

    // Overload of greater than or equal operator
    bool operator>=(const FloatNumber& other) const {
        return value >= other.value;
    }

    // Overload of output stream operator
    friend std::ostream& operator<<(std::ostream& os, const FloatNumber& obj) {
        os << obj.value;
        return os;
    }

private:
    float value;
    size_t oid;
    static size_t nextOid;
};

// Initialize the static member variable
size_t FloatNumber::nextOid = 1;

#endif // FLOATNUMBER_HPP
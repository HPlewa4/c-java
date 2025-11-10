#ifndef ACTIVATION_FUNCTION_H
#define ACTIVATION_FUNCTION_H

#include <memory>
#include <ostream>

// Enum class for activation types
enum class ActivationType {
    SIGMOID,
    RELU,
    TANH,
    SOFTMAX,
    LINEAR
};

// Operator<< for ActivationType to support stream output
inline std::ostream& operator<<(std::ostream& os, const ActivationType& type) {
    switch (type) {
        case ActivationType::SIGMOID: return os << "SIGMOID";
        case ActivationType::RELU: return os << "RELU";
        case ActivationType::TANH: return os << "TANH";
        case ActivationType::SOFTMAX: return os << "SOFTMAX";
        case ActivationType::LINEAR: return os << "LINEAR";
        default: return os << "UNKNOWN";
    }
}

// Abstract base class for polymorphism - demonstrates virtual methods
class ActivationFunction {
public:
    virtual ~ActivationFunction() = default;

    // Pure virtual methods - must be overridden
    virtual double activate(double x) const = 0;
    virtual double derivative(double x) const = 0;
    virtual ActivationType getType() const = 0;
};

// Factory for creating activation functions
class ActivationFactory {
public:
    static std::unique_ptr<ActivationFunction> create(ActivationType type);
};

#endif

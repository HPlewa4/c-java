#ifndef RELU_ACTIVATION_H
#define RELU_ACTIVATION_H

#include "activation_function.h"
#include <algorithm>

// Concrete class - inheritance and polymorphism
class ReLUActivation : public ActivationFunction {
public:
    double activate(double x) const override {
        return std::max(0.0, x);
    }

    double derivative(double x) const override {
        return x > 0.0 ? 1.0 : 0.0;
    }

    ActivationType getType() const override {
        return ActivationType::RELU;
    }
};

#endif

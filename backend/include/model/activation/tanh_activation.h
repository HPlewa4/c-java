#ifndef TANH_ACTIVATION_H
#define TANH_ACTIVATION_H

#include "activation_function.h"
#include <cmath>

// Concrete class - inheritance and polymorphism
class TanhActivation : public ActivationFunction {
public:
    double activate(double x) const override {
        return std::tanh(x);
    }

    double derivative(double x) const override {
        return 1.0 - x * x;
    }

    ActivationType getType() const override {
        return ActivationType::TANH;
    }
};

#endif

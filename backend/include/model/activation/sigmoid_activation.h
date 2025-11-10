#ifndef SIGMOID_ACTIVATION_H
#define SIGMOID_ACTIVATION_H

#include "activation_function.h"
#include <cmath>

// Concrete class - inheritance and polymorphism
class SigmoidActivation : public ActivationFunction {
public:
    double activate(double x) const override {
        return 1.0 / (1.0 + std::exp(-x));
    }

    double derivative(double x) const override {
        return x * (1.0 - x);
    }

    ActivationType getType() const override {
        return ActivationType::SIGMOID;
    }
};

#endif

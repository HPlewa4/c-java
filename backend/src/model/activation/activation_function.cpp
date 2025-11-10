#include "activation_function.h"
#include "sigmoid_activation.h"
#include "relu_activation.h"
#include "tanh_activation.h"

// Factory method implementation - demonstrates polymorphism
std::unique_ptr<ActivationFunction> ActivationFactory::create(ActivationType type) {
    switch (type) {
        case ActivationType::SIGMOID:
            return std::make_unique<SigmoidActivation>();
        case ActivationType::RELU:
            return std::make_unique<ReLUActivation>();
        case ActivationType::TANH:
            return std::make_unique<TanhActivation>();
        default:
            return std::make_unique<SigmoidActivation>();
    }
}

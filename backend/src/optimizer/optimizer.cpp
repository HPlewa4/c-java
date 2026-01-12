#include "../../include/optimizer/optimizer.h"
#include "../../include/optimizer/adam_optimizer.h"

void SGDOptimizer::update_1d(std::vector<double>& params,
                             const std::vector<double>& grads) {
    for (size_t i = 0; i < params.size(); i++) {
        params[i] -= learning_rate * grads[i];
    }
}

void SGDOptimizer::update_2d(std::vector<std::vector<double>>& params,
                             const std::vector<std::vector<double>>& grads) {
    for (size_t i = 0; i < params.size(); i++) {
        for (size_t j = 0; j < params[i].size(); j++) {
            params[i][j] -= learning_rate * grads[i][j];
        }
    }
}

void SGDOptimizer::update_4d(
    std::vector<std::vector<std::vector<std::vector<double>>>>& params,
    const std::vector<std::vector<std::vector<std::vector<double>>>>& grads) {

    for (size_t oc = 0; oc < params.size(); oc++) {
        for (size_t ic = 0; ic < params[oc].size(); ic++) {
            for (size_t kh = 0; kh < params[oc][ic].size(); kh++) {
                for (size_t kw = 0; kw < params[oc][ic][kh].size(); kw++) {
                    params[oc][ic][kh][kw] -= learning_rate * grads[oc][ic][kh][kw];
                }
            }
        }
    }
}

std::unique_ptr<Optimizer> OptimizerFactory::create(OptimizerType type, double lr) {
    switch (type) {
        case OptimizerType::SGD:
            return std::make_unique<SGDOptimizer>(lr);
        case OptimizerType::ADAM:
            return std::make_unique<AdamOptimizer>(lr);
        default:
            return std::make_unique<SGDOptimizer>(lr);
    }
}

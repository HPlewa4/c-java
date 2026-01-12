#include "../../include/optimizer/adam_optimizer.h"
#include <iostream>

AdamOptimizer::AdamOptimizer(double lr, double beta1, double beta2, double epsilon)
    : Optimizer(lr), beta1(beta1), beta2(beta2), epsilon(epsilon), timestep(0) {
}

void AdamOptimizer::initialize_1d(const std::vector<double>& params) {
    m_1d.resize(params.size(), 0.0);
    v_1d.resize(params.size(), 0.0);
    initialized_1d = true;
}

void AdamOptimizer::initialize_2d(const std::vector<std::vector<double>>& params) {
    m_2d.resize(params.size());
    v_2d.resize(params.size());
    for (size_t i = 0; i < params.size(); i++) {
        m_2d[i].resize(params[i].size(), 0.0);
        v_2d[i].resize(params[i].size(), 0.0);
    }
    initialized_2d = true;
}

void AdamOptimizer::initialize_4d(
    const std::vector<std::vector<std::vector<std::vector<double>>>>& params) {
    m_4d.resize(params.size());
    v_4d.resize(params.size());
    for (size_t oc = 0; oc < params.size(); oc++) {
        m_4d[oc].resize(params[oc].size());
        v_4d[oc].resize(params[oc].size());
        for (size_t ic = 0; ic < params[oc].size(); ic++) {
            m_4d[oc][ic].resize(params[oc][ic].size());
            v_4d[oc][ic].resize(params[oc][ic].size());
            for (size_t kh = 0; kh < params[oc][ic].size(); kh++) {
                m_4d[oc][ic][kh].resize(params[oc][ic][kh].size(), 0.0);
                v_4d[oc][ic][kh].resize(params[oc][ic][kh].size(), 0.0);
            }
        }
    }
    initialized_4d = true;
}

void AdamOptimizer::update_1d(std::vector<double>& params,
                              const std::vector<double>& grads) {
    if (!initialized_1d) {
        initialize_1d(params);
    }

    timestep++;

    double bias_correction1 = 1.0 - std::pow(beta1, timestep);
    double bias_correction2 = 1.0 - std::pow(beta2, timestep);

    for (size_t i = 0; i < params.size(); i++) {
        m_1d[i] = beta1 * m_1d[i] + (1.0 - beta1) * grads[i];

        v_1d[i] = beta2 * v_1d[i] + (1.0 - beta2) * grads[i] * grads[i];

        double m_hat = m_1d[i] / bias_correction1;
        double v_hat = v_1d[i] / bias_correction2;

        params[i] -= learning_rate * m_hat / (std::sqrt(v_hat) + epsilon);
    }
}

void AdamOptimizer::update_2d(std::vector<std::vector<double>>& params,
                              const std::vector<std::vector<double>>& grads) {
    if (!initialized_2d) {
        initialize_2d(params);
    }

    timestep++;

    double bias_correction1 = 1.0 - std::pow(beta1, timestep);
    double bias_correction2 = 1.0 - std::pow(beta2, timestep);

    static bool debug_once = true;

    for (size_t i = 0; i < params.size(); i++) {
        for (size_t j = 0; j < params[i].size(); j++) {
            m_2d[i][j] = beta1 * m_2d[i][j] + (1.0 - beta1) * grads[i][j];

            v_2d[i][j] = beta2 * v_2d[i][j] + (1.0 - beta2) * grads[i][j] * grads[i][j];

            double m_hat = m_2d[i][j] / bias_correction1;
            double v_hat = v_2d[i][j] / bias_correction2;

            if (debug_once && i == 0 && j == 0) {
                std::cout << "  [DEBUG Adam] grad=" << grads[i][j]
                          << " m=" << m_2d[i][j] << " v=" << v_2d[i][j]
                          << " m_hat=" << m_hat << " v_hat=" << v_hat
                          << " update=" << (learning_rate * m_hat / (std::sqrt(v_hat) + epsilon)) << std::endl;
            }

            params[i][j] -= learning_rate * m_hat / (std::sqrt(v_hat) + epsilon);
        }
    }
    debug_once = false;
}

void AdamOptimizer::update_4d(
    std::vector<std::vector<std::vector<std::vector<double>>>>& params,
    const std::vector<std::vector<std::vector<std::vector<double>>>>& grads) {

    if (!initialized_4d) {
        initialize_4d(params);
    }

    timestep++;

    double bias_correction1 = 1.0 - std::pow(beta1, timestep);
    double bias_correction2 = 1.0 - std::pow(beta2, timestep);

    for (size_t oc = 0; oc < params.size(); oc++) {
        for (size_t ic = 0; ic < params[oc].size(); ic++) {
            for (size_t kh = 0; kh < params[oc][ic].size(); kh++) {
                for (size_t kw = 0; kw < params[oc][ic][kh].size(); kw++) {
                    double grad = grads[oc][ic][kh][kw];

                    m_4d[oc][ic][kh][kw] = beta1 * m_4d[oc][ic][kh][kw] +
                                           (1.0 - beta1) * grad;

                    v_4d[oc][ic][kh][kw] = beta2 * v_4d[oc][ic][kh][kw] +
                                           (1.0 - beta2) * grad * grad;

                    double m_hat = m_4d[oc][ic][kh][kw] / bias_correction1;
                    double v_hat = v_4d[oc][ic][kh][kw] / bias_correction2;

                    params[oc][ic][kh][kw] -= learning_rate * m_hat /
                                              (std::sqrt(v_hat) + epsilon);
                }
            }
        }
    }
}

void AdamOptimizer::reset() {
    timestep = 0;
    initialized_1d = false;
    initialized_2d = false;
    initialized_4d = false;
    m_1d.clear();
    m_2d.clear();
    m_4d.clear();
    v_1d.clear();
    v_2d.clear();
    v_4d.clear();
}
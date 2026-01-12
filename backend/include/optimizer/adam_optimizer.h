#ifndef ADAM_OPTIMIZER_H
#define ADAM_OPTIMIZER_H

#include "optimizer.h"
#include <cmath>

class AdamOptimizer : public Optimizer {
private:
    double beta1;
    double beta2;
    double epsilon;
    int timestep;

    std::vector<double> m_1d;
    std::vector<std::vector<double>> m_2d;
    std::vector<std::vector<std::vector<std::vector<double>>>> m_4d;

    std::vector<double> v_1d;
    std::vector<std::vector<double>> v_2d;
    std::vector<std::vector<std::vector<std::vector<double>>>> v_4d;

    bool initialized_1d = false;
    bool initialized_2d = false;
    bool initialized_4d = false;

    void initialize_1d(const std::vector<double>& params);
    void initialize_2d(const std::vector<std::vector<double>>& params);
    void initialize_4d(const std::vector<std::vector<std::vector<std::vector<double>>>>& params);

public:
    explicit AdamOptimizer(double lr = 0.001,
                          double beta1 = 0.9,
                          double beta2 = 0.999,
                          double epsilon = 1e-8);

    void update_1d(std::vector<double>& params,
                   const std::vector<double>& grads) override;

    void update_2d(std::vector<std::vector<double>>& params,
                   const std::vector<std::vector<double>>& grads) override;

    void update_4d(
        std::vector<std::vector<std::vector<std::vector<double>>>>& params,
        const std::vector<std::vector<std::vector<std::vector<double>>>>& grads) override;

    void reset() override;
};

#endif
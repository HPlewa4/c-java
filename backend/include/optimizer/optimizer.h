#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <memory>

class Optimizer {
protected:
    double learning_rate;

public:
    explicit Optimizer(double lr) : learning_rate(lr) {}
    virtual ~Optimizer() = default;

    virtual void update_1d(std::vector<double>& params,
                           const std::vector<double>& grads) = 0;

    virtual void update_2d(std::vector<std::vector<double>>& params,
                           const std::vector<std::vector<double>>& grads) = 0;

    virtual void update_4d(
        std::vector<std::vector<std::vector<std::vector<double>>>>& params,
        const std::vector<std::vector<std::vector<std::vector<double>>>>& grads) = 0;

    virtual void reset() = 0;

    double get_learning_rate() const { return learning_rate; }

    void set_learning_rate(double lr) { learning_rate = lr; }
};

class SGDOptimizer : public Optimizer {
public:
    explicit SGDOptimizer(double lr = 0.001) : Optimizer(lr) {}

    void update_1d(std::vector<double>& params,
                   const std::vector<double>& grads) override;

    void update_2d(std::vector<std::vector<double>>& params,
                   const std::vector<std::vector<double>>& grads) override;

    void update_4d(
        std::vector<std::vector<std::vector<std::vector<double>>>>& params,
        const std::vector<std::vector<std::vector<std::vector<double>>>>& grads) override;

    void reset() override {}
};

enum class OptimizerType {
    SGD,
    ADAM
};

class OptimizerFactory {
public:
    static std::unique_ptr<Optimizer> create(OptimizerType type, double lr);
};

#endif
#include "svm.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>

void main(void)
{
    int N = 15000;
    int P = 3;
    std::vector<svm_node> x_nodes(N * (P + 1));
    std::vector<svm_node*> x(N, nullptr);
    std::vector<double> y(N, 1.0);

    std::mt19937 gen(1234);
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    auto node = &x_nodes[0];
    for (int n = 0; n < N; n++)
    {
        x[n] = node;
        for (int p = 0; p < P; p++)
        {
            node->index = p + 1;
            node->value = dis(gen);
            ++node;
        }
        node->index = -1;
        ++node;
    }
    svm_problem prob;
    prob.l = N;
    prob.x = &x[0];
    prob.y = &y[0];

    svm_parameter param;
    param.svm_type = ONE_CLASS;
    param.kernel_type = RBF;
    param.degree = 3;
    param.gamma = 1.0 / P;
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 4000;
    param.C = 1;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 0;
    param.probability = 0;
    param.nr_weight = 0;
    param.weight_label = nullptr;
    param.weight = nullptr;

    auto tic = std::chrono::system_clock::now();
    auto model = svm_train(&prob, &param);
    auto toc = std::chrono::system_clock::now();
    std::cout << "SVM training takes " << std::chrono::duration<double>(toc - tic).count() << " sec\n";

    svm_free_and_destroy_model(&model);

}

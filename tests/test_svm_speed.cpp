#include "svm.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>

struct Problem
{
    std::vector<svm_node> x_nodes;
    std::vector<svm_node*> x;
    std::vector<double> y;
    svm_problem prob;
};

Problem create_problem(int N, int P)
{
    Problem problem;
    problem.x_nodes.resize(N * (P + 1));
    problem.x.assign(N, nullptr);
    problem.y.assign(N, 0.0);

    std::mt19937 gen(1234);
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    auto node = &problem.x_nodes[0];
    for (int n = 0; n < N; n++)
    {
        problem.x[n] = node;
        for (int p = 0; p < P; p++)
        {
            node->index = p + 1;
            node->value = dis(gen);
            ++node;
        }
        node->index = -1;
        ++node;
    }
    problem.prob.l = N;
    problem.prob.x = &problem.x[0];
    problem.prob.y = &problem.y[0];
    return problem;
}

svm_parameter default_parameter(int P)
{
    svm_parameter param;
    param.kernel_type = RBF;
    param.degree = 3;
    param.gamma = 1.0 / P;
    param.coef0 = 0.5;
    param.degree = 3;
    param.nu = 0.5;
    param.cache_size = 4000;
    param.C = 1.0;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 0;
    param.probability = 0;
    param.nr_weight = 0;
    param.weight_label = nullptr;
    param.weight = nullptr;
    return param;
}

void test_svc()
{
    int N = 15000;
    int P = 3;
    auto problem = create_problem(N, P);

    std::mt19937 gen(1234);
    std::uniform_int_distribution<> dis(0, 1);
    for (int n = 0; n < N; n++)
        problem.y[n] = dis(gen);

    auto param = default_parameter(P);
    param.svm_type = NU_SVC;

    auto tic = std::chrono::system_clock::now();
    auto model = svm_train(&problem.prob, &param);
    auto toc = std::chrono::system_clock::now();
    std::cout << "SVC training takes " << std::chrono::duration<double>(toc - tic).count() << " sec\n";

    svm_free_and_destroy_model(&model);
}

void test_one_class_svm()
{
    int N = 15000;
    int P = 3;
    auto problem = create_problem(N, P);

    auto param = default_parameter(P);
    param.svm_type = ONE_CLASS;

    auto tic = std::chrono::system_clock::now();
    auto model = svm_train(&problem.prob, &param);
    auto toc = std::chrono::system_clock::now();
    std::cout << "One-class SVM training takes " << std::chrono::duration<double>(toc - tic).count() << " sec\n";

    svm_free_and_destroy_model(&model);
}

void test_svr()
{
    int N = 15000;
    int P = 3;
    auto problem = create_problem(N, P);

    std::mt19937 gen(1234);
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    for (int n = 0; n < N; n++)
        problem.y[n] = dis(gen);

    auto param = default_parameter(P);
    param.svm_type = NU_SVR;

    auto tic = std::chrono::system_clock::now();
    auto model = svm_train(&problem.prob, &param);
    auto toc = std::chrono::system_clock::now();
    std::cout << "SVR training takes " << std::chrono::duration<double>(toc - tic).count() << " sec\n";

    svm_free_and_destroy_model(&model);
}

int main()
{
    test_one_class_svm();
}

#ifndef SVM_INTEL_H
#define SVM_INTEL_H

#define USE_SVM_INTEL

#ifdef USE_SVM_INTEL

#include "svm.h"

#include <vector>
#include <memory>

typedef float Qfloat;
typedef double Dfloat;
typedef signed char schar;

class Cache
{
public:
    Cache(int l, size_t size);

    // request data [0,len)
    // return some position p where [p,len) need to be filled
    // (p >= len if nothing needs to be filled)
    int get_data(int index, Qfloat **data, int len);
    void swap_index(int i, int j);

private:
    struct Entry
    {
        Qfloat *data;
        int len;
    };

    std::vector<Qfloat> buffer_;
    std::vector<Entry> entries_;
    size_t stride_;
    size_t l_;
};


class QMatrix {
public:
    virtual Qfloat *get_Q(int column, int len) const = 0;
    virtual const double *get_QD() const = 0;
    virtual void swap_index(int i, int j) const = 0;
    virtual ~QMatrix() {}
};


class Kernel : public QMatrix {
public:
    Kernel(int l, svm_node * const * x, const svm_parameter& param);
    virtual ~Kernel() = default;

    static double k_function(const svm_node *x,
                             const svm_node *y,
                             const svm_parameter& param);

    virtual Qfloat *get_Q(int column, int len) const override;
    virtual const double *get_QD() const override;
    virtual void swap_index(int i, int j) const override;

private:
    static double dot(const svm_node *px, const svm_node *py);

    const int kernel_type;
    const int degree;
    const Dfloat gamma;
    const Dfloat coef0;

    const int l_;
    const bool same_type_;

    std::unique_ptr<Cache> cache;
    std::vector<Dfloat> x_buffer_;
    size_t x_stride_;
    int num_features_;
    std::vector<double> diag_;
    std::vector<Dfloat> x2_;
    std::unique_ptr<std::vector<Dfloat> > scratch_;
};


class SVC_Q : public Kernel
{
public:
    SVC_Q(const svm_problem& prob, const svm_parameter& param, const schar *y_);
};


class ONE_CLASS_Q : public Kernel
{
public:
    ONE_CLASS_Q(const svm_problem& prob, const svm_parameter& param);
};


class SVR_Q : public Kernel
{
public:
    SVR_Q(const svm_problem& prob, const svm_parameter& param);
};


#endif  // USE_SVM_INTEL
#endif  // SVM_INTEL_H

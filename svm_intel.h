#ifndef SVM_INTEL_H
#define SVM_INTEL_H

#define USE_SVM_INTEL

#ifdef USE_SVM_INTEL

#include "svm.h"

typedef float Qfloat;
typedef signed char schar;

class Cache
{
public:
    Cache(int l, long int size);
    ~Cache();

    // request data [0,len)
    // return some position p where [p,len) need to be filled
    // (p >= len if nothing needs to be filled)
    int get_data(const int index, float **data, int len);
    void swap_index(int i, int j);
private:
};


class QMatrix {
public:
    virtual Qfloat *get_Q(int column, int len) const = 0;
    virtual double *get_QD() const = 0;
    virtual void swap_index(int i, int j) const = 0;
    virtual ~QMatrix() {}
};


class Kernel : public QMatrix {
public:
    Kernel(int l, svm_node * const * x, const svm_parameter& param);
    virtual ~Kernel();

    static double k_function(const svm_node *x,
                             const svm_node *y,
                             const svm_parameter& param);

    virtual Qfloat *get_Q(int column, int len) const override;
    virtual double *get_QD() const override;
    virtual void swap_index(int i, int j) const override;
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

#ifndef SVM_INTEL_H
#define SVM_INTEL_H

#define USE_SVM_INTEL 1

#if USE_SVM_INTEL != 0

#include "svm.h"
#include <ipps.h>

#include <vector>
#include <memory>

typedef float Qfloat;
typedef double Dfloat;
typedef signed char schar;

template<class T>
class Buffer
{
public:
    Buffer() {}
    Buffer(size_t len)
        : len_(len)
    {
        data_ = reinterpret_cast<T*>(ippsMalloc_8u_L(len * sizeof(T)));
    }
    Buffer(Buffer<T> &&other)
        : data_(other.data_)
        , len_(other.len_)
    {
        other.data_ = nullptr;
        other.len_ = 0;
    }

    ~Buffer()
    {
        if (data_ != nullptr)
            ippsFree(data_);
    }

    Buffer& operator=(Buffer<T> &&other)
    {
        std::swap(data_, other.data_);
        std::swap(len_, other.len_);
        return *this;
    }

    operator const T*() const { return data_; }
    operator T*() { return data_; }

    size_t size() const { return len_; }

private:
    T *data_{ nullptr };
    size_t len_{ 0 };
};

class Cache
{
public:
    Cache(int l, size_t size);
    ~Cache();

    // request data [0,len)
    // return some position p where [p,len) need to be filled
    // (p >= len if nothing needs to be filled)
    int get_data(int index, Qfloat **data, int len);
    void swap_index(int i, int j);

private:
    struct Entry;
    struct CacheLine
    {
        CacheLine *prev;
        CacheLine *next;
        Entry *entry;
        Qfloat *data;
    };
    union Data
    {
        Qfloat *data;
        CacheLine *cache;
    };
    struct Entry
    {
        Data data;
        int len;
    };

    Buffer<Qfloat> buffer_;
    std::vector<Entry> entries_;
    std::vector<CacheLine> cache_lines_;
    CacheLine *first_line_{ nullptr };
    CacheLine *last_line_{ nullptr };
    size_t stride_;
    size_t l_;
    size_t num_hits_{ 0 };
    size_t num_misses_{ 0 };
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

    virtual Qfloat *get_Q(int column, int len) const override = 0;
    virtual const double *get_QD() const override;
    virtual void swap_index(int i, int j) const override;

protected:
    void compute_kernel(Qfloat *data, int column, int pos, int len) const;
    std::unique_ptr<Cache> cache;

private:
    static double dot(const svm_node *px, const svm_node *py);

    const int kernel_type;
    const int degree;
    const Dfloat gamma;
    const Dfloat coef0;

    const int l_;
    const bool same_type_;

    Buffer<Dfloat> x_buffer_;
    size_t x_stride_;
    int num_features_;
    Buffer<double> diag_;
    Buffer<Dfloat> x2_;
    std::unique_ptr<Buffer<Dfloat> > scratch_;
};


class SVC_Q : public Kernel
{
public:
    SVC_Q(const svm_problem& prob, const svm_parameter& param, const schar *y_);
    Qfloat *get_Q(int column, int len) const override;

private:
    const int l_;
    Buffer<Qfloat> y_plus_;
    Buffer<Qfloat> y_minus_;
    std::unique_ptr<Buffer<Qfloat> > scratch_;
};


class ONE_CLASS_Q : public Kernel
{
public:
    ONE_CLASS_Q(const svm_problem& prob, const svm_parameter& param);
    Qfloat *get_Q(int column, int len) const override;
};


class SVR_Q : public Kernel
{
public:
    SVR_Q(const svm_problem& prob, const svm_parameter& param);
    Qfloat *get_Q(int column, int len) const override;
    const double *get_QD() const override;

public:
    const int l_;
    Buffer<double> QD_;
    std::unique_ptr<Buffer<Qfloat> > buffers_[2];
    mutable int buffer_index_;
};


#endif  // USE_SVM_INTEL
#endif  // SVM_INTEL_H

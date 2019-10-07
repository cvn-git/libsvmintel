#include "svm_intel.h"

#ifdef USE_SVM_INTEL

#include "svm_intel_math.h"

#include <algorithm>
#include <iostream>

#define SVM_ERROR(msg) {std::cout << __FILE__ << "(" << __LINE__ << "): ERROR: " << (msg) << std::endl; exit(-1);}

const size_t aligned_bytes = 64;

static size_t compute_aligned_stride(size_t len, size_t ele_size)
{
    return (size_t(std::ceil((len * ele_size) / double(aligned_bytes))) * aligned_bytes) / ele_size;
}

Cache::Cache(int l, size_t size) : l_(l)
{
    stride_ = compute_aligned_stride(l, sizeof(Qfloat));
    if ((stride_ * l * sizeof(Qfloat)) > size)
        SVM_ERROR("Not enough cache memory");

    buffer_.resize(l * stride_);
    entries_.resize(l);
    for (int k = 0; k < l; k++)
    {
        auto &entry = entries_[k];
        entry.data = &buffer_[k * stride_];
        entry.len = 0;
    }
}

int Cache::get_data(int index, Qfloat **data, int len)
{
    if ((index < 0) || (index >= l_))
        SVM_ERROR("Invalid index");
    auto &entry = entries_[index];
    *data = entry.data;
    auto prev_len = entry.len;
    entry.len = std::max(entry.len, len);
    return prev_len;
}

Kernel::Kernel(int l, svm_node * const * x, const svm_parameter& param)
    : kernel_type(param.kernel_type)
    , degree(param.degree)
    , gamma(param.gamma)
    , coef0(param.coef0)
    , l_(l)
    , same_type_(std::is_same<Dfloat, Qfloat>::value)
    , cache(new Cache(l, size_t(std::round(param.cache_size * 1e6))))
{
    // Count features
    num_features_ = 0;
    for (int point = 0; point < l; point++)
    {
        auto node = x[point];
        while (node->index > 0)
        {
            if (num_features_ < node->index)
                num_features_ = node->index;
            ++node;
        }
    }
    //std::cout << num_features_ << " feature(s)\n";

    // Allocate x in dense format
    x_stride_ = compute_aligned_stride(l, sizeof(Dfloat));
    x_buffer_.assign(x_stride_ * num_features_, Dfloat(0));

    // Populate x values in dense format
    for (int point = 0; point < l; point++)
    {
        auto node = x[point];
        while (node->index > 0)
        {
            x_buffer_[point + ((node->index - 1) * x_stride_)] = static_cast<Dfloat>(node->value);
            ++node;
        }
    }

    // Precompute kernel matrix's diagonal
    diag_.resize(l);
    for (int point = 0; point < l; point++)
        diag_[point] = k_function(x[point], x[point], param);

    // Precompute x^2
    if (kernel_type == RBF)
    {
        x2_.resize(l);
        for (int k = 0; k < l; k++)
            x2_[k] = static_cast<Dfloat>(dot(x[k], x[k]));
    }

    // Scratch memory for kernel computation
    if (!same_type_)
        scratch_.reset(new std::vector<Dfloat>(l));
}

Qfloat* Kernel::get_Q(int column, int len) const
{
    Qfloat *data = nullptr;
    auto pos = cache->get_data(column, &data, len);
    auto num_points = len - pos;
    if (num_points > 0)
    {
        Dfloat *out_ptr = (same_type_) ? reinterpret_cast<Dfloat*>(data + pos) : scratch_->data();
        auto x_ptr = &x_buffer_[pos];
        auto xi_ptr = &x_buffer_[column];
        auto stride = int(x_stride_);
        switch (kernel_type)
        {
        case LINEAR:
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            break;
        case POLY:
            SVM_ERROR("Not implemented");
            break;
        case RBF:
        {
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            Dfloat xi2 = x2_[column];
            for (int k = 0; k < num_points; k++)
                out_ptr[k] = std::exp(-gamma * (x2_[k + pos] + xi2 - 2 * out_ptr[k]));
            break;
        }
        case SIGMOID:
            SVM_ERROR("Not implemented");
            break;
        default:
            SVM_ERROR("Invalid kernel type");
            break;
        }

        if (!same_type_)
        {
            for (int k = 0; k < num_points; k++)
                data[k + pos] = static_cast<Qfloat>(out_ptr[k]);
        }
    }

    return data;
}

const double* Kernel::get_QD() const
{
    return &diag_[0];
}

void Kernel::swap_index(int i, int j) const
{
    SVM_ERROR("Not implemented");
}

SVC_Q::SVC_Q(const svm_problem& prob, const svm_parameter& param, const schar *y_)
    : Kernel(prob.l, prob.x, param)
{
    SVM_ERROR("Not implemented");
}

ONE_CLASS_Q::ONE_CLASS_Q(const svm_problem& prob, const svm_parameter& param)
    : Kernel(prob.l, prob.x, param)
{
}

SVR_Q::SVR_Q(const svm_problem& prob, const svm_parameter& param)
    : Kernel(prob.l, prob.x, param)
{
    SVM_ERROR("Not implemented");
}

#endif  // USE_SVM_INTEL

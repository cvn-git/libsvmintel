#include "svm_intel.h"

#if USE_SVM_INTEL != 0

#include "svm_intel_math.h"

#include <cmath>
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
    entries_.resize(l);

    stride_ = compute_aligned_stride(l, sizeof(Qfloat));
    if ((stride_ * l * sizeof(Qfloat)) > size)
    {
        auto num_lines = static_cast<int>(size / (stride_ * sizeof(Qfloat)));
        if (num_lines < 2)
            SVM_ERROR("Not enough caching memory");
        //std::cout << num_lines << " cache lines allocated\n";
        buffer_ = Buffer<Qfloat>(num_lines * stride_);
        cache_lines_.resize(num_lines);
        first_line_ = &cache_lines_[0];
        last_line_ = &cache_lines_[num_lines - 1];
        for (int k = 0; k < num_lines; k++)
        {
            auto &line = cache_lines_[k];
            line.prev = (k == 0) ? nullptr : &cache_lines_[k - 1];
            line.next = (k < (num_lines - 1)) ? &cache_lines_[k + 1] : nullptr;
            line.entry = &entries_[k];
            line.data = &buffer_[k * stride_];

            auto &entry = entries_[k];
            entry.data.cache = &line;
            entry.len = 0;
        }
        for (int k = num_lines; k < l; k++)
        {
            auto &entry = entries_[k];
            entry.data.cache = nullptr;
            entry.len = 0;
        }
    }
    else
    {
        buffer_ = Buffer<Qfloat>(l * stride_);
        for (int k = 0; k < l; k++)
        {
            auto &entry = entries_[k];
            entry.data.data = &buffer_[k * stride_];
            entry.len = 0;
        }
    }
}

Cache::~Cache()
{
    //std::cout << "SVM cache: " << num_hits_ << " hits, " << num_misses_ << " misses\n";
#if 0
    int cnt = 0;
    auto node = first_line_;
    while (node != nullptr)
    {
        cnt++;
        node = node->next;
    }
    std::cout << cnt << " SVM cache lines deallocated\n";
#endif
}

int Cache::get_data(int index, Qfloat **data, int len)
{
    if ((index < 0) || (index >= l_))
        SVM_ERROR("Invalid index");
    auto &entry = entries_[index];

    if (cache_lines_.empty())
    {
        *data = entry.data.data;
    }
    else
    {
        auto &line = entry.data.cache;
        if (line == nullptr)
        {
            // No data cached
            entry.len = 0;

            // Replace the oldest line
            auto old_line = first_line_;
            first_line_ = old_line->next;
            first_line_->prev = nullptr;
            old_line->entry->data.cache = nullptr;
            line = old_line;
            line->entry = &entry;
        }
        else
        {
            // Remove this line from the chain
            auto prev_line = line->prev;
            auto next_line = line->next;
            if (prev_line == nullptr)
                first_line_ = next_line;
            else
                prev_line->next = next_line;
            if (next_line == nullptr)
                last_line_ = prev_line;
            else
                next_line->prev = prev_line;
        }

        *data = line->data;

        // Push this line to the end
        line->next = nullptr;
        line->prev = last_line_;
        last_line_->next = line;
        last_line_ = line;
    }

    auto prev_len = entry.len;
    if (prev_len < len)
    {
        entry.len = len;
        ++num_misses_;
    }
    else
    {
        ++num_hits_;
    }
    return prev_len;
}

Kernel::Kernel(int l, svm_node * const * x, const svm_parameter& param)
    : cache(new Cache(l, size_t(std::round(param.cache_size * 1e6))))
    , kernel_type(param.kernel_type)
    , degree(param.degree)
    , gamma(static_cast<Dfloat>(param.gamma))
    , coef0(static_cast<Dfloat>(param.coef0))
    , l_(l)
    , same_type_(std::is_same<Dfloat, Qfloat>::value)
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
    x_buffer_ = Buffer<Dfloat>(x_stride_ * num_features_);
    ippsZero(x_buffer_, static_cast<int>(x_buffer_.size()));

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
    diag_ = Buffer<double>(l);
    for (int point = 0; point < l; point++)
        diag_[point] = k_function(x[point], x[point], param);

    // Precompute x^2
    if (kernel_type == RBF)
    {
        x2_ = Buffer<Dfloat>(l);
        for (int k = 0; k < l; k++)
            x2_[k] = static_cast<Dfloat>(dot(x[k], x[k]));
    }

    // Scratch memory for kernel computation
    if (!same_type_)
        scratch_.reset(new Buffer<Dfloat>(l));
}

void Kernel::compute_kernel(Qfloat *data, int column, int pos, int len) const
{
    auto num_points = len - pos;
    if (num_points > 0)
    {
        Dfloat *out_ptr = (same_type_) ? reinterpret_cast<Dfloat*>(data + pos) : *scratch_;
        auto x_ptr = &x_buffer_[pos];
        auto xi_ptr = &x_buffer_[column];
        auto stride = int(x_stride_);
        switch (kernel_type)
        {
        case LINEAR:
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            break;
        case POLY:
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            ippsMulC_I(gamma, out_ptr, num_points);
            ippsAddC_I(coef0, out_ptr, num_points);
            compute_poly_powx<Dfloat, Qfloat>(out_ptr, static_cast<Dfloat>(degree), num_points);
            break;
        case RBF:
        {
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            ippsMulC_I(Dfloat(-2), out_ptr, num_points);
            ippsAdd_I(&x2_[pos], out_ptr, num_points);
            ippsAddC_I(x2_[column], out_ptr, num_points);
            ippsMulC_I(-gamma, out_ptr, num_points);
            ippsExp_I(out_ptr, num_points);
            break;
        }
        case SIGMOID:
            compute_dot(out_ptr, x_ptr, stride, xi_ptr, stride, num_features_, num_points);
            ippsMulC_I(gamma, out_ptr, num_points);
            ippsAddC_I(coef0, out_ptr, num_points);
            compute_sigmoid_tanh<Dfloat, Qfloat>(out_ptr, num_points);
            break;
        default:
            SVM_ERROR("Invalid kernel type");
            break;
        }

        if (!same_type_)
            ippsCopy(&out_ptr[pos], &data[pos], num_points);
    }
}

const double* Kernel::get_QD() const
{
    return &diag_[0];
}

void Kernel::swap_index(int i, int j) const
{
    SVM_ERROR("Not implemented yet. Please disable shrinking feature.");
}

SVC_Q::SVC_Q(const svm_problem& prob, const svm_parameter& param, const schar *y_)
    : Kernel(prob.l, prob.x, param)
    , l_(prob.l)
    , y_plus_(prob.l)
    , y_minus_(prob.l)
    , scratch_(new Buffer<Qfloat>(prob.l))
{
    for (int k = 0; k < l_; k++)
    {
        auto val = static_cast<Qfloat>(y_[k]);
        y_plus_[k] = val;
        y_minus_[k] = -val;
    }
}

Qfloat* SVC_Q::get_Q(int column, int len) const
{
    Qfloat *data = nullptr;
    auto pos = cache->get_data(column, &data, len);
    auto num_points = len - pos;
    if (num_points > 0)
    {
        compute_kernel(data, column, pos, len);
        if (y_plus_[column] < 0)
            ippsMul_I(&y_minus_[pos], &data[pos], num_points);
        else
            ippsMul_I(&y_plus_[pos], &data[pos], num_points);
    }
    return data;
}

ONE_CLASS_Q::ONE_CLASS_Q(const svm_problem& prob, const svm_parameter& param)
    : Kernel(prob.l, prob.x, param)
{
}

Qfloat* ONE_CLASS_Q::get_Q(int column, int len) const
{
    Qfloat *data = nullptr;
    auto pos = cache->get_data(column, &data, len);
    compute_kernel(data, column, pos, len);
    return data;
}

SVR_Q::SVR_Q(const svm_problem& prob, const svm_parameter& param)
    : Kernel(prob.l, prob.x, param)
    , l_(prob.l)
    , QD_(prob.l * 2)
    , buffer_index_(0)
{
    for (int k = 0; k < 2; k++)
        buffers_[k].reset(new Buffer<Qfloat>(l_ * 2));

    auto qd = Kernel::get_QD();
    ippsCopy(qd, &QD_[0], prob.l);
    ippsCopy(qd, &QD_[prob.l], prob.l);
}

Qfloat* SVR_Q::get_Q(int column, int) const
{
    Qfloat *data = nullptr;
    int kernel_column = column % l_;
    auto pos = cache->get_data(kernel_column, &data, l_);
    compute_kernel(data, kernel_column, pos, l_);

    Qfloat *output = *buffers_[buffer_index_];
    buffer_index_ = 1 - buffer_index_;
    Qfloat sign = (column < l_) ? Qfloat(1) : Qfloat(-1);
    ippsMulC(data, sign, output, l_);
    ippsMulC(data, -sign, output + l_, l_);

    return output;
}

const double* SVR_Q::get_QD() const
{
    return &QD_[0];
}

#endif  // USE_SVM_INTEL

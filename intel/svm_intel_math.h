#ifndef SVM_INTEL_MATH_H
#define SVM_INTEL_MATH_H

#include <ipps.h>
#include <ippvm.h>

static inline IppStatus ippsAdd_I(const Ipp32f* pSrc, Ipp32f* pSrcDst, int len)
{
    return ippsAdd_32f_I(pSrc, pSrcDst, len);
}

static inline IppStatus ippsAdd_I(const Ipp64f* pSrc, Ipp64f* pSrcDst, int len)
{
    return ippsAdd_64f_I(pSrc, pSrcDst, len);
}

static inline IppStatus ippsAddC(const Ipp32f* pSrc, Ipp32f val, Ipp32f* pDst, int len)
{
    return ippsAddC_32f(pSrc, val, pDst, len);
}

static inline IppStatus ippsAddC(const Ipp64f* pSrc, Ipp64f val, Ipp64f* pDst, int len)
{
    return ippsAddC_64f(pSrc, val, pDst, len);
}

static inline IppStatus ippsAddC_I(Ipp32f val, Ipp32f* pSrcDst, int len)
{
    return ippsAddC_32f_I(val, pSrcDst, len);
}

static inline IppStatus ippsAddC_I(Ipp64f val, Ipp64f* pSrcDst, int len)
{
    return ippsAddC_64f_I(val, pSrcDst, len);
}

static inline IppStatus ippsAddProductC(const Ipp32f* pSrc, const Ipp32f val, Ipp32f* pSrcDst, int len)
{
    return ippsAddProductC_32f(pSrc, val, pSrcDst, len);
}

static inline IppStatus ippsAddProductC(const Ipp64f* pSrc, const Ipp64f val, Ipp64f* pSrcDst, int len)
{
    return ippsAddProductC_64f(pSrc, val, pSrcDst, len);
}

static inline IppStatus ippsCopy(const Ipp64f* pSrc, Ipp64f* pDst, int len)
{
    return ippsCopy_64f(pSrc, pDst, len);
}

static inline IppStatus ippsCopy(const Ipp32f* pSrc, Ipp64f* pDst, int len)
{
    return ippsConvert_32f64f(pSrc, pDst, len);
}

static inline IppStatus ippsExp_I(Ipp32f* pSrcDst, int len)
{
    return ippsExp_32f_I(pSrcDst, len);
}

static inline IppStatus ippsExp_I(Ipp64f* pSrcDst, int len)
{
    return ippsExp_64f_I(pSrcDst, len);
}

static inline IppStatus ippsMulC(const Ipp32f* pSrc, Ipp32f val, Ipp32f* pDst, int len)
{
    return ippsMulC_32f(pSrc, val, pDst, len);
}

static inline IppStatus ippsMulC(const Ipp64f* pSrc, Ipp64f val, Ipp64f* pDst, int len)
{
    return ippsMulC_64f(pSrc, val, pDst, len);
}

static inline IppStatus ippsMulC_I(Ipp32f val, Ipp32f* pSrcDst, int len)
{
    return ippsMulC_32f_I(val, pSrcDst, len);
}

static inline IppStatus ippsMulC_I(Ipp64f val, Ipp64f* pSrcDst, int len)
{
    return ippsMulC_64f_I(val, pSrcDst, len);
}

static inline IppStatus ippsPowx(const Ipp32f* pSrc1, const Ipp32f ConstValue, Ipp32f* pDst, Ipp32s len)
{
    return ippsPowx_32f_A24(pSrc1, ConstValue, pDst, len);
}

static inline IppStatus ippsPowx(const Ipp64f* pSrc1, const Ipp64f ConstValue, Ipp64f* pDst, Ipp32s len)
{
    return ippsPowx_64f_A53(pSrc1, ConstValue, pDst, len);
}

static inline IppStatus ippsTanh(const Ipp32f* pSrc, Ipp32f* pDst, Ipp32s len)
{
    return ippsTanh_32f_A24(pSrc, pDst, len);
}

static inline IppStatus ippsTanh(const Ipp64f* pSrc, Ipp64f* pDst, Ipp32s len)
{
    return ippsTanh_64f_A53(pSrc, pDst, len);
}


template<typename T>
static inline void compute_dot(T *output, const T *x, int x_stride, const T *xi, int xi_stride, int num_features, int len)
{
    ippsMulC(x, *xi, output, len);
    for (int feature = 1; feature < num_features; feature++)
    {
        x += x_stride;
        xi += xi_stride;
        ippsAddProductC(x, *xi, output, len);
    }
}

template<class TD, class TQ>
static inline void compute_poly_powx(TD *data, TD degree, int len)
{
    ippsPowx(data, degree, data, len);
}

template<>
static inline void compute_poly_powx<double, float>(double *data, double degree, int len)
{
    ippsPowx_64f_A26(data, degree, data, len);
}

template<class TD, class TQ>
static inline void compute_sigmoid_tanh(TD *data, int len)
{
    ippsTanh(data, data, len);
}

template<>
static inline void compute_sigmoid_tanh<double, float>(double *data, int len)
{
    ippsTanh_64f_A26(data, data, len);
}

#endif

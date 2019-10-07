#ifndef SVM_INTEL_MATH_H
#define SVM_INTEL_MATH_H

#include <ipps.h>

static inline IppStatus ippsAdd_I(const Ipp32f* pSrc, Ipp32f* pSrcDst, int len)
{
    return ippsAdd_32f_I(pSrc, pSrcDst, len);
}

static inline IppStatus ippsAdd_I(const Ipp64f* pSrc, Ipp64f* pSrcDst, int len)
{
    return ippsAdd_64f_I(pSrc, pSrcDst, len);
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

#endif

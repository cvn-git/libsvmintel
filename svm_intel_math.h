#ifndef SVM_INTEL_MATH_H
#define SVM_INTEL_MATH_H


static inline void compute_dot(double *output, const double *x, int x_stride, const double *xi, int xi_stride, int num_features, int len)
{
    for (int point = 0; point < len; point++)
        output[point] = 0.0;

    for (int feature = 0; feature < num_features; feature++)
    {
        auto xi_val = *xi;
        for (int point = 0; point < len; point++)
            output[point] += x[point] * xi_val;

        x += x_stride;
        xi += xi_stride;
    }
}

#endif

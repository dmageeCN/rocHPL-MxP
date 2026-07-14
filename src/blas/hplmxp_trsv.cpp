
#include "hplmxp.hpp"

template <>
void HPLMXP_trsvU(const int m, const double* A, const int lda, double* x) {
  HIPBLAS_CHECK(hipblasDtrsv(blas_hdl,
                             HIPBLAS_FILL_MODE_UPPER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_NON_UNIT,
                             m,
                             A,
                             lda,
                             x,
                             1));
}

template <>
void HPLMXP_trsvL(const int m, const double* A, const int lda, double* x) {
  HIPBLAS_CHECK(hipblasDtrsv(blas_hdl,
                             HIPBLAS_FILL_MODE_LOWER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_UNIT,
                             m,
                             A,
                             m,
                             x,
                             1));
}

template <>
void HPLMXP_trsvU(const int m, const float* A, const int lda, float* x) {
  HIPBLAS_CHECK(hipblasStrsv(blas_hdl,
                             HIPBLAS_FILL_MODE_UPPER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_NON_UNIT,
                             m,
                             A,
                             lda,
                             x,
                             1));
}

template <>
void HPLMXP_trsvL(const int m, const float* A, const int lda, float* x) {
  HIPBLAS_CHECK(hipblasStrsv(blas_hdl,
                             HIPBLAS_FILL_MODE_LOWER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_UNIT,
                             m,
                             A,
                             m,
                             x,
                             1));
}

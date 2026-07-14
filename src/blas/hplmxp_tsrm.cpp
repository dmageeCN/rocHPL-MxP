
#include "hplmxp.hpp"

template <>
void HPLMXP_trsmR(const int     m,
                  const int     n,
                  const double  alpha,
                  const double* a,
                  const int     lda,
                  double*       b,
                  const int     ldb) {
  HIPBLAS_CHECK(hipblasDtrsm(blas_hdl,
                             HIPBLAS_SIDE_RIGHT,
                             HIPBLAS_FILL_MODE_UPPER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_NON_UNIT,
                             m,
                             n,
                             &alpha,
                             a,
                             lda,
                             b,
                             ldb));
}

template <>
void HPLMXP_trsmR(const int    m,
                  const int    n,
                  const float  alpha,
                  const float* a,
                  const int    lda,
                  float*       b,
                  const int    ldb) {
  HIPBLAS_CHECK(hipblasStrsm(blas_hdl,
                             HIPBLAS_SIDE_RIGHT,
                             HIPBLAS_FILL_MODE_UPPER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_NON_UNIT,
                             m,
                             n,
                             &alpha,
                             a,
                             lda,
                             b,
                             ldb));
}

template <>
void HPLMXP_trsmL(const int     m,
                  const int     n,
                  const double  alpha,
                  const double* a,
                  const int     lda,
                  double*       b,
                  const int     ldb) {
  HIPBLAS_CHECK(hipblasDtrsm(blas_hdl,
                             HIPBLAS_SIDE_LEFT,
                             HIPBLAS_FILL_MODE_LOWER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_UNIT,
                             m,
                             n,
                             &alpha,
                             a,
                             lda,
                             b,
                             ldb));
}

template <>
void HPLMXP_trsmL(const int    m,
                  const int    n,
                  const float  alpha,
                  const float* a,
                  const int    lda,
                  float*       b,
                  const int    ldb) {
  HIPBLAS_CHECK(hipblasStrsm(blas_hdl,
                             HIPBLAS_SIDE_LEFT,
                             HIPBLAS_FILL_MODE_LOWER,
                             HIPBLAS_OP_N,
                             HIPBLAS_DIAG_UNIT,
                             m,
                             n,
                             &alpha,
                             a,
                             lda,
                             b,
                             ldb));
}

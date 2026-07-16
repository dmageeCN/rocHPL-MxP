
#include "hplmxp.hpp"

template <>
void HPLMXP_trsmR(const int     m,
                  const int     n,
                  const double  alpha,
                  const double* a,
                  const int     lda,
                  double*       b,
                  const int     ldb) {
  CUBLAS_CHECK(cublasDtrsm(blas_hdl,
                            CUBLAS_SIDE_RIGHT,
                            CUBLAS_FILL_MODE_UPPER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_NON_UNIT,
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
  CUBLAS_CHECK(cublasStrsm(blas_hdl,
                            CUBLAS_SIDE_RIGHT,
                            CUBLAS_FILL_MODE_UPPER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_NON_UNIT,
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
  CUBLAS_CHECK(cublasDtrsm(blas_hdl,
                            CUBLAS_SIDE_LEFT,
                            CUBLAS_FILL_MODE_LOWER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_UNIT,
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
  CUBLAS_CHECK(cublasStrsm(blas_hdl,
                            CUBLAS_SIDE_LEFT,
                            CUBLAS_FILL_MODE_LOWER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_UNIT,
                            m,
                            n,
                            &alpha,
                            a,
                            lda,
                            b,
                            ldb));
}

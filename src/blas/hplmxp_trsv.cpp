
#include "hplmxp.hpp"

template <>
void HPLMXP_trsvU(const int m, const double* A, const int lda, double* x) {
  CUBLAS_CHECK(cublasDtrsv(blas_hdl,
                            CUBLAS_FILL_MODE_UPPER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_NON_UNIT,
                            m,
                            A,
                            lda,
                            x,
                            1));
}

template <>
void HPLMXP_trsvL(const int m, const double* A, const int lda, double* x) {
  CUBLAS_CHECK(cublasDtrsv(blas_hdl,
                            CUBLAS_FILL_MODE_LOWER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_UNIT,
                            m,
                            A,
                            m,
                            x,
                            1));
}

template <>
void HPLMXP_trsvU(const int m, const float* A, const int lda, float* x) {
  CUBLAS_CHECK(cublasStrsv(blas_hdl,
                            CUBLAS_FILL_MODE_UPPER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_NON_UNIT,
                            m,
                            A,
                            lda,
                            x,
                            1));
}

template <>
void HPLMXP_trsvL(const int m, const float* A, const int lda, float* x) {
  CUBLAS_CHECK(cublasStrsv(blas_hdl,
                            CUBLAS_FILL_MODE_LOWER,
                            CUBLAS_OP_N,
                            CUBLAS_DIAG_UNIT,
                            m,
                            A,
                            m,
                            x,
                            1));
}

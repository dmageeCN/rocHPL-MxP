
#include "hplmxp.hpp"

// cuSOLVER's getrf requires an explicit device workspace buffer. Query the
// required size and lazily grow a cached, per-type workspace buffer,
// reallocating only when a larger buffer is ever needed.
template <typename T>
static T* HPLMXP_getrf_workspace(const int size) {
  static T*  work      = nullptr;
  static int work_size = 0;

  if(size > work_size) {
    if(work) CUDA_CHECK(cudaFree(work));
    CUDA_CHECK(cudaMalloc(&work, sizeof(T) * (size_t)size));
    work_size = size;
  }
  return work;
}

// Passing devIpiv == nullptr requests a no-pivot LU factorization from
// cuSOLVER. Note cusolverDn*getrf() does not take an explicit lwork
// argument - only *getrf_bufferSize() does; the workspace buffer itself is
// sized according to that queried value.
template <>
void HPLMXP_getrf(const int m, const int n, float* a, const int lda) {
  int lwork = 0;
  CUSOLVER_CHECK(cusolverDnSgetrf_bufferSize(solver_hdl, m, n, a, lda, &lwork));
  float* work = HPLMXP_getrf_workspace<float>(lwork);

  CUSOLVER_CHECK(
      cusolverDnSgetrf(solver_hdl, m, n, a, lda, work, nullptr, blas_info));
}

template <>
void HPLMXP_getrf(const int m, const int n, double* a, const int lda) {
  int lwork = 0;
  CUSOLVER_CHECK(cusolverDnDgetrf_bufferSize(solver_hdl, m, n, a, lda, &lwork));
  double* work = HPLMXP_getrf_workspace<double>(lwork);

  CUSOLVER_CHECK(
      cusolverDnDgetrf(solver_hdl, m, n, a, lda, work, nullptr, blas_info));
}

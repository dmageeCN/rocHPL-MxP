
#include "hplmxp.hpp"

// hipSOLVER's getrf requires an explicit device workspace buffer (unlike
// rocSOLVER's rocsolver_Xgetrf_npvt, which manages its own scratch space
// internally). Query the required size and lazily grow a cached, per-type
// workspace buffer, reallocating only when a larger buffer is ever needed.
template <typename T>
static T* HPLMXP_getrf_workspace(const int size) {
  static T*  work      = nullptr;
  static int work_size = 0;

  if(size > work_size) {
    if(work) HIP_CHECK(hipFree(work));
    HIP_CHECK(hipMalloc(&work, sizeof(T) * (size_t)size));
    work_size = size;
  }
  return work;
}

// Passing devIpiv == nullptr requests a no-pivot LU factorization, which is
// portable across both the AMD (rocSOLVER) and NVIDIA (cuSOLVER) hipSOLVER
// backends.
template <>
void HPLMXP_getrf(const int m, const int n, float* a, const int lda) {
  int lwork = 0;
  HIPSOLVER_CHECK(hipsolverSgetrf_bufferSize(solver_hdl, m, n, a, lda, &lwork));
  float* work = HPLMXP_getrf_workspace<float>(lwork);

  HIPSOLVER_CHECK(hipsolverSgetrf(
      solver_hdl, m, n, a, lda, work, lwork, nullptr, blas_info));
}

template <>
void HPLMXP_getrf(const int m, const int n, double* a, const int lda) {
  int lwork = 0;
  HIPSOLVER_CHECK(hipsolverDgetrf_bufferSize(solver_hdl, m, n, a, lda, &lwork));
  double* work = HPLMXP_getrf_workspace<double>(lwork);

  HIPSOLVER_CHECK(hipsolverDgetrf(
      solver_hdl, m, n, a, lda, work, lwork, nullptr, blas_info));
}

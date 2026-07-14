
#include "hplmxp.hpp"

// Neither hipBLAS nor hipSOLVER expose a portable triangular matrix inverse
// (hipblasDtrtri exists but is unimplemented on the NVIDIA backend, and
// hipSOLVER has no trtri at all). Instead, invert A by solving a single
// triangular system against an identity matrix, using only the already
// portable HPLMXP_trsmR/HPLMXP_trsmL primitives:
//
//   HPLMXP_trtriU: X * U = I  (trsmR, upper, non-unit) => X = inv(U)
//   HPLMXP_trtriL: L * X = I  (trsmL, lower, unit)     => X = inv(L)
//
// With an exact identity right-hand side, standard column-by-column TRSM
// back-substitution produces an exact-zero result in the non-referenced
// triangle, so only the relevant triangle of the result is copied back
// into A, leaving the other triangle (holding the other factor) untouched.

template <typename T>
static T* HPLMXP_trtri_workspace(const int m) {
  static T*  work    = nullptr;
  static int work_ld = 0;

  if(m > work_ld) {
    if(work) HIP_CHECK(hipFree(work));
    HIP_CHECK(hipMalloc(&work, sizeof(T) * (size_t)m * (size_t)m));
    work_ld = m;
  }
  return work;
}

template <>
void HPLMXP_trtriU(const int m, float* A, const int lda) {
  if(m <= 0) return;
  float* work = HPLMXP_trtri_workspace<float>(m);

  HPLMXP_identity(m, work, m);
  HPLMXP_trsmR(m, m, 1.0f, A, lda, work, m);
  HPLMXP_lacpyU(m, m, work, m, A, lda);
}

template <>
void HPLMXP_trtriU(const int m, double* A, const int lda) {
  if(m <= 0) return;
  double* work = HPLMXP_trtri_workspace<double>(m);

  HPLMXP_identity(m, work, m);
  HPLMXP_trsmR(m, m, 1.0, A, lda, work, m);
  HPLMXP_lacpyU(m, m, work, m, A, lda);
}

template <>
void HPLMXP_trtriL(const int m, float* A, const int lda) {
  if(m <= 0) return;
  float* work = HPLMXP_trtri_workspace<float>(m);

  HPLMXP_identity(m, work, m);
  HPLMXP_trsmL(m, m, 1.0f, A, lda, work, m);
  HPLMXP_lacpyL(m, m, work, m, A, lda);
}

template <>
void HPLMXP_trtriL(const int m, double* A, const int lda) {
  if(m <= 0) return;
  double* work = HPLMXP_trtri_workspace<double>(m);

  HPLMXP_identity(m, work, m);
  HPLMXP_trsmL(m, m, 1.0, A, lda, work, m);
  HPLMXP_lacpyL(m, m, work, m, A, lda);
}

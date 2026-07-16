/* ---------------------------------------------------------------------
 * -- High Performance Computing Linpack Benchmark (HPL)
 *    HPL - 2.2 - February 24, 2016
 *    Antoine P. Petitet
 *    University of Tennessee, Knoxville
 *    Innovative Computing Laboratory
 *    (C) Copyright 2000-2008 All Rights Reserved
 *
 *    Modified by: Noel Chalmers
 *    (C) 2018-2022 Advanced Micro Devices, Inc.
 *    See the rocHPL/LICENCE file for details.
 *
 *    SPDX-License-Identifier: (BSD-3-Clause)
 * ---------------------------------------------------------------------
 */
#ifndef HPLMXP_BLAS_HPP
#define HPLMXP_BLAS_HPP

#include <cublas_v2.h>
#include <cusolverDn.h>

#define CUDA_CHECK(val) cudaCheck((val), #val, __FILE__, __LINE__)
inline void cudaCheck(cudaError_t       err,
                      const char* const func,
                      const char* const file,
                      const int         line) {
  if(err != cudaSuccess) {
    fprintf(
        stderr,
        "Error: CUDA runtime error in file %s, line %d, error code: %s, %s\n",
        file,
        line,
        cudaGetErrorString(err),
        func);
    exit(err);
  }
}

#define CUBLAS_CHECK(val) cublasCheck((val), #val, __FILE__, __LINE__)
inline void cublasCheck(cublasStatus_t    err,
                        const char* const func,
                        const char* const file,
                        const int         line) {
  if(err != CUBLAS_STATUS_SUCCESS) {
    fprintf(stderr,
            "Error: cublas error in file %s, line %d, error code: %s, %s\n",
            file,
            line,
            cublasGetStatusString(err),
            func);
    exit(err);
  }
}

// cuSOLVER has no equivalent of cublasGetStatusString, so the raw status
// code is printed instead.
#define CUSOLVER_CHECK(val) cusolverCheck((val), #val, __FILE__, __LINE__)
inline void cusolverCheck(cusolverStatus_t  err,
                          const char* const func,
                          const char* const file,
                          const int         line) {
  if(err != CUSOLVER_STATUS_SUCCESS) {
    fprintf(stderr,
            "Error: cusolver error in file %s, line %d, error code: %d, %s\n",
            file,
            line,
            static_cast<int>(err),
            func);
    exit(static_cast<int>(err));
  }
}

extern cublasHandle_t   blas_hdl;
extern cusolverDnHandle_t solver_hdl;
extern cudaStream_t     computeStream;
extern int*             blas_info;

extern cudaEvent_t getrf, lbcast, ubcast;
extern cudaEvent_t piv;
extern cudaEvent_t DgemmStart, DgemmEnd, LgemmStart, LgemmEnd, UgemmStart,
    UgemmEnd, TgemmStart, TgemmEnd;

#define REDUCTION_SCRATCH_SIZE 512
extern fp64_t* reduction_scratch;
extern fp64_t* h_reduction_scratch;

template <typename T, typename U>
void HPLMXP_gemv(const int m,
                 const int n,
                 const T   alpha,
                 const U*  A,
                 const int lda,
                 const T*  x,
                 const T   beta,
                 T*        y);

template <typename T>
void HPLMXP_trsvU(const int m, const T* A, const int lda, T* x);

template <typename T>
void HPLMXP_trsvL(const int m, const T* A, const int lda, T* x);

template <typename T>
void HPLMXP_getrf(const int m, const int n, T* a, const int lda);

template <typename T>
void HPLMXP_trtriU(const int m, T* A, const int lda);

template <typename T>
void HPLMXP_trtriL(const int m, T* A, const int lda);

template <typename T>
void HPLMXP_trsmR(const int m,
                  const int n,
                  const T   alpha,
                  const T*  a,
                  const int lda,
                  T*        b,
                  const int ldb);

template <typename T>
void HPLMXP_trsmL(const int m,
                  const int n,
                  const T   alpha,
                  const T*  a,
                  const int lda,
                  T*        b,
                  const int ldb);

template <typename T, typename U>
void HPLMXP_gemmNT(const int m,
                   const int n,
                   const int k,
                   const T   alpha,
                   const U*  a,
                   const int lda,
                   const U*  b,
                   const int ldb,
                   const T   beta,
                   T*        c,
                   const int ldc);

#endif

/* ---------------------------------------------------------------------
 * -- High Performance Computing Linpack Benchmark (HPL)
 *    Noel Chalmers
 *    (C) 2018-2022 Advanced Micro Devices, Inc.
 *    See the rocHPL/LICENCE file for details.
 *
 *    SPDX-License-Identifier: (BSD-3-Clause)
 * ---------------------------------------------------------------------
 */

#include "hplmxp.hpp"
#include <random>

cublasHandle_t     blas_hdl;
cusolverDnHandle_t solver_hdl;
cudaStream_t       computeStream;
int*               blas_info;
fp64_t*            reduction_scratch;
fp64_t*            h_reduction_scratch;

cudaEvent_t getrf, lbcast, ubcast;
cudaEvent_t piv;
cudaEvent_t DgemmStart, DgemmEnd, LgemmStart, LgemmEnd, UgemmStart, UgemmEnd,
    TgemmStart, TgemmEnd;

static char host_name[MPI_MAX_PROCESSOR_NAME];

/*
  This function finds out how many MPI processes are running on the same node
  and assigns a local rank that can be used to map a process to a device.
  This function needs to be called by all the MPI processes.
*/
void HPLMXP_InitGPU(const HPLMXP_T_grid& grid) {
  char host_name[MPI_MAX_PROCESSOR_NAME];

  int i, n, namelen, rank, nprocs;
  int dev;

  int nprow, npcol, myrow, mycol;
  HPLMXP_grid_info(grid, nprow, npcol, myrow, mycol);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  MPI_Get_processor_name(host_name, &namelen);

  int localRank = grid.local_mycol + grid.local_myrow * grid.local_npcol;
  int localSize = grid.local_npcol * grid.local_nprow;

  /* Find out how many GPUs are in the system and their device number */
  int deviceCount;
  CUDA_CHECK(cudaGetDeviceCount(&deviceCount));

  if(deviceCount < 1) {
    if(localRank == 0)
      HPLMXP_pabort(__LINE__,
                    "HPLMXP_InitGPU",
                    "Node %s found no GPUs. Is the CUDA driver loaded?",
                    host_name);
    MPI_Finalize();
    exit(1);
  }

  dev = localRank % deviceCount;

#ifdef HPLMXP_VERBOSE_PRINT
  if(rank < localSize) {
    cudaDeviceProp props;
    CUDA_CHECK(cudaGetDeviceProperties(&props, dev));

    printf("GPU  Binding: Process %d [(p,q)=(%d,%d)] GPU: %d, pciBusID %x \n",
           rank,
           grid.local_myrow,
           grid.local_mycol,
           dev,
           props.pciBusID);
  }
#endif

  /* Assign device to MPI process, initialize BLAS and probe device properties
   */
  CUDA_CHECK(cudaSetDevice(dev));

  /* gpu */
  CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&blas_info), sizeof(int)));
  CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&reduction_scratch),
                         sizeof(double) * REDUCTION_SCRATCH_SIZE));
  CUDA_CHECK(cudaMallocHost(reinterpret_cast<void**>(&h_reduction_scratch),
                            sizeof(double)));

  CUDA_CHECK(cudaStreamCreate(&computeStream));

  CUDA_CHECK(cudaEventCreateWithFlags(&getrf, cudaEventDisableTiming));
  CUDA_CHECK(cudaEventCreateWithFlags(&lbcast, cudaEventDisableTiming));
  CUDA_CHECK(cudaEventCreateWithFlags(&ubcast, cudaEventDisableTiming));
  CUDA_CHECK(cudaEventCreate(&DgemmStart));
  CUDA_CHECK(cudaEventCreate(&DgemmEnd));
  CUDA_CHECK(cudaEventCreate(&LgemmStart));
  CUDA_CHECK(cudaEventCreate(&LgemmEnd));
  CUDA_CHECK(cudaEventCreate(&UgemmStart));
  CUDA_CHECK(cudaEventCreate(&UgemmEnd));
  CUDA_CHECK(cudaEventCreate(&TgemmStart));
  CUDA_CHECK(cudaEventCreate(&TgemmEnd));
  CUDA_CHECK(cudaEventCreate(&piv));


  /* Create a cuBLAS handle */
  CUBLAS_CHECK(cublasCreate(&blas_hdl));
  CUBLAS_CHECK(cublasSetPointerMode(blas_hdl, CUBLAS_POINTER_MODE_HOST));
  CUBLAS_CHECK(cublasSetStream(blas_hdl, computeStream));

  /* Create a cuSOLVER handle */
  CUSOLVER_CHECK(cusolverDnCreate(&solver_hdl));
  CUSOLVER_CHECK(cusolverDnSetStream(solver_hdl, computeStream));
}

void HPLMXP_FreeGPU() {
  CUBLAS_CHECK(cublasDestroy(blas_hdl));
  CUSOLVER_CHECK(cusolverDnDestroy(solver_hdl));

  CUDA_CHECK(cudaEventDestroy(getrf));
  CUDA_CHECK(cudaEventDestroy(lbcast));
  CUDA_CHECK(cudaEventDestroy(ubcast));
  CUDA_CHECK(cudaEventDestroy(DgemmStart));
  CUDA_CHECK(cudaEventDestroy(DgemmEnd));
  CUDA_CHECK(cudaEventDestroy(LgemmStart));
  CUDA_CHECK(cudaEventDestroy(LgemmEnd));
  CUDA_CHECK(cudaEventDestroy(UgemmStart));
  CUDA_CHECK(cudaEventDestroy(UgemmEnd));
  CUDA_CHECK(cudaEventDestroy(TgemmStart));
  CUDA_CHECK(cudaEventDestroy(TgemmEnd));
  CUDA_CHECK(cudaEventDestroy(piv));

  CUDA_CHECK(cudaFree(reduction_scratch));
  CUDA_CHECK(cudaFree(blas_info));

  CUDA_CHECK(cudaStreamDestroy(computeStream));
}

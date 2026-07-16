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

template <typename T>
void HPLMXP_pmat_init(HPLMXP_T_grid&    grid,
                      const int         N,
                      const int         NB,
                      HPLMXP_T_pmat<T>& A) {

  const int nblocks = N / NB;

  A.n     = N;
  A.nb    = NB;
  A.nbrow = (nblocks - grid.myrow + grid.nprow - 1) / grid.nprow;
  A.nbcol = (nblocks - grid.mycol + grid.npcol - 1) / grid.npcol;

  A.mp = A.nbrow * NB;
  A.nq = A.nbcol * NB;

  A.A = nullptr;
  A.d = nullptr;
  A.b = nullptr;
  A.x = nullptr;

  A.ld = 0;

  A.norma = 0.0;
  A.normb = 0.0;
  A.res   = 0.0;

  A.piv  = nullptr;
  A.pivL = nullptr;
  A.pivU = nullptr;
  A.work = nullptr;

  A.panels[0].L = nullptr;
  A.panels[0].U = nullptr;
  A.panels[1].L = nullptr;
  A.panels[1].U = nullptr;
}

template void HPLMXP_pmat_init(HPLMXP_T_grid&         grid,
                               const int              N,
                               const int              NB,
                               HPLMXP_T_pmat<double>& A);

template void HPLMXP_pmat_init(HPLMXP_T_grid&        grid,
                               const int             N,
                               const int             NB,
                               HPLMXP_T_pmat<float>& A);

template <typename T>
void HPLMXP_pmat_free(HPLMXP_T_pmat<T>& A) {

  if(A.work) {
    CUDA_CHECK(cudaFree(A.work));
    A.work = nullptr;
  }

  HPLMXP_pdpanel_free(A.panels[1]);
  HPLMXP_pdpanel_free(A.panels[0]);

  if(A.pivU) {
    CUDA_CHECK(cudaFree(A.pivU));
    A.pivU = nullptr;
  }
  if(A.pivL) {
    CUDA_CHECK(cudaFree(A.pivL));
    A.pivL = nullptr;
  }
  if(A.piv) {
    CUDA_CHECK(cudaFree(A.piv));
    A.piv = nullptr;
  }

  if(A.b) {
    CUDA_CHECK(cudaFree(A.b));
    A.b = nullptr;
  }
  if(A.d) {
    CUDA_CHECK(cudaFree(A.d));
    A.d = nullptr;
  }
  if(A.x) {
    CUDA_CHECK(cudaFree(A.x));
    A.x = nullptr;
  }
  if(A.A) {
    CUDA_CHECK(cudaFree(A.A));
    A.A = nullptr;
  }
}

template void HPLMXP_pmat_free(HPLMXP_T_pmat<double>& A);

template void HPLMXP_pmat_free(HPLMXP_T_pmat<float>& A);

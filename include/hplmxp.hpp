
#ifndef HPLMXP_HPP
#define HPLMXP_HPP

#include <cuda_runtime_api.h>
#include <cuda_fp16.h>

#ifdef HPLMXP_TRACING
#include <nvtx3/nvToolsExt.h>
#endif

/*
 * ---------------------------------------------------------------------
 * #define types
 * ---------------------------------------------------------------------
 */
#define fp64_t double
#define fp32_t float
#define fp16_t __half

//#define HPLMXP_USE_COLLECTIVES 1

#include "hplmxp_version.hpp"
#include "hplmxp_misc.hpp"
#include "hplmxp_blas.hpp"
#include "hplmxp_auxil.hpp"
#include "hplmxp_comm.hpp"
#include "hplmxp_pauxil.hpp"
#include "hplmxp_grid.hpp"
#include "hplmxp_panel.hpp"
#include "hplmxp_pgesv.hpp"
#include "hplmxp_pmatgen.hpp"
#include "hplmxp_ptimer.hpp"
#include "hplmxp_ptest.hpp"

#endif

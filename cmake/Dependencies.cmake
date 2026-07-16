# Modifications (c) 2019-2022 Advanced Micro Devices, Inc.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# Dependencies

# Git
find_package(Git REQUIRED)

# MPI
set(MPI_HOME ${HPLMXP_MPI_DIR})
find_package(MPI REQUIRED)

# CUDA Toolkit - provides the CUDA::cudart, CUDA::cublas, CUDA::cusolver and
# CUDA::nvToolsExt imported targets used throughout the build. Unlike the
# ROCm hipblas-config.cmake/hipsolver-config.cmake packages this project
# previously relied on, FindCUDAToolkit is a standard, built-in CMake module,
# so no extra path options or manual IMPORTED_LOCATION lookups are needed -
# CMake finds it automatically alongside the CUDA language compiler.
find_package(CUDAToolkit REQUIRED)

message("-- CUDA Toolkit version:      ${CUDAToolkit_VERSION}")
message("-- CUDA Toolkit include dirs: ${CUDAToolkit_INCLUDE_DIRS}")

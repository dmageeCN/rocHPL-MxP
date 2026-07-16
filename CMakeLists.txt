# Modifications (c) 2018-2022 Advanced Micro Devices, Inc.
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

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

option(HPLMXP_VERBOSE_PRINT   "Enable printing to terminal during run" OFF)
option(HPLMXP_PROGRESS_REPORT "Enable printing progess report to terminal during run" OFF)
option(HPLMXP_DETAILED_TIMING "Enable detailed timers during run" OFF)
option(HPLMXP_TRACING         "Annotate profiler traces with NVTX markers" OFF)

option(HPLMXP_MPI_DIR  "Path to MPI library" ${CMAKE_CURRENT_SOURCE_DIR}/tpl/openmpi)

# CUDA architectures to build for. Defaults to "native" (CMake >= 3.24
# detects the architecture(s) of the GPUs present on the build machine).
# Override with e.g. -DCMAKE_CUDA_ARCHITECTURES=90 for H100.
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES "native")
endif()

set(CMAKE_INSTALL_PREFIX "rocHPL-MxP" CACHE PATH "Install path prefix, prepended onto install directories")

# CMake modules
list(APPEND CMAKE_MODULE_PATH
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# rocHPL-MxP project
project(rochplmxp VERSION 2.1.0 LANGUAGES CXX CUDA)

# Build flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Build options
option(HPLMXP_DEBUG "Compile with modest debugging turned on" OFF)
option(HPLMXP_DETAILED_DEBUG "Compile with voluminous debugging information turned on" OFF)

# Dependencies
include(cmake/Dependencies.cmake)

# This option only works for make/nmake and the ninja generators, but no reason it shouldn't be on all the time
# This tells cmake to create a compile_commands.json file that can be used with clang tooling or vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# HPL sources
file(GLOB_RECURSE rochplmxp_source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

# HPL device sources - these contain __global__ kernels or <<<>>> kernel
# launches and must be compiled by the CUDA compiler rather than the plain
# host C++ compiler, even though they keep the .cpp extension.
file(GLOB_RECURSE rochplmxp_device_source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*_device.cpp")
set_source_files_properties(${rochplmxp_device_source} PROPERTIES LANGUAGE CUDA)

# Target executable
add_executable(rochplmxp ${rochplmxp_source})

target_compile_options(rochplmxp PRIVATE
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:-O0 -g>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<NOT:$<CONFIG:Debug>>>:-O3 -march=native -Wno-deprecated-declarations>
  $<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<CONFIG:Debug>>:-G -g>
  $<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<NOT:$<CONFIG:Debug>>>:-O3 --use_fast_math>
)

if(HPLMXP_DEBUG)
  target_compile_definitions(rochplmxp PRIVATE HPLMXP_DEBUG)
endif()

if(HPLMXP_VERBOSE_PRINT)
  target_compile_definitions(rochplmxp PRIVATE HPLMXP_VERBOSE_PRINT)
endif()

if(HPLMXP_DETAILED_TIMING)
  target_compile_definitions(rochplmxp PRIVATE HPLMXP_DETAILED_TIMING)
endif()

if(HPLMXP_PROGRESS_REPORT)
  target_compile_definitions(rochplmxp PRIVATE HPLMXP_PROGRESS_REPORT)
endif()

if(HPLMXP_TRACING)
  target_compile_definitions(rochplmxp PRIVATE HPLMXP_TRACING)
endif()

# Target include directories
target_include_directories(rochplmxp
                           PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                           $<BUILD_INTERFACE:${HPLMXP_MPI_DIR}/include>)

# CUDA runtime, cuBLAS, cuSOLVER
target_link_libraries(rochplmxp PRIVATE CUDA::cudart)
target_link_libraries(rochplmxp PRIVATE CUDA::cublas)
target_link_libraries(rochplmxp PRIVATE CUDA::cusolver)

# MPI
target_link_libraries(rochplmxp PRIVATE MPI::MPI_CXX)

if(HPLMXP_TRACING)
  target_link_libraries(rochplmxp PRIVATE CUDA::nvToolsExt)
endif()

# Target properties
set_target_properties(rochplmxp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(rochplmxp PROPERTIES LINKER_LANGUAGE CXX)

# # Configure a header file to pass the rocHPL-MxP version
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/hplmxp_version.hpp.in"
               "${PROJECT_BINARY_DIR}/include/hplmxp_version.hpp")

# Configure run scripts
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_rochplmxp.in"
               "${CMAKE_BINARY_DIR}/run_rochplmxp"
               @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/mpirun_rochplmxp.in"
               "${CMAKE_BINARY_DIR}/mpirun_rochplmxp"
               @ONLY)

#move input file
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/HPL-MxP.dat
     DESTINATION ${CMAKE_BINARY_DIR})

# Install targets
install(TARGETS rochplmxp
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(PROGRAMS ${CMAKE_BINARY_DIR}/run_rochplmxp ${CMAKE_BINARY_DIR}/mpirun_rochplmxp
        DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_BINARY_DIR}/HPL-MxP.dat
        DESTINATION ${CMAKE_INSTALL_PREFIX})

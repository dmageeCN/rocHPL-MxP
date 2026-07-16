#!/usr/bin/env bash
# Author: Nico Trost
# Modified by: Noel Chalmers

#set -x #echo on

# Determine the directory this script lives in, regardless of whether it is
# sourced or executed, and regardless of bash vs zsh.
if [[ -z $THISDIR ]]; then
    THISFILE=${BASH_SOURCE[0]}
    : ${THISFILE:=$0}

    export THISDIR=$(dirname $(realpath ${THISFILE}))
fi

# #################################################
# helper functions
# #################################################
function display_help()
{
  echo "rocHPL-MxP build helper script"
  echo "./install "
  echo "    [-h|--help] prints this help message"
  echo "    [-g|--debug] Set build type to Debug (otherwise build Release)"
  echo "    [--prefix] Path to rocHPL-MxP install location (Default: build/rocHPL-MxP)"
  echo "    [--build-dir] Path to build directory (Default: ./build)"
  echo "    [--with-cuda=<dir>] Path to CUDA Toolkit install (Default: /usr/local/cuda)"
  echo "    [--with-mpi=<dir>] Path to external MPI install (Default: clone+build OpenMPI)"
  echo "    [--verbose-print] Verbose output during HPL setup (Default: on)"
  echo "    [--enable-tracing] Annotate profiler traces with NVTX markers (Default: off)"
  echo "    [--progress-report] Print progress report to terminal during HPL run (Default: on)"
  echo "    [--detailed-timing] Record detailed timers during HPL run (Default: on)"
}

# prereq: ${ID} must be defined before calling
supported_distro( )
{
  if [ -z ${ID+foo} ]; then
    printf "supported_distro(): \$ID must be set\n"
    exit 2
  fi

  case "${ID}" in
    ubuntu|centos|rhel|rocky|almalinux|fedora|sles)
        true
        ;;
    *)  printf "This script is currently supported on Ubuntu, CentOS, RHEL, Rocky, AlmaLinux, Fedora and SLES\n"
        exit 2
        ;;
  esac
}

exit_with_error( )
{
  if (( $1 == 2 )); then
    # Failure in some install step
    # Print some message about needed dependencies

    # dependencies needed for executable to build
    local library_dependencies_ubuntu=( "git" "make" "cmake" "libnuma-dev" "pkg-config" "autoconf" "libtool" "automake" "m4" "flex" "libgomp1")
    local library_dependencies_centos=( "git" "make" "cmake3" "gcc-c++" "rpm-build" "epel-release" "numactl-libs" "autoconf" "libtool" "automake" "m4" "flex" "libgomp")
    local library_dependencies_fedora=( "git" "make" "cmake" "gcc-c++" "libcxx-devel" "rpm-build" "numactl-libs"  "autoconf" "libtool" "automake" "m4" "flex" "libgomp")
    local library_dependencies_sles=(   "git" "make" "cmake" "gcc-c++" "libcxxtools9" "rpm-build" "libnuma-devel" "autoconf" "libtool" "automake" "m4" "flex" "libgomp1")

    if [[ "${with_cuda}" == /usr/local/cuda ]]; then
      library_dependencies_ubuntu+=("cuda-toolkit")
      library_dependencies_centos+=("cuda-toolkit")
      library_dependencies_fedora+=("cuda-toolkit")
      library_dependencies_sles+=("cuda-toolkit")
    fi

    printf "Installation failed. Some required packages may be missing.\n"
    printf "The following package manager install command may be needed:\n"
    case "${ID}" in
      ubuntu)
        printf "sudo apt install -y ${library_dependencies_ubuntu[*]}\n"
        ;;

      centos|rhel|rocky|almalinux)
        printf "sudo yum -y --nogpgcheck install ${library_dependencies_centos[*]}\n"
        ;;

      fedora)
        printf "sudo dnf install -y ${library_dependencies_fedora[*]}\n"
        ;;

      sles)
        printf "sudo zypper -n --no-gpg-checks install ${library_dependencies_sles[*]}\n"
        ;;
      *)
        exit 2
        ;;
    esac
  fi

  exit $1
}

check_exit_code( )
{
  if (( $? != 0 )); then
    exit $@
  fi
}

# Clone and build OpenMPI+UCX in rochpl/tpl
install_openmpi( )
{
  #OpenMPI and UCX install to one of these locations depending on OS
  ucx_lib_folder=./tpl/ucx/lib
  ompi_lib_folder=./tpl/openmpi/lib
  ucx_lib64_folder=./tpl/ucx/lib64
  ompi_lib64_folder=./tpl/openmpi/lib64

  if [ ! -d "./tpl/ucx" ]; then
    mkdir -p tpl && cd tpl
    git clone --branch v1.18.0 https://github.com/openucx/ucx.git ucx
    check_exit_code 2
    cd ucx;
    ./autogen.sh; ./autogen.sh #why do we have to run this twice?
    check_exit_code 2
    mkdir build; cd build
    ../contrib/configure-opt --prefix=${PWD}/../ --with-cuda=${with_cuda} --without-knem --without-rocm --without-java
    check_exit_code 2
    make -j$(nproc)
    check_exit_code 2
    make install
    check_exit_code 2
    cd ../../..
  elif ([ ! -f "${ucx_lib_folder}/libucm.so" ] || [ ! -f "${ucx_lib_folder}/libucp.so" ]  || \
        [ ! -f "${ucx_lib_folder}/libucs.so" ] || [ ! -f "${ucx_lib_folder}/libuct.so" ]) && \
        ([ ! -f "${ucx_lib64_folder}/libucm.so" ] || [ ! -f "${ucx_lib64_folder}/libucp.so" ]  || \
        [ ! -f "${ucx_lib64_folder}/libucs.so" ] || [ ! -f "${ucx_lib64_folder}/libuct.so" ]); then
    cd tpl/ucx;
    ./autogen.sh; ./autogen.sh
    check_exit_code 2
    mkdir build; cd build
    ../contrib/configure-opt --prefix=${PWD}/../ --with-cuda=${with_cuda} --without-knem --without-rocm --without-java
    check_exit_code 2
    make -j$(nproc)
    check_exit_code 2
    make install
    check_exit_code 2
    cd ../../..
  fi

  # Check for successful build
  if ([ ! -f "${ucx_lib_folder}/libucm.so" ] || [ ! -f "${ucx_lib_folder}/libucp.so" ]  || \
      [ ! -f "${ucx_lib_folder}/libucs.so" ] || [ ! -f "${ucx_lib_folder}/libuct.so" ]) &&
     ([ ! -f "${ucx_lib64_folder}/libucm.so" ] || [ ! -f "${ucx_lib64_folder}/libucp.so" ]  || \
      [ ! -f "${ucx_lib64_folder}/libucs.so" ] || [ ! -f "${ucx_lib64_folder}/libuct.so" ]); then
    echo "Error: UCX install unsuccessful."
    exit 3
  fi

  if [ ! -d "./tpl/openmpi" ]; then
    mkdir -p tpl && cd tpl
    git clone --branch v5.0.7 --recursive https://github.com/open-mpi/ompi.git openmpi
    check_exit_code 2
    cd openmpi; ./autogen.pl;
    check_exit_code 2
    mkdir build; cd build
    ../configure --prefix=${PWD}/../ --with-ucx=${PWD}/../../ucx --without-verbs --disable-man-pages --enable-mca-no-build=btl-uct
    check_exit_code 2
    make -j$(nproc)
    check_exit_code 2
    make install
    check_exit_code 2
    cd ../../..
  elif [ ! -f "${ompi_lib_folder}/libmpi.so" ] && [ ! -f "${ompi_lib64_folder}/libmpi.so" ]; then
    cd tpl/openmpi; ./autogen.pl;
    check_exit_code 2
    mkdir build; cd build
    ../configure --prefix=${PWD}/../ --with-ucx=${PWD}/../../ucx --without-verbs --disable-man-pages --enable-mca-no-build=btl-uct
    check_exit_code 2
    make -j$(nproc)
    check_exit_code 2
    make install
    check_exit_code 2
    cd ../../..
  fi

  # Check for successful build
  if [ ! -f "${ompi_lib_folder}/libmpi.so" ] && [ ! -f "${ompi_lib64_folder}/libmpi.so" ]; then
    echo "Error: OpenMPI install unsuccessful."
    exit_with_error 2
  fi
}

# #################################################
# Pre-requisites check
# #################################################
# Exit code 0: alls well
# Exit code 1: problems with getopt
# Exit code 2: problems with supported platforms

# check if getopt command is installed
type getopt > /dev/null
if [[ $? -ne 0 ]]; then
  echo "This script uses getopt to parse arguments; try installing the util-linux package";
  exit_with_error 1
fi

# os-release file describes the system
if [[ -e "/etc/os-release" ]]; then
  source /etc/os-release
else
  echo "This script depends on the /etc/os-release file"
  exit_with_error 1
fi

# The following function exits script if an unsupported distro is detected
supported_distro

# #################################################
# global variables
# #################################################
install_prefix=rocHPL-MxP
build_dir=./build
build_release=true
with_cuda=/usr/local/cuda
with_mpi=tpl/openmpi
verbose_print=OFF
enable_tracing=OFF
progress_report=OFF
detailed_timing=OFF

# #################################################
# Parameter parsing
# #################################################

# check if we have a modern version of getopt that can handle whitespace and long parameters
getopt -T
if [[ $? -eq 4 ]]; then
  GETOPT_PARSE=$(getopt --name "${0}" --longoptions help,debug,prefix:,build-dir:,with-cuda:,with-mpi:,verbose-print,enable-tracing,progress-report,detailed-timing --options hg -- "$@")
else
  echo "Need a new version of getopt"
  exit_with_error 1
fi

if [[ $? -ne 0 ]]; then
  echo "getopt invocation failed; could not parse the command line";
  exit_with_error 1
fi

eval set -- "${GETOPT_PARSE}"

while true; do
  case "${1}" in
    -h|--help)
        display_help
        exit 0
        ;;
    -g|--debug)
        build_release=false
        shift ;;
    --prefix)
        install_prefix=${2}
        shift 2 ;;
    --build-dir)
        build_dir=${2}
        shift 2 ;;
    --with-cuda)
        with_cuda=${2}
        shift 2 ;;
    --with-mpi)
        with_mpi=${2}
        shift 2 ;;
    --verbose-print)
        verbose_print=ON
        shift ;;
    --enable-tracing)
        enable_tracing=ON
        shift ;;
    --progress-report)
        progress_report=ON
        shift ;;
    --detailed-timing)
        detailed_timing=ON
        shift ;;
    --) shift ; break ;;
    *)  echo "Unexpected command line parameter received; aborting";
        exit_with_error 1
        ;;
  esac
done

echo "--- Creating project build directory in: ${build_dir}"
echo "--- INSTALLING HPL_MXP to: ${install_prefix}"

# #################################################
# prep
# #################################################
# ensure a clean build environment
rm -rf ${build_dir}

# Default cmake executable is called cmake
cmake_executable=cmake

# We append customary CUDA path; if user provides custom CUDA path in ${path},
# our hard-coded path has lesser priority
export CUDA_PATH=${with_cuda}
export PATH=${PATH}:${CUDA_PATH}/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CUDA_PATH}/lib:${CUDA_PATH}/lib64

pushd .
  # #################################################
  # MPI
  # #################################################
  if [[ "${with_mpi}" == tpl/openmpi ]]; then

    with_mpi=${PWD}/tpl/openmpi
    install_openmpi

  fi

  # #################################################
  # configure & build
  # #################################################
  build_type="Debug"
  if [[ "${build_release}" == true ]]; then
    build_type="Release"
  fi
  cmake_common_options="-DCMAKE_INSTALL_PREFIX=${install_prefix} -DHPLMXP_MPI_DIR=${with_mpi} -DCUDAToolkit_ROOT=${with_cuda}"
  cmake_common_options+=" -DCMAKE_BUILD_TYPE=${build_type}"
  cmake_common_options+=" -DHPLMXP_VERBOSE_PRINT=${verbose_print} -DHPLMXP_PROGRESS_REPORT=${progress_report}"
  cmake_common_options+=" -DHPLMXP_DETAILED_TIMING=${detailed_timing} -DHPLMXP_TRACING=${enable_tracing}"

  # Build library with the CUDA toolchain because of existence of device kernels
  mkdir -p ${build_dir} && cd ${build_dir}
  echo "${cmake_executable} ${cmake_common_options} ${THISDIR}"
  ${cmake_executable} ${cmake_common_options} ${THISDIR}
  check_exit_code 2

  make -j$(nproc) install
  check_exit_code 2

popd

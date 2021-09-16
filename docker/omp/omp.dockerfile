#-------------------------------------------OMP Dockerfile-------------------------------------------#
ARG base_image="intel/oneapi-basekit:devel-ubuntu18.04"
FROM intel/oneapi-hpckit

RUN apt-get update -y
 #OPEN CV
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y libopencv-dev python3-opencv
#-----------------------SPEED LOGGER-----------------------------#
RUN apt-get install -y libspdlog-dev
#----------------------Essentials---------------------------------#
RUN apt-get install -y software-properties-common ca-certificates wget curl ssh
RUN apt-get install -y apt-transport-https
RUN apt-get install -y build-essential
RUN apt-get install -y manpages-dev
RUN apt-get install -y --no-install-recommends git wget --reinstall ca-certificates \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get install -y --no-install-recommends -o=Dpkg::Use-Pty=0 git

#-----------------------------------------------OneAPI ----------------------------------------------#
#--------------------------------------------OneAPI-basekit------------------------------------------#

ARG DEBIAN_FRONTEND=noninteractive
ARG APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1

# install Intel(R) oneAPI Base Toolkit

RUN apt-get update -y && \
apt-get install -y libspdlog-dev && \
apt-get install -y --no-install-recommends -o=Dpkg::Use-Pty=0 \
intel-basekit-getting-started \
intel-oneapi-advisor \
intel-oneapi-ccl-devel \
intel-oneapi-common-licensing \
intel-oneapi-common-vars \
intel-oneapi-compiler-dpcpp-cpp \
intel-oneapi-dal-devel \
intel-oneapi-dev-utilities \
intel-oneapi-dnnl-devel \
intel-oneapi-dpcpp-debugger \
intel-oneapi-ipp-devel \
intel-oneapi-ippcp-devel \
intel-oneapi-libdpstd-devel \
intel-oneapi-mkl-devel \
intel-oneapi-onevpl-devel \
intel-oneapi-python \
intel-oneapi-tbb-devel \
intel-oneapi-vtune \
--

# reinstall newer cmake
ARG cmake_url=https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh
ADD $cmake_url /
RUN file=$(basename "$cmake_url") && \
    bash $file --prefix=/usr --skip-license && \
    rm $file

# install Intel GPU drivers
RUN echo 'deb [trusted=yes arch=amd64] https://repositories.intel.com/graphics/ubuntu bionic main' \
> /etc/apt/sources.list.d/intel-graphics.list

ARG url=https://repositories.intel.com/graphics/intel-graphics.key
ADD $url /
RUN file=$(basename "$url") && \
    apt-key add "$file" && \
    rm "$file"

RUN apt-get update -y && \
apt-get install -y --no-install-recommends -o=Dpkg::Use-Pty=0 \
intel-opencl \
intel-level-zero-gpu \
level-zero \
level-zero-devel

# setvars.sh environment variables
ENV ACL_BOARD_VENDOR_PATH="/opt/Intel/OpenCLFPGA/oneAPI/Boards"
ENV ADVISOR_2021_DIR="/opt/intel/oneapi/advisor/2021.2.0"
ENV APM="/opt/intel/oneapi/advisor/2021.2.0/perfmodels"
ENV CCL_CONFIGURATION="cpu_gpu_dpcpp"
ENV CCL_ROOT="/opt/intel/oneapi/ccl/2021.2.0"
ENV CLASSPATH="/opt/intel/oneapi/mpi/2021.2.0//lib/mpi.jar:/opt/intel/oneapi/dal/2021.2.0/lib/onedal.jar"
ENV CMAKE_PREFIX_PATH="/opt/intel/oneapi/vpl:/opt/intel/oneapi/tbb/2021.2.0/env/..:/opt/intel/oneapi/dal/2021.2.0:${CMAKE_PREFIX_PATH}"
ENV CONDA_DEFAULT_ENV="base"
ENV CONDA_EXE="/opt/intel/oneapi/intelpython/latest/bin/conda"
ENV CONDA_PREFIX="/opt/intel/oneapi/intelpython/latest"
ENV CONDA_PROMPT_MODIFIER="(base) "
ENV CONDA_PYTHON_EXE="/opt/intel/oneapi/intelpython/latest/bin/python"
ENV CONDA_SHLVL="1"
ENV CPATH="/opt/intel/oneapi/vpl/2021.2.2/include:/opt/intel/oneapi/tbb/2021.2.0/env/../include:/opt/intel/oneapi/mpi/2021.2.0//include:/opt/intel/oneapi/mkl/latest/include:/opt/intel/oneapi/ippcp/2021.2.0/include:/opt/intel/oneapi/ipp/2021.2.0/include:/opt/intel/oneapi/dpl/2021.2.0/linux/include:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/dev-utilities/2021.2.0/include:/opt/intel/oneapi/dal/2021.2.0/include:/opt/intel/oneapi/compiler/2021.2.0/linux/include:/opt/intel/oneapi/ccl/2021.2.0/include/cpu_gpu_dpcpp:${CPATH}"
ENV DAALROOT="/opt/intel/oneapi/dal/2021.2.0"
ENV DALROOT="/opt/intel/oneapi/dal/2021.2.0"
ENV DAL_MAJOR_BINARY="1"
ENV DAL_MINOR_BINARY="1"
ENV DNNLROOT="/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp"
ENV FI_PROVIDER_PATH="/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib/prov:/usr/lib64/libfabric"
ENV INFOPATH="/opt/intel/oneapi/debugger/10.1.1/documentation/info/"
ENV INTELFPGAOCLSDKROOT="/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga"
ENV INTEL_PYTHONHOME="/opt/intel/oneapi/debugger/10.1.1/dep"
ENV IPPCP_TARGET_ARCH="intel64"
ENV IPPCRYPTOROOT="/opt/intel/oneapi/ippcp/2021.2.0"
ENV IPPROOT="/opt/intel/oneapi/ipp/2021.2.0"
ENV IPP_TARGET_ARCH="intel64"
ENV I_MPI_ROOT="/opt/intel/oneapi/mpi/2021.2.0"
ENV LD_LIBRARY_PATH="/opt/intel/oneapi/vpl/2021.2.2/lib:/opt/intel/oneapi/tbb/2021.2.0/env/../lib/intel64/gcc4.8:/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib:/opt/intel/oneapi/mpi/2021.2.0//lib/release:/opt/intel/oneapi/mpi/2021.2.0//lib:/opt/intel/oneapi/mkl/latest/lib/intel64:/opt/intel/oneapi/ippcp/2021.2.0/lib/intel64:/opt/intel/oneapi/ipp/2021.2.0/lib/intel64:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/debugger/10.1.1/dep/lib:/opt/intel/oneapi/debugger/10.1.1/libipt/intel64/lib:/opt/intel/oneapi/debugger/10.1.1/gdb/intel64/lib:/opt/intel/oneapi/dal/2021.2.0/lib/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/x64:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/emu:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/host/linux64/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/linux64/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib/intel64_lin:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib:/opt/intel/oneapi/ccl/2021.2.0/lib/cpu_gpu_dpcpp:${LD_LIBRARY_PATH}"
ENV LIBRARY_PATH="/opt/intel/oneapi/vpl/2021.2.2/lib:/opt/intel/oneapi/tbb/2021.2.0/env/../lib/intel64/gcc4.8:/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib:/opt/intel/oneapi/mpi/2021.2.0//lib/release:/opt/intel/oneapi/mpi/2021.2.0//lib:/opt/intel/oneapi/mkl/latest/lib/intel64:/opt/intel/oneapi/ippcp/2021.2.0/lib/intel64:/opt/intel/oneapi/ipp/2021.2.0/lib/intel64:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/dal/2021.2.0/lib/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib/intel64_lin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib:/opt/intel/oneapi/ccl/2021.2.0/lib/cpu_gpu_dpcpp:${LIBRARY_PATH}"
ENV MANPATH="/opt/intel/oneapi/mpi/2021.2.0/man:/opt/intel/oneapi/debugger/10.1.1/documentation/man::/opt/intel/oneapi/compiler/2021.2.0/documentation/en/man/common::${MANPATH}"
ENV MKLROOT="/opt/intel/oneapi/mkl/latest"
ENV NLSPATH="/opt/intel/oneapi/mkl/latest/lib/intel64/locale/%l_%t/%N"
ENV OCL_ICD_FILENAMES="libintelocl_emu.so:libalteracl.so:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/x64/libintelocl.so"
ENV ONEAPI_ROOT="/opt/intel/oneapi"
ENV PATH="/opt/intel/oneapi/vtune/2021.2.0/bin64:/opt/intel/oneapi/vpl/2021.2.2/bin:/opt/intel/oneapi/mpi/2021.2.0//libfabric/bin:/opt/intel/oneapi/mpi/2021.2.0//bin:/opt/intel/oneapi/mkl/latest/bin/intel64:/opt/intel/oneapi/intelpython/latest/bin:/opt/intel/oneapi/intelpython/latest/condabin:/opt/intel/oneapi/dev-utilities/2021.2.0/bin:/opt/intel/oneapi/debugger/10.1.1/gdb/intel64/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/llvm/aocl-bin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/bin/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/ioc/bin:/opt/intel/oneapi/advisor/2021.2.0/bin64:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:${PATH}"
ENV PKG_CONFIG_PATH="/opt/intel/oneapi/vtune/2021.2.0/include/pkgconfig/lib64:/opt/intel/oneapi/vpl/2021.2.2/lib/pkgconfig:/opt/intel/oneapi/mkl/latest/tools/pkgconfig:/opt/intel/oneapi/advisor/2021.2.0/include/pkgconfig/lib64:${PKG_CONFIG_PATH}"
ENV PYTHONPATH="/opt/intel/oneapi/advisor/2021.2.0/pythonapi:${PYTHONPATH}"
ENV SETVARS_COMPLETED="1"
ENV SETVARS_VARS_PATH="/opt/intel/oneapi/vtune/latest/env/vars.sh"
ENV TBBROOT="/opt/intel/oneapi/tbb/2021.2.0/env/.."
ENV VPL_BIN="/opt/intel/oneapi/vpl/2021.2.2/bin"
ENV VPL_INCLUDE="/opt/intel/oneapi/vpl/2021.2.2/include"
ENV VPL_LIB="/opt/intel/oneapi/vpl/2021.2.2/lib"
ENV VPL_ROOT="/opt/intel/oneapi/vpl/2021.2.2"
ENV VTUNE_PROFILER_2021_DIR="/opt/intel/oneapi/vtune/2021.2.0"
ENV _CE_CONDA=""
ENV _CE_M=""

#-----------------------------------------------------OneAPI-hpckit----------------------------------------------------#

ARG DEBIAN_FRONTEND=noninteractive

# install Intel(R) oneAPI HPC Toolkit
RUN apt-get update -y && \
apt-get install -y --no-install-recommends -o=Dpkg::Use-Pty=0 \
intel-hpckit-getting-started \
intel-oneapi-clck \
intel-oneapi-common-licensing \
intel-oneapi-common-vars \
intel-oneapi-compiler-dpcpp-cpp-and-cpp-classic \
intel-oneapi-compiler-fortran \
intel-oneapi-dev-utilities \
intel-oneapi-inspector \
intel-oneapi-itac \
intel-oneapi-mpi-devel \
--

# setvars.sh environment variables
ENV ACL_BOARD_VENDOR_PATH="/opt/Intel/OpenCLFPGA/oneAPI/Boards"
ENV ADVISOR_2021_DIR="/opt/intel/oneapi/advisor/2021.2.0"
ENV APM="/opt/intel/oneapi/advisor/2021.2.0/perfmodels"
ENV CCL_CONFIGURATION="cpu_gpu_dpcpp"
ENV CCL_ROOT="/opt/intel/oneapi/ccl/2021.2.0"
ENV CLASSPATH="/opt/intel/oneapi/mpi/2021.2.0//lib/mpi.jar:/opt/intel/oneapi/dal/2021.2.0/lib/onedal.jar"
ENV CLCK_ROOT="/opt/intel/oneapi/clck/2021.2.0"
ENV CMAKE_PREFIX_PATH="/opt/intel/oneapi/vpl:/opt/intel/oneapi/tbb/2021.2.0/env/..:/opt/intel/oneapi/dal/2021.2.0:${CMAKE_PREFIX_PATH}"
ENV CONDA_DEFAULT_ENV="base"
ENV CONDA_EXE="/opt/intel/oneapi/intelpython/latest/bin/conda"
ENV CONDA_PREFIX="/opt/intel/oneapi/intelpython/latest"
ENV CONDA_PROMPT_MODIFIER="(base) "
ENV CONDA_PYTHON_EXE="/opt/intel/oneapi/intelpython/latest/bin/python"
ENV CONDA_SHLVL="1"
ENV CPATH="/opt/intel/oneapi/vpl/2021.2.2/include:/opt/intel/oneapi/tbb/2021.2.0/env/../include:/opt/intel/oneapi/mpi/2021.2.0//include:/opt/intel/oneapi/mkl/latest/include:/opt/intel/oneapi/ippcp/2021.2.0/include:/opt/intel/oneapi/ipp/2021.2.0/include:/opt/intel/oneapi/dpl/2021.2.0/linux/include:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/dev-utilities/2021.2.0/include:/opt/intel/oneapi/dal/2021.2.0/include:/opt/intel/oneapi/compiler/2021.2.0/linux/include:/opt/intel/oneapi/ccl/2021.2.0/include/cpu_gpu_dpcpp:${CPATH}"
ENV CPLUS_INCLUDE_PATH="/opt/intel/oneapi/clck/2021.2.0/include:${CPLUS_INCLUDE_PATH}"
ENV DAALROOT="/opt/intel/oneapi/dal/2021.2.0"
ENV DALROOT="/opt/intel/oneapi/dal/2021.2.0"
ENV DAL_MAJOR_BINARY="1"
ENV DAL_MINOR_BINARY="1"
ENV DNNLROOT="/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp"
ENV FI_PROVIDER_PATH="/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib/prov:/usr/lib64/libfabric"
ENV INFOPATH="/opt/intel/oneapi/debugger/10.1.1/documentation/info/"
ENV INSPECTOR_2021_DIR="/opt/intel/oneapi/inspector/2021.2.0"
ENV INTELFPGAOCLSDKROOT="/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga"
ENV INTEL_LICENSE_FILE="/opt/intel/licenses:/root/intel/licenses:/opt/intel/oneapi/clck/2021.2.0/licensing:/opt/intel/licenses:/root/intel/licenses:/Users/Shared/Library/Application Support/Intel/Licenses"
ENV INTEL_PYTHONHOME="/opt/intel/oneapi/debugger/10.1.1/dep"
ENV IPPCP_TARGET_ARCH="intel64"
ENV IPPCRYPTOROOT="/opt/intel/oneapi/ippcp/2021.2.0"
ENV IPPROOT="/opt/intel/oneapi/ipp/2021.2.0"
ENV IPP_TARGET_ARCH="intel64"
ENV I_MPI_ROOT="/opt/intel/oneapi/mpi/2021.2.0"
ENV LD_LIBRARY_PATH="/opt/intel/oneapi/vpl/2021.2.2/lib:/opt/intel/oneapi/tbb/2021.2.0/env/../lib/intel64/gcc4.8:/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib:/opt/intel/oneapi/mpi/2021.2.0//lib/release:/opt/intel/oneapi/mpi/2021.2.0//lib:/opt/intel/oneapi/mkl/latest/lib/intel64:/opt/intel/oneapi/itac/2021.2.0/slib:/opt/intel/oneapi/ippcp/2021.2.0/lib/intel64:/opt/intel/oneapi/ipp/2021.2.0/lib/intel64:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/debugger/10.1.1/dep/lib:/opt/intel/oneapi/debugger/10.1.1/libipt/intel64/lib:/opt/intel/oneapi/debugger/10.1.1/gdb/intel64/lib:/opt/intel/oneapi/dal/2021.2.0/lib/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/x64:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/emu:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/host/linux64/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/linux64/lib:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib/intel64_lin:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib:/opt/intel/oneapi/ccl/2021.2.0/lib/cpu_gpu_dpcpp:${LD_LIBRARY_PATH}"
ENV LIBRARY_PATH="/opt/intel/oneapi/vpl/2021.2.2/lib:/opt/intel/oneapi/tbb/2021.2.0/env/../lib/intel64/gcc4.8:/opt/intel/oneapi/mpi/2021.2.0//libfabric/lib:/opt/intel/oneapi/mpi/2021.2.0//lib/release:/opt/intel/oneapi/mpi/2021.2.0//lib:/opt/intel/oneapi/mkl/latest/lib/intel64:/opt/intel/oneapi/ippcp/2021.2.0/lib/intel64:/opt/intel/oneapi/ipp/2021.2.0/lib/intel64:/opt/intel/oneapi/dnnl/2021.2.0/cpu_dpcpp_gpu_dpcpp/lib:/opt/intel/oneapi/dal/2021.2.0/lib/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/compiler/lib/intel64_lin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib:/opt/intel/oneapi/clck/2021.2.0/lib/intel64:/opt/intel/oneapi/ccl/2021.2.0/lib/cpu_gpu_dpcpp:${LIBRARY_PATH}"
ENV MANPATH="/opt/intel/oneapi/mpi/2021.2.0/man:/opt/intel/oneapi/itac/2021.2.0/man:/opt/intel/oneapi/debugger/10.1.1/documentation/man:/opt/intel/oneapi/clck/2021.2.0/man::/opt/intel/oneapi/compiler/2021.2.0/documentation/en/man/common:::${MANPATH}"
ENV MKLROOT="/opt/intel/oneapi/mkl/latest"
ENV NLSPATH="/opt/intel/oneapi/mkl/latest/lib/intel64/locale/%l_%t/%N"
ENV OCL_ICD_FILENAMES="libintelocl_emu.so:libalteracl.so:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/x64/libintelocl.so"
ENV ONEAPI_ROOT="/opt/intel/oneapi"
ENV PATH="/opt/intel/oneapi/vtune/2021.2.0/bin64:/opt/intel/oneapi/vpl/2021.2.2/bin:/opt/intel/oneapi/mpi/2021.2.0//libfabric/bin:/opt/intel/oneapi/mpi/2021.2.0//bin:/opt/intel/oneapi/mkl/latest/bin/intel64:/opt/intel/oneapi/itac/2021.2.0/bin:/opt/intel/oneapi/itac/2021.2.0/bin:/opt/intel/oneapi/intelpython/latest/bin:/opt/intel/oneapi/intelpython/latest/condabin:/opt/intel/oneapi/inspector/2021.2.0/bin64:/opt/intel/oneapi/dev-utilities/2021.2.0/bin:/opt/intel/oneapi/debugger/10.1.1/gdb/intel64/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/llvm/aocl-bin:/opt/intel/oneapi/compiler/2021.2.0/linux/lib/oclfpga/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/bin/intel64:/opt/intel/oneapi/compiler/2021.2.0/linux/bin:/opt/intel/oneapi/compiler/2021.2.0/linux/ioc/bin:/opt/intel/oneapi/clck/2021.2.0/bin/intel64:/opt/intel/oneapi/advisor/2021.2.0/bin64:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:${PATH}"
ENV PKG_CONFIG_PATH="/opt/intel/oneapi/vtune/2021.2.0/include/pkgconfig/lib64:/opt/intel/oneapi/vpl/2021.2.2/lib/pkgconfig:/opt/intel/oneapi/mkl/latest/tools/pkgconfig:/opt/intel/oneapi/inspector/2021.2.0/include/pkgconfig/lib64:/opt/intel/oneapi/advisor/2021.2.0/include/pkgconfig/lib64:${PKG_CONFIG_PATH}:"
ENV PYTHONPATH="/opt/intel/oneapi/advisor/2021.2.0/pythonapi:${PYTHONPATH}"
ENV SETVARS_COMPLETED="1"
ENV SETVARS_VARS_PATH="/opt/intel/oneapi/vtune/latest/env/vars.sh"
ENV TBBROOT="/opt/intel/oneapi/tbb/2021.2.0/env/.."
ENV VPL_BIN="/opt/intel/oneapi/vpl/2021.2.2/bin"
ENV VPL_INCLUDE="/opt/intel/oneapi/vpl/2021.2.2/include"
ENV VPL_LIB="/opt/intel/oneapi/vpl/2021.2.2/lib"
ENV VPL_ROOT="/opt/intel/oneapi/vpl/2021.2.2"
ENV VTUNE_PROFILER_2021_DIR="/opt/intel/oneapi/vtune/2021.2.0"
ENV VT_ADD_LIBS="-ldwarf -lelf -lvtunwind -lm -lpthread"
ENV VT_LIB_DIR="/opt/intel/oneapi/itac/2021.2.0/lib"
ENV VT_MPI="impi4"
ENV VT_ROOT="/opt/intel/oneapi/itac/2021.2.0"
ENV VT_SLIB_DIR="/opt/intel/oneapi/itac/2021.2.0/slib"
ENV _CE_CONDA=""
ENV _CE_M=""

#-------------------------ZFP-------------------------------------#
WORKDIR /
RUN git clone https://github.com/LLNL/zfp.git && cd /zfp && make

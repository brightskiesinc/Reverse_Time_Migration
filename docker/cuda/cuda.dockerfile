#-------------------------------------------Cuda Dockerfile-------------------------------------------#
FROM nvidia/cuda:10.2-devel

RUN apt-get update && apt-get install -y --no-install-recommends gcc-8 g++-8 git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update -y
# OPEN CV
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y libopencv-dev python3-opencv

#-----------------------SPEED LOGGER------------------------------#
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

#------------------------CMake------------------------------------#
# reinstall newer cmake
ARG cmake_url=https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh
ADD $cmake_url /
RUN file=$(basename "$cmake_url") && \
    bash $file --prefix=/usr --skip-license && \
    rm $file
    
#-------------------------Env variables---------------------------#
ENV PATH="/usr/local/nvidia/bin:/usr/local/cuda/bin:/sbin:/bin:/usr/local/bin:/usr/sbin:${PATH}"
ENV LD_LIBRARY_PATH="/usr/local/nvidia/lib:/usr/local/nvidia/lib64:${LD_LIBRARY_PATH}"
ENV LIBRARY_PATH="/usr/local/cuda/lib64/stubs:${LIBRARY_PATH}"
#-------------------------ZFP-------------------------------------#
WORKDIR /
RUN git clone https://github.com/LLNL/zfp.git && cd /zfp && make

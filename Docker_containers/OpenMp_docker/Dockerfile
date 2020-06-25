FROM ubuntu:18.04
RUN apt-get update && apt-get install -y --no-install-recommends git wget --reinstall ca-certificates \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /opt
RUN wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/16225/parallel_studio_xe_2020_cluster_edition.tgz \
    && tar xvzf ./*.tgz \
    && rm ./*.tgz \
    && mkdir -p /opt/src/license \
    && mv /opt/parallel* /opt/src/
WORKDIR /opt/src
RUN wget https://computing.llnl.gov/projects/floating-point-compression/download/zfp-0.5.5.tar.gz \
    && tar xvf ./zfp* \
    && rm ./*.tar.gz
WORKDIR /opt/opencv_build
RUN git clone https://github.com/opencv/opencv.git  \
    && git clone https://github.com/opencv/opencv_contrib.git 
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends build-essential cmake cpio sudo vim pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libasound2 \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev libpango1.0-dev libpango-1.0-0 \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev xorg openbox xauth \
    python3-dev python3-pip gcc-8 g++-8 \
    && apt-get remove -y gcc-7 \
    && pip3 install -U pip==20.0.2 numpy==1.18.5 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8 \ 
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /opt/opencv_build/opencv/build
RUN cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=ON \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_build/opencv_contrib/modules \
    -D BUILD_EXAMPLES=ON ..
RUN make -j8 && make install
COPY files /opt/src/files
RUN useradd bskies -m -s /bin/bash \
    && echo "bskies    ALL=(ALL)    NOPASSWD: ALL" >> /etc/sudoers \
    && echo "source /opt/src/files/envs.sh" >> /home/bskies/.bashrc 
WORKDIR /opt/src/files
ENTRYPOINT ["./entrypoint.sh"]

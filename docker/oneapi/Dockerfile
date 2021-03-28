FROM intel/oneapi-basekit:devel-ubuntu18.04
RUN apt-get update && apt-get install -y --no-install-recommends gcc-8 g++-8 git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* 
WORKDIR /opt/opencv_build
RUN git clone https://github.com/opencv/opencv.git  \
    && git clone https://github.com/opencv/opencv_contrib.git 
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y build-essential cmake wget sudo \
    vim pkg-config libgtk-3-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    python3-dev python3-pip \
    && apt-get remove -y gcc-7 \
    && pip3 install -U pip==20.0.2 numpy==1.18.5 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
WORKDIR /opt/opencv_build/opencv/build
RUN  cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=ON \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_build/opencv_contrib/modules \
    -D BUILD_EXAMPLES=ON ..
RUN make -j8 && make install
RUN useradd bskies -m -s /bin/bash \
    && echo "bskies    ALL=(ALL)    NOPASSWD: ALL" >> /etc/sudoers \ 
    && echo "source /opt/intel/inteloneapi/setvars.sh" >> /home/bskies/.bashrc 
CMD ["su", "-", "bskies"]


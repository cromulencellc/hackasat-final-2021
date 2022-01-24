FROM ubuntu:20.04 as builder

WORKDIR /src_rtems
ENV PATH="/opt/rtems/5/bin:${PATH}"

RUN dpkg --add-architecture i386

RUN apt update -y; \
    apt upgrade -y; \
    DEBIAN_FRONTEND="noninteractive" apt install -y --no-install-recommends \
      gcc-multilib \
      autoconf \
      automake \
      bison \
      diffutils \
      flex \
      gawk \
      git \
      gperf \
      help2man \
      make \
      patch \
      texinfo \
      wget \
      'g++' \
      python-dev \
      xz-utils \
      libtool-bin \
      libncurses-dev \
      zip \
      unzip \
      bzip2 \
      zlib1g-dev \
      cmake \
      curl \
      default-jre \
      libssl-dev \
      libyaml-dev \
      libffi-dev \
      libreadline6-dev \
      zlib1g-dev \
      python-dev \
      xxd \
      linux-libc-dev:i386 \
    && apt-get clean autoclean \
    && rm -fr /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

# Checkout rtems 5.0.0 branch
RUN git clone git://git.rtems.org/rtems-source-builder.git rsb ;\
    cd rsb ;\
    git checkout -b has d48abf49ab0a 

# Checkout rtems 5.0.0 branch
RUN git clone git://git.rtems.org/rtems.git;\
    cd rtems ;\
    git checkout -b has 362cf319d44e

# Apply Leaf's mem unaligned patch
COPY *.patch /src_rtems/rtems
RUN cd /src_rtems/rtems; \ 
   git apply *.patch ;

# Set the builder to build both the sparc toolcahin and the x86 toolchain
RUN cd /src_rtems/rsb/rtems ;\
    /src_rtems/rsb/source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc ;\
    /src_rtems/rsb/source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-i386

RUN cd /src_rtems/rtems; \ 
    ./bootstrap -c ;\ 
    /src_rtems/rsb/source-builder/sb-bootstrap

ENV RTEMS_MAKEFILE_PATH=/opt/rtems/5/sparc-rtems5/leon3

# Give configs for compiling soft float + optimizations
COPY leon3_custom.cfg /src_rtems/rtems/bsps/sparc/leon3/config/leon3.cfg

# Copy in the modified greth driver
RUN rm /src_rtems/rtems/bsps/shared/grlib/net/greth.c
COPY greth.c /src_rtems/rtems/bsps/shared/grlib/net/

# Fix the ping executable
RUN rm /src_rtems/rtems/cpukit/libmisc/shell/main_ping.c
COPY main_ping.c /src_rtems/rtems/cpukit/libmisc/shell/main_ping.c

# Actually compile the toolchains
RUN cd /src_rtems/kernel/leon3 ;\
    /src_rtems/rtems/configure --prefix=/opt/rtems/5 --target=sparc-rtems5 --enable-rtemsbsp=leon3 \
        --enable-posix --enable-networking --enable-drvmgr ;\
    make -j4; \
    make install

RUN cd /src_rtems/kernel/i386 ;\
    /src_rtems/rtems/configure --prefix=/opt/rtems/5 --target=i386-rtems5 --enable-rtemsbsp=pc686 \
        --enable-posix --enable-networking CLOCK_DRIVER_USE_TSC=1;\
    make -j4; \
    make install

WORKDIR /src_rtems

ARG UID=1000
ARG GID=1000

RUN groupadd -r --gid=${GID} builder ;\
    useradd -m -r --uid=${UID} -g builder builder ;\
    chown -R builder:builder /src_rtems


RUN mkdir -p /opt/rtems/

RUN mv /src_rtems /opt/rtems

USER builder
WORKDIR  /apps/opensatkit/cfs

ENTRYPOINT [ "/bin/bash" , "/apps/opensatkit/cfs/build_mission.sh"]


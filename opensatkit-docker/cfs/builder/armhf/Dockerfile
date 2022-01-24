# FROM debian:stretch as builder

FROM ubuntu:18.04 as builder

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=America/New_York

RUN dpkg --add-architecture i386

RUN apt-get update ; \
    apt-get install -y  gcc-multilib

RUN apt-get update ; \
    apt-get install -y \
        cmake \
        curl \
        wget \
        default-jre \
        libssl-dev \
        libyaml-dev \
        libffi-dev \
        libreadline6-dev \
        zlib1g-dev \
        python-dev \
        xxd \
        gcc-arm-linux-gnueabihf \
        g++-arm-linux-gnueabihf \
        linux-libc-dev:i386 \
        libgsl-dev \
        libxml2 \
    && apt-get clean autoclean \
    && rm -fr /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*


# This line is needed because gcc-multilib and the arm cross compiler conflict
# To get around it, don't install gcc-multilib and insert this soft-link instead
RUN ln -s /usr/include/asm-generic /usr/include/asm

ARG ROOTFS_VERSION=1.0.0
ARG GITLAB_TOKEN

WORKDIR  /tmp/

RUN mkdir -p /opt/rootfs
ADD adcs-001.tar.gz /opt/rootfs/

ARG UID=1000
ARG GID=1000

RUN if [ ${UID:-0} -ne 0 ] && [ ${GID:-0} -ne 0 ]; then \
    groupadd -g ${UID} cfs && \
    useradd -l -m -u ${UID} -g cfs cfs; \
    fi

RUN mkdir -p /apps/opensatkit/cfs
WORKDIR  /apps/opensatkit/cfs

ENTRYPOINT [ "/bin/bash" , "/apps/opensatkit/cfs/build_mission.sh"]

# Thank you pedant
# Author: Meenmachine
# Container that compiles binaries and openssl dynamic library

FROM centos:7 AS client-builder

RUN yum -y update && \
    yum -y install \
    centos-release-scl \
    git \
    perl-core \
    pcre-devel \
    wget \
    zlib-devel

RUN yum -y update && \
    yum -y install devtoolset-8-gcc* \
    gcc \
    gcc-c++ \
    make

RUN scl enable devtoolset-8 bash

WORKDIR /src/

RUN git clone https://github.com/openssl/openssl.git && \
    cd openssl && \
    git checkout 5fdc95e443b4d62a3d1f7094ae6d6ae4682b77e0

WORKDIR /src/openssl
RUN mkdir build && cd build && \
    ../config && \
    make -j8

WORKDIR /src/
RUN mkdir -p /opt/ssl
RUN cp /src/openssl/build/*.so* /opt/ssl/

ENV LD_LIBRARY_PATH=/opt/ssl/

COPY build_centos.sh /src/

ENTRYPOINT [ "bash", "/src/build_centos.sh" ]
### Builds Cado-NFS that is used to factor compound numbers

FROM ubuntu:20.04 as builder

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=America/New_York

RUN apt -y update && apt -y install \
    git \
    build-essential \
    cmake \
    ssh \
    rsync \
    gzip \
    libgmp3-dev

WORKDIR /src
RUN git clone https://gitlab.inria.fr/cado-nfs/cado-nfs.git
RUN cd cado-nfs && git checkout c7e40d9e95cb6faaf16bfd969d7d3054dfa5094d

WORKDIR /src/cado-nfs
RUN make -j8

RUN mv /src/cado-nfs/build/* /src/build/
WORKDIR /src/build

ENTRYPOINT ["./cado-nfs.py"]
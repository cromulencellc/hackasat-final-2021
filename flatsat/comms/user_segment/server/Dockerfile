FROM ubuntu:20.04 AS server_builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y git \
    python3-venv \
    python3-dev \
    build-essential \
    git

ENV VIRTUAL_ENV="/opt/venv"
RUN /usr/bin/python3 -m venv --system-site-packages $VIRTUAL_ENV
ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"

COPY requirements.txt /tmp/
RUN python3 -m pip install --upgrade pip && \
    python3 -m pip install -r /tmp/requirements.txt 

### Keygen Openssl Builder ###
WORKDIR /src/

RUN git clone https://github.com/openssl/openssl.git && \
    cd openssl && \
    git checkout 5fdc95e443b4d62a3d1f7094ae6d6ae4682b77e0 && \
    mkdir build && cd build && \
    ../config && \
    make -j8

RUN mkdir -p /opt/ssl
RUN cp /src/openssl/build/*.so* /opt/ssl/

COPY tools/key_gen/main.c /src/

RUN gcc -I openssl/include \
        -I openssl/crypto/rsa/ \
        -I openssl/build/include \
        -I openssl/providers/common/include \
        main.c openssl/crypto/rsa/rsa_mp.c \
        -L/opt/ssl/ -lcrypto -lssl -o gen

#######################
# Challenge container
#######################

# FROM phusion/baseimage:master-amd64 as user-seg-server
FROM ubuntu:20.04 as user-seg-server

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y --no-install-recommends \
    netcat \
    python3 \
    tini

ENV VIRTUAL_ENV="/opt/venv"
ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"
COPY --from=server_builder /opt/venv /opt/venv

### User Segment Server Files

WORKDIR /server

COPY tools/generate_team_keys_v2.py \
     /server/tools/
COPY tools/generate_team_keys_safe.py \
     /server/tools/
COPY tools/ping_controller/controller.py \
     /server/tools/ping_controller/

WORKDIR /server

COPY server.py \
     /server

RUN chmod +x server.py tools/generate_team_keys_v2.py tools/generate_team_keys_safe.py && \
    python -m compileall .

### Key Gen Files

COPY --from=server_builder /opt/ssl /opt/ssl
COPY --from=server_builder /src/gen /server/tools/key_gen/gen

ENV LD_LIBRARY_PATH=/opt/ssl/

### Lower User Add

RUN groupadd -r --gid=1000 userseg && \
    useradd -m -r -s /bin/bash -g userseg --uid=1000 userseg

RUN chown -R userseg /server && \
    chgrp -R userseg /server

USER userseg

ENV PATH=$PATH:/server/.local/bin
ENTRYPOINT ["/usr/bin/tini", "--", "/server/server.py"]
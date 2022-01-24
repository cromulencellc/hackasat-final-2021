# Thank you pedant

FROM ubuntu:20.04 AS keygen-builder

RUN apt update && \
    apt install -y \
    git build-essential

WORKDIR /src/

RUN git clone https://github.com/openssl/openssl.git && \
    cd openssl && \
    git checkout 5fdc95e443b4d62a3d1f7094ae6d6ae4682b77e0 && \
    mkdir build && cd build && \
    ../config && \
    make -j8

RUN mkdir -p /opt/ssl
RUN cp /src/openssl/build/*.so* /opt/ssl/

COPY main.c /src/

RUN gcc -I openssl/include \
        -I openssl/crypto/rsa/ \
        -I openssl/build/include \
        -I openssl/providers/common/include \
        main.c openssl/crypto/rsa/rsa_mp.c \
        -L/opt/ssl/ -lcrypto -lssl -o gen

FROM ubuntu:20.04 AS keygen 

COPY --from=keygen-builder /opt/ssl /opt/ssl
COPY --from=keygen-builder /src/gen /keygen/gen

ENV LD_LIBRARY_PATH=/opt/ssl/

# Mount config at /config in container
ENTRYPOINT [ "/keygen/gen" ]
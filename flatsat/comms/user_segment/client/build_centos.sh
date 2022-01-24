#!/bin/bash

mkdir -p /build/binary/libs

cp /src/openssl/build/*.so /build/binary/libs/

cd /build/binary/libs/

ln -s ./libcrypto.so ./libcrypto.so.3
ln -s ./libssl.so ./libssl.so.3

source /opt/rh/devtoolset-8/enable

g++ -fpermissive \
    -I /src/include -I /src/openssl/include/ \
    -I /src/openssl/crypto/rsa/ \
    -I /src/openssl/build/include \
    -I /src/openssl/providers/common/include/ \
    /src/src/client.cpp \
    /src/openssl/crypto/rsa/rsa_mp.c \
    -std=c++17 \
    -L /src/openssl/build \
    -lcrypto \
    -lssl \
    -o /build/binary/client

cd /build/binary
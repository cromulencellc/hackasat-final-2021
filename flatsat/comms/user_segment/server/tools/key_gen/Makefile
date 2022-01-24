IMAGE_REPO=user_seg
IMAGE_TAG=client

build: Dockerfile main.c
	docker build -t $(IMAGE_REPO):$(IMAGE_TAG) .

run: build
	docker run -it --rm -v $(shell pwd):/keys/ $(IMAGE_REPO):$(IMAGE_TAG)

local_build:
	gcc -I openssl/include -I openssl/crypto/rsa/ -I openssl/build/include -I openssl/providers/common/include main.c openssl/crypto/rsa/rsa_mp.c -Lopenssl/build -lcrypto -lssl -o gen
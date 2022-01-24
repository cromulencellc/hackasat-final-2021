# Contains old Makefile directives for old compile

REMOTE_DOCKER_TAG="has2/finals/comms/user-seg-client:latest"
GCC=g++
LIBS=-lcrypto -lssl
DOCKER_LIB_INCLUDES=-L/opt/ssl

SSL_IDAR=./include/openssl
IDAR=./include
CFLAGS=-I$(IDAR) -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Werror
DEBUG=-g

_SSL_INCLUDES=-I /include -I /crypto/rsa -I /build/include -I /providers/common/include
SSL_INCLUDES=$(patsubst /%,$(SSL_IDAR)/%,$(_SSL_INCLUDES))

_LOCAL_LIB_INCLUDES=-L /build
LOCAL_LIB_INCLUDES=$(patsubst /%,$(SSL_IDAR)/%,$(_LOCAL_LIB_INCLUDES))

_DEPS = client.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_SRC = client.cpp
SRC = $(patsubst %,$(SDIR)/%,$(_SRC)

_OBJ = $(patsubst %.c,%.o,$(_SRC)) 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

TARGET = client
OTHERS = stripped debug

all: $(ALL) $(OTHERS)

tester: src/client.cpp include/openssl/crypto/rsa/rsa_mp.c
	docker run -it --rm \
		-v $(shell pwd)/src/:/src/src \
		-v $(shell pwd)/include/:/src/include \
		-v $(shell pwd)/build/binary/:/build/binary \
		user_segment:client

local_build: src/client.cpp include/openssl/crypto/rsa/rsa_mp.c
	$(GCC) -fpermissive -I $(IDAR) $(SSL_INCLUDES) $^ -std=c++17 $(LOCAL_LIB_INCLUDES) $(LIBS) -o client

docker_pull:
	docker pull $(REMOTE_DOCKER_TAG)

docker_build:
	docker build -t user_segment:client .

client: src/client.cpp
	@echo $(LOCAL_INCLUDES)
	$(GCC) -o $@ $^ $(CFLAGS) $(LIBS)

stripped: client
	strip client -o $(TARGET)_$@

debug: src/client.cpp
	$(GCC) -o $(TARGET)_$@ $^ $(CFLAGS) $(DEBUG) $(LIBS)

clean:
	$(RM) $(TARGET) $(addprefix $(TARGET)_, $(OTHERS))

run:
	# docker run --network=host -v /home/dev/Desktop/comms/user_segment/server/configs:/configs -it --rm has2/finals/comms/user-seg-client:latest -k 4491623606443121946 -f /configs/team_private_keys/team_1_rsa_priv.pem -d AAAAAAAAAAAA -i 1
	docker run -it --rm --network=host -v $(shell pwd)/../server/configs/:/configs \
		-k 4491623606443121946 \
		-d AAAAAAAAAAAA \
		-i 1 \
		$(REMOTE_DOCKER_TAG)

test:
	LD_LIBRARY_PATH=./include/openssl/build ./local_another -f ../server/configs/team_private_keys/team_1_rsa_priv.pem -d 123123123123123123 -i 1 -k 41414141

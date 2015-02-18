CC=gcc
AR=ar
RANLIB=ranlib

OPENSSL_INCLUDE_DIRECTORY=/usr/local/opt/openssl/include
OPENSSL_LIBRARY_DIRECTORY=/usr/local/opt/openssl/lib

all: build tests check tests/dopenssl/bn.status tests/dopenssl/rand.status tests/dopenssl/rsa.status

build: lib/libdopenssl.a lib/libdopenssl.dylib

lib/libdopenssl.dylib: src/dopenssl/rand.o src/dopenssl/bn.o src/dopenssl/rsa.o
	@mkdir -p lib
	${CC} -Wall -fPIC -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath,@loader_path -undefined dynamic_lookup -Wl,-install_name,@rpath/libdopenssl.dylib -Wl,-headerpad_max_install_names src/dopenssl/bn.o src/dopenssl/rand.o src/dopenssl/rsa.o -shared -o lib/libdopenssl.dylib -lssl -lcrypto

lib/libdopenssl.a: src/dopenssl/rand.o src/dopenssl/bn.o src/dopenssl/rsa.o
	@mkdir -p lib
	${AR} crs lib/libdopenssl.a src/dopenssl/bn.o src/dopenssl/rand.o src/dopenssl/rsa.o
	${RANLIB} lib/libdopenssl.a

src/dopenssl/bn.o: ../../src/dopenssl/bn.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fPIC -DNDEBUG -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -fPIC -c ../../src/dopenssl/bn.c -o src/dopenssl/bn.o

src/dopenssl/rand.o: ../../src/dopenssl/rand.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fPIC -DNDEBUG -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -fPIC -c ../../src/dopenssl/rand.c -o src/dopenssl/rand.o

src/dopenssl/rsa.o: ../../src/dopenssl/rsa.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fPIC -DNDEBUG -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -fPIC -c ../../src/dopenssl/rsa.c -o src/dopenssl/rsa.o

tests: tests/dopenssl/bn tests/dopenssl/rand tests/dopenssl/rsa

check: tests/dopenssl/bn.status tests/dopenssl/rand.status tests/dopenssl/rsa.status

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/bn.o -o tests/dopenssl/bn -lssl -lcrypto -ldopenssl

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rand.o -o tests/dopenssl/rand -lssl -lcrypto -ldopenssl

tests/dopenssl/rsa: lib/libdopenssl.dylib tests/dopenssl/rsa.o
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lssl -lcrypto -ldopenssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fPIC -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fPIC -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fPIC -isystem ${OPENSSL_INCLUDE_DIRECTORY} -isystem . -I ../../src -I src -O2 -Wall -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

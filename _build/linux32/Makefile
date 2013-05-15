all: tests/dopenssl/rsa.out tests/dopenssl/rsa.err install tests/dopenssl/rand.err tests/dopenssl/bn.out tests/dopenssl/rand.out check tests tests/dopenssl/bn.err build

tests/dopenssl/rsa.out: tests/dopenssl/rsa lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rsa: tests/dopenssl/rsa.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	gcc -L lib -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lcrypto -ldopenssl -lssl

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

lib/libdopenssl.so: sources/dopenssl/rsa.o sources/dopenssl/rand.o sources/dopenssl/bn.o
	@mkdir -p lib
	gcc -Wall -fPIC -L lib '-Wl,-rpath,$$ORIGIN' sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o -shared -o lib/libdopenssl.so -lcrypto -lssl

sources/dopenssl/rsa.o: ../../sources/dopenssl/rsa.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rsa.c -o sources/dopenssl/rsa.o

sources/dopenssl/rand.o: ../../sources/dopenssl/rand.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rand.c -o sources/dopenssl/rand.o

sources/dopenssl/bn.o: ../../sources/dopenssl/bn.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/bn.c -o sources/dopenssl/bn.o

tests/dopenssl/rsa.err: tests/dopenssl/rsa lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

install: /usr/lib/libdopenssl.so /usr/include/dopenssl/all.h /usr/include/dopenssl/rsa.h /usr/lib/libdopenssl.a /usr/include/dopenssl/rand.h /usr/include/dopenssl/bn.h

/usr/lib/libdopenssl.so: lib/libdopenssl.so
	@mkdir -p /usr/lib
	cp lib/libdopenssl.so /usr/lib/libdopenssl.so

/usr/include/dopenssl/all.h: ../../sources/dopenssl/all.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/all.h /usr/include/dopenssl/all.h

/usr/include/dopenssl/rsa.h: ../../sources/dopenssl/rsa.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rsa.h /usr/include/dopenssl/rsa.h

/usr/lib/libdopenssl.a: lib/libdopenssl.a
	@mkdir -p /usr/lib
	cp lib/libdopenssl.a /usr/lib/libdopenssl.a

lib/libdopenssl.a: sources/dopenssl/rsa.o sources/dopenssl/rand.o sources/dopenssl/bn.o
	@mkdir -p lib
	ar crs lib/libdopenssl.a sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o
	ranlib lib/libdopenssl.a

/usr/include/dopenssl/rand.h: ../../sources/dopenssl/rand.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rand.h /usr/include/dopenssl/rand.h

/usr/include/dopenssl/bn.h: ../../sources/dopenssl/bn.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/bn.h /usr/include/dopenssl/bn.h

tests/dopenssl/rand.err: tests/dopenssl/rand lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	gcc -L lib -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' tests/dopenssl/rand.o -o tests/dopenssl/rand -lcrypto -ldopenssl -lssl

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

tests/dopenssl/bn.out: tests/dopenssl/bn lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	gcc -L lib -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' tests/dopenssl/bn.o -o tests/dopenssl/bn -lcrypto -ldopenssl -lssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rand.out: tests/dopenssl/rand lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

check: tests/dopenssl/rand.status tests/dopenssl/bn.status tests/dopenssl/rsa.status

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests: tests/dopenssl/rand tests/dopenssl/bn tests/dopenssl/rsa

tests/dopenssl/bn.err: tests/dopenssl/bn lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

build: lib/libdopenssl.a lib/libdopenssl.so


all: tests/dopenssl/rand.out tests/dopenssl/bn.err tests/dopenssl/rsa.out install tests check build tests/dopenssl/bn.out tests/dopenssl/rsa.err tests/dopenssl/rand.err

tests/dopenssl/rand.out: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.7 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rand.o -o tests/dopenssl/rand -lssl -lcrypto -ldopenssl

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

lib/libdopenssl.dylib: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	gcc-mp-4.7 -Wall -fPIC -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -undefined dynamic_lookup -Wl,-install_name,@rpath/libdopenssl.dylib -Wl,-headerpad_max_install_names sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o -shared -o lib/libdopenssl.dylib -lssl -lcrypto

sources/dopenssl/rand.o: ../../sources/dopenssl/rand.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rand.c -o sources/dopenssl/rand.o

sources/dopenssl/bn.o: ../../sources/dopenssl/bn.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/bn.c -o sources/dopenssl/bn.o

sources/dopenssl/rsa.o: ../../sources/dopenssl/rsa.c
	@mkdir -p sources/dopenssl
	gcc -Wall -fPIC -DNDEBUG -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rsa.c -o sources/dopenssl/rsa.o

tests/dopenssl/bn.err: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.7 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/bn.o -o tests/dopenssl/bn -lssl -lcrypto -ldopenssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rsa.out: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rsa: lib/libdopenssl.dylib tests/dopenssl/rsa.o
	@mkdir -p tests/dopenssl
	gcc-mp-4.7 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lssl -lcrypto -ldopenssl

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	gcc -Wall -fPIC -isystem /opt/local/include -isystem . -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

install: /usr/include/dopenssl/rand.h /usr/include/dopenssl/all.h /usr/lib/libdopenssl.dylib /usr/include/dopenssl/bn.h /usr/lib/libdopenssl.a /usr/include/dopenssl/rsa.h

/usr/include/dopenssl/rand.h: ../../sources/dopenssl/rand.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rand.h /usr/include/dopenssl/rand.h

/usr/include/dopenssl/all.h: ../../sources/dopenssl/all.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/all.h /usr/include/dopenssl/all.h

/usr/lib/libdopenssl.dylib: lib/libdopenssl.dylib
	@mkdir -p /usr/lib
	cp lib/libdopenssl.dylib /usr/lib/libdopenssl.dylib

/usr/include/dopenssl/bn.h: ../../sources/dopenssl/bn.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/bn.h /usr/include/dopenssl/bn.h

/usr/lib/libdopenssl.a: lib/libdopenssl.a
	@mkdir -p /usr/lib
	cp lib/libdopenssl.a /usr/lib/libdopenssl.a

lib/libdopenssl.a: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	ar crs lib/libdopenssl.a sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o
	ranlib lib/libdopenssl.a

/usr/include/dopenssl/rsa.h: ../../sources/dopenssl/rsa.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rsa.h /usr/include/dopenssl/rsa.h

tests: tests/dopenssl/rand tests/dopenssl/bn tests/dopenssl/rsa

check: tests/dopenssl/rsa.status tests/dopenssl/rand.status tests/dopenssl/bn.status

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

build: lib/libdopenssl.a lib/libdopenssl.dylib

tests/dopenssl/bn.out: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/rsa.err: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.err: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

all: tests/dopenssl/rand.out tests/dopenssl/bn.err tests/dopenssl/rsa.out install tests check build tests/dopenssl/bn.out tests/dopenssl/rsa.err tests/dopenssl/rand.err

tests/dopenssl/rand.out: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rand.o -o tests/dopenssl/rand -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

lib/libdopenssl.dylib: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	gcc-mp-4.8 -fPIC -Wall -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -undefined dynamic_lookup -Wl,-install_name,@rpath/libdopenssl.dylib -Wl,-headerpad_max_install_names sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o -shared -o lib/libdopenssl.dylib -lcrypto -lssl

sources/dopenssl/rand.o: ../../sources/dopenssl/rand.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rand.c -o sources/dopenssl/rand.o

sources/dopenssl/bn.o: ../../sources/dopenssl/bn.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/bn.c -o sources/dopenssl/bn.o

sources/dopenssl/rsa.o: ../../sources/dopenssl/rsa.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rsa.c -o sources/dopenssl/rsa.o

tests/dopenssl/bn.err: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/bn.o -o tests/dopenssl/bn -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rsa.out: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rsa: lib/libdopenssl.dylib tests/dopenssl/rsa.o
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

install: /usr/include/dopenssl/rand.h /usr/include/dopenssl/all.h /usr/lib/libdopenssl.dylib /usr/include/dopenssl/bn.h /usr/lib/libdopenssl.a /usr/include/dopenssl/rsa.h

/usr/include/dopenssl/rand.h: ../../sources/dopenssl/rand.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rand.h /usr/include/dopenssl/rand.h

/usr/include/dopenssl/all.h: ../../sources/dopenssl/all.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/all.h /usr/include/dopenssl/all.h

/usr/lib/libdopenssl.dylib: lib/libdopenssl.dylib
	@mkdir -p /usr/lib
	cp lib/libdopenssl.dylib /usr/lib/libdopenssl.dylib
	install_name_tool -id lib/libdopenssl.dylib lib/libdopenssl.dylib

/usr/include/dopenssl/bn.h: ../../sources/dopenssl/bn.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/bn.h /usr/include/dopenssl/bn.h

/usr/lib/libdopenssl.a: lib/libdopenssl.a
	@mkdir -p /usr/lib
	cp lib/libdopenssl.a /usr/lib/libdopenssl.a

lib/libdopenssl.a: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	ar crs lib/libdopenssl.a sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o
	ranlib lib/libdopenssl.a

/usr/include/dopenssl/rsa.h: ../../sources/dopenssl/rsa.h
	@mkdir -p /usr/include/dopenssl
	cp ../../sources/dopenssl/rsa.h /usr/include/dopenssl/rsa.h

tests: tests/dopenssl/rand tests/dopenssl/bn tests/dopenssl/rsa

check: tests/dopenssl/rsa.status tests/dopenssl/rand.status tests/dopenssl/bn.status

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

build: lib/libdopenssl.a lib/libdopenssl.dylib

tests/dopenssl/bn.out: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/rsa.err: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.err: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

all: tests/dopenssl/rand.out tests/dopenssl/bn.err tests/dopenssl/rsa.out install tests check build tests/dopenssl/bn.out tests/dopenssl/rsa.err tests/dopenssl/rand.err

tests/dopenssl/rand.out: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rand.o -o tests/dopenssl/rand -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

lib/libdopenssl.dylib: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	gcc-mp-4.8 -fPIC -Wall -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -undefined dynamic_lookup -Wl,-install_name,@rpath/libdopenssl.dylib -Wl,-headerpad_max_install_names sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o -shared -o lib/libdopenssl.dylib -lcrypto -lssl

sources/dopenssl/rand.o: ../../sources/dopenssl/rand.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rand.c -o sources/dopenssl/rand.o

sources/dopenssl/bn.o: ../../sources/dopenssl/bn.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/bn.c -o sources/dopenssl/bn.o

sources/dopenssl/rsa.o: ../../sources/dopenssl/rsa.c
	@mkdir -p sources/dopenssl
	gcc-mp-4.8 -fPIC -Wall -DNDEBUG -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -fPIC -c ../../sources/dopenssl/rsa.c -o sources/dopenssl/rsa.o

tests/dopenssl/bn.err: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/bn.o -o tests/dopenssl/bn -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rsa.out: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rsa: lib/libdopenssl.dylib tests/dopenssl/rsa.o
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -L lib -L /opt/local/lib -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../../lib -undefined dynamic_lookup tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lcrypto -lssl -lcrypto -lssl -ldopenssl

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	gcc-mp-4.8 -fPIC -Wall -isystem . -isystem /opt/local/include -I ../../sources -I sources -O2 -Wall -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

install: /Users/mycure/local/include/dopenssl/rand.h /Users/mycure/local/include/dopenssl/bn.h /Users/mycure/local/include/dopenssl/all.h /Users/mycure/local/include/dopenssl/rsa.h /Users/mycure/local/lib/libdopenssl.dylib /Users/mycure/local/lib/libdopenssl.a

/Users/mycure/local/include/dopenssl/rand.h: ../../sources/dopenssl/rand.h
	@mkdir -p /Users/mycure/local/include/dopenssl
	cp ../../sources/dopenssl/rand.h /Users/mycure/local/include/dopenssl/rand.h

/Users/mycure/local/include/dopenssl/bn.h: ../../sources/dopenssl/bn.h
	@mkdir -p /Users/mycure/local/include/dopenssl
	cp ../../sources/dopenssl/bn.h /Users/mycure/local/include/dopenssl/bn.h

/Users/mycure/local/include/dopenssl/all.h: ../../sources/dopenssl/all.h
	@mkdir -p /Users/mycure/local/include/dopenssl
	cp ../../sources/dopenssl/all.h /Users/mycure/local/include/dopenssl/all.h

/Users/mycure/local/include/dopenssl/rsa.h: ../../sources/dopenssl/rsa.h
	@mkdir -p /Users/mycure/local/include/dopenssl
	cp ../../sources/dopenssl/rsa.h /Users/mycure/local/include/dopenssl/rsa.h

/Users/mycure/local/lib/libdopenssl.dylib: lib/libdopenssl.dylib
	@mkdir -p /Users/mycure/local/lib
	cp lib/libdopenssl.dylib /Users/mycure/local/lib/libdopenssl.dylib
	install_name_tool -id lib/libdopenssl.dylib lib/libdopenssl.dylib

/Users/mycure/local/lib/libdopenssl.a: lib/libdopenssl.a
	@mkdir -p /Users/mycure/local/lib
	cp lib/libdopenssl.a /Users/mycure/local/lib/libdopenssl.a

lib/libdopenssl.a: sources/dopenssl/rand.o sources/dopenssl/bn.o sources/dopenssl/rsa.o
	@mkdir -p lib
	ar crs lib/libdopenssl.a sources/dopenssl/bn.o sources/dopenssl/rand.o sources/dopenssl/rsa.o
	ranlib lib/libdopenssl.a

tests: tests/dopenssl/rand tests/dopenssl/bn tests/dopenssl/rsa

check: tests/dopenssl/rsa.status tests/dopenssl/rand.status tests/dopenssl/bn.status

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

build: lib/libdopenssl.a lib/libdopenssl.dylib

tests/dopenssl/bn.out: tests/dopenssl/bn lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

tests/dopenssl/rsa.err: tests/dopenssl/rsa lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.err: tests/dopenssl/rand lib/libdopenssl.dylib
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand


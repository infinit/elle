SOURCES =					\
  src/reactor/debug.cc				\
  src/reactor/exception.cc			\
  src/reactor/lockable.cc			\
  src/reactor/mutex.cc				\
  src/reactor/network/exception.cc		\
  src/reactor/network/resolve.cc		\
  src/reactor/network/server.cc			\
  src/reactor/network/socket.cc			\
  src/reactor/network/tcp-server.cc		\
  src/reactor/network/tcp-socket.cc		\
  src/reactor/network/udp-server.cc		\
  src/reactor/network/udp-server-socket.cc	\
  src/reactor/network/udp-socket.cc		\
  src/reactor/operation.cc			\
  src/reactor/rw-mutex.cc			\
  src/reactor/scheduler.cc			\
  src/reactor/semaphore.cc			\
  src/reactor/signal.cc				\
  src/reactor/sleep.cc				\
  src/reactor/thread.cc				\
  src/reactor/waitable.cc			\
  $(SOURCES_BACKEND_THREAD_BOOST)		\
  $(SOURCES_BACKEND_CORO_IO)			\

SOURCES_BACKEND_THREAD_BOOST =			\
  src/reactor/backend/pthread/pthread.cc	\

SOURCES_BACKEND_CORO_IO =				\
  src/reactor/backend/coro_io/libcoroutine/context.cc	\
  src/reactor/backend/coro_io/libcoroutine/coroutine.cc	\
  src/reactor/backend/coro_io/thread.cc			\

SOURCES_ASM =						\
  src/reactor/backend/coro_io/libcoroutine/asm.S	\

LIB=lib/libreactor.a

TEST_SOURCES =					\
  tests/reactor/backend/test.cc			\
  tests/reactor/network/test.cc			\
  tests/reactor/test.cc				\

TEST=tests/test

OBJECTS = $(SOURCES:.cc=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

CPPFLAGS += -I src
#CPPFLAGS += -DINFINIT_REACTOR_ENABLE_DEBUG
CXXFLAGS += -g -Woverloaded-virtual -Wall -Wno-parentheses
LDFLAGS += -lboost_system -lboost_thread
LDFLAGS += -lpthread

all: $(LIB) $(TEST)

DEPS=$(SOURCES:=.deps) $(TEST_SOURCES:=.deps)
-include $(DEPS)

%.cc.deps: %.cc
	$(CXX) $(CPPFLAGS) -MM $< \
	| sed 's,^\(.*\)\.o: \(.*\)\.cc,\2.o: \2.cc,' > $@.tmp
	cp $@.tmp $@
	sed 's,^\(.*\)\.o: ,\1.cc.deps: ,' $@.tmp >> $@
	rm $@.tmp

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(LIB): $(OBJECTS)
	ar crs $@ $^

$(TEST): $(TEST_OBJECTS) $(LIB) $(SOURCES_ASM)
	$(CXX) $^ $(LDFLAGS) -lboost_unit_test_framework -o $@

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TEST) $(DEPS)

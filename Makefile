projectpath = ${CURDIR}
libuv_path = ${projectpath}/lib/libuv
lockless_path = ${projectpath}/lib/lockless_allocator
tcmalloc_path = ${projectpath}/lib/tcmalloc
lmdb_path = ${projectpath}/lib/lmdb

ifeq ($(OS),Windows_NT)
	OPERATING_SYSTEM = WINDOWS
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		OPERATING_SYSTEM = LINUX
		PLATFORM_TARGET = linux
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
		OPERATING_SYSTEM = OSX
		PLATFORM_TARGET = osx
        CCFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

all: deps $(PLATFORM_TARGET)

target: all 

osx: deps
	if [ ! -d "build" ]; then mkdir -p build; fi
	if [ ! -d "build/Makefile" ]; then cd build;cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 ..; fi
	cmake --build ./build --target all --config Debug -- -j 10

xcode: deps
	if [ ! -d "build" ]; then mkdir -p build; fi
	if [ ! -d "build/rewind.xcodeproj" ]; then cd build;cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -G Xcode ..; fi
	cd build;xcodebuild -project rewind.xcodeproj/

linux: deps $(lockless_path)
	rm -rf build
	mkdir -p build
	cd build;cmake ..
	cd build;make VERBOSE=1

$(libuv_path)/.libs/libuv.a:
	if [ ! -d "$(libuv_path)" ]; then git clone https://github.com/libuv/libuv.git $(libuv_path); fi
	cd $(libuv_path);sh autogen.sh
	cd $(libuv_path);./configure
	cd $(libuv_path);make

$(tcmalloc_path):
	if [ ! -d "$(tcmalloc_path)" ]; then git clone https://github.com/gperftools/gperftools.git $(tcmalloc_path); fi
	cd $(tcmalloc_path);./autogen.sh
	cd $(tcmalloc_path);./configure
	cd $(tcmalloc_path);make

$(lockless_path):
	if [ ! -d "$(lockless_path)" ]; then wget https://locklessinc.com/downloads/lockless_allocator_src.tgz -P lib; fi
	cd lib;tar xvzf lockless_allocator_src.tgz
	cd lib/lockless_allocator;make

$(lmdb_path)/libraries/liblmdb/liblmdb.a:
	if [ ! -d "$(lmdb_path)" ]; then git clone https://github.com/LMDB/lmdb.git $(lmdb_path); fi
	cd $(lmdb_path)/libraries/liblmdb;make

tools: $(wrk_path)/wrk $(wrk2_path)/wrk

test: $(PLATFORM_TARGET)
	./build/tests

deps: $(libuv_path)/.libs/libuv.a $(tcmalloc_path) $(lmdb_path)/libraries/liblmdb/liblmdb.a

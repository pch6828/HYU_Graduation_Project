include ../rocksdb-6.15.5/make_config.mk

ifndef DISABLE_JEMALLOC
	ifdef JEMALLOC
		PLATFORM_CXXFLAGS += -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE
	endif
	EXEC_LDFLAGS := $(JEMALLOC_LIB) $(EXEC_LDFLAGS) -lpthread
	PLATFORM_CXXFLAGS += $(JEMALLOC_INCLUDE)
endif

ifneq ($(USE_RTTI), 1)
	CXXFLAGS += -fno-rtti
endif

.PHONY: clean

all: 
	./compile.sh

test_with_origin_rocksdb: ./lib/origin_rocksdb.a read_bottleneck_breakdown_test.cc
	$(CXX) $(CXXFLAGS) -g read_bottleneck_breakdown_test.cc -o$@ ./lib/origin_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

test_with_custom_rocksdb: ./lib/custom_rocksdb.a read_bottleneck_breakdown_test.cc
	$(CXX) $(CXXFLAGS) -g read_bottleneck_breakdown_test.cc -o$@ ./lib/custom_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

clean:
	rm -rf ./test_with_origin_rocksdb ./test_with_custom_rocksdb ./lib/origin_rocksdb* ./lib/custom_rocksdb*

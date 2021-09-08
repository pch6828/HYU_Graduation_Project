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

all: test_with_origin_rocksdb test_with_custom_rocksdb

test_with_origin_rocksdb: origin_rocksdb.a read_bottleneck_breakdown_test.cc
	$(CXX) $(CXXFLAGS) -g read_bottleneck_breakdown_test.cc -o$@ origin_rocksdb.a -I../include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

test_with_custom_rocksdb: custom_rocksdb.a read_bottleneck_breakdown_test.cc
	$(CXX) $(CXXFLAGS) -g read_bottleneck_breakdown_test.cc -o$@ custom_rocksdb.a -I../include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

clean:
	rm -rf ./test_with_origin_rocksdb ./test_with_custom_rocksdb ./origin_rocksdb* ./custom_rocksdb*

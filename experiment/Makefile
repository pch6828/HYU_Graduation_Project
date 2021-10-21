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

test_with_origin_rocksdb: test_with_origin_rocksdb_skewed test_with_origin_rocksdb_uniform

test_with_custom_rocksdb: test_with_custom_rocksdb_skewed test_with_custom_rocksdb_uniform

test_with_origin_rocksdb_skewed: ./lib/origin_rocksdb.a skewed_experiment.cc
	$(CXX) $(CXXFLAGS) -g skewed_experiment.cc -o$@ ./lib/origin_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

test_with_custom_rocksdb_skewed: ./lib/custom_rocksdb.a skewed_experiment.cc
	$(CXX) $(CXXFLAGS) -g skewed_experiment.cc -o$@ ./lib/custom_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

test_with_origin_rocksdb_uniform: ./lib/origin_rocksdb.a uniform_experiment.cc
	$(CXX) $(CXXFLAGS) -g uniform_experiment.cc -o$@ ./lib/origin_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

test_with_custom_rocksdb_uniform: ./lib/custom_rocksdb.a uniform_experiment.cc
	$(CXX) $(CXXFLAGS) -g uniform_experiment.cc -o$@ ./lib/custom_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

correctness_test: ./lib/custom_rocksdb.a correctness_test.cc
	$(CXX) $(CXXFLAGS) -g correctness_test.cc -o$@ ./lib/custom_rocksdb.a -I../rocksdb-6.15.5/include -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

clean:
	rm -rf ./test_with_* ./lib/origin_rocksdb* ./lib/custom_rocksdb* ./correctness_test

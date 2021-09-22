cd ../rocksdb-6.15.5
make clean
make -j10 LIBNAME=../experiment/lib/origin_rocksdb
cd ../experiment
make test_with_origin_rocksdb -j10
cd ../rocksdb-6.15.5
make clean
make -j10 LIBNAME=../experiment/lib/custom_rocksdb SEQ_FILTER=1
cd ../experiment
make test_with_custom_rocksdb -j10
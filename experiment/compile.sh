cd ../rocksdb-6.15.5
make -j10 LIBNAME=../experiment/origin_rocksdb
make clean
make -j10 LIBNAME=../experiment/custom_rocksdb
make clean
cd ../experiment
make -j10
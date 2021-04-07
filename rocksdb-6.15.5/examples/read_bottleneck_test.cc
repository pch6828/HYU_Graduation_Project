#ifndef ROCKSDB_LITE

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/table.h"

#include <iostream>
#include <ctime>
#include <string>
using namespace ROCKSDB_NAMESPACE;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_read_bottleneck_test";
#else
std::string kDBPath = "/tmp/rocksdb_read_bottleneck_test";
#endif

int main(int argc, char* argv[]) {
  Options options;
  TransactionDBOptions txn_db_options;
  BlockBasedTableOptions table_options;
  options.create_if_missing = true;
  TransactionDB* txn_db;

  table_options.no_block_cache = true;
  options.write_buffer_size = 4*1024*1024;
  options.max_bytes_for_level_base = 8*1024*1024;
  options.max_bytes_for_level_multiplier = 2;
  options.level0_file_num_compaction_trigger = 2;
  options.level0_slowdown_writes_trigger = 3;
  options.level0_stop_writes_trigger = 4;
  options.num_levels = 5;
  options.table_factory.reset(NewBlockBasedTableFactory(table_options));
  
  options.statistics = rocksdb::CreateDBStatistics();
  options.statistics->set_stats_level(kAll);
  
  // delete existing db
  DestroyDB(kDBPath, options);
  // open DB
  Status s = TransactionDB::Open(options, txn_db_options, kDBPath, &txn_db);
  assert(s.ok());

  WriteOptions write_options;
  ReadOptions read_options;
  TransactionOptions txn_options;
  std::string value;

  ////////////////////////////////////////////////////////
  //
  // "Repeatable Read" (Snapshot Isolation) Example
  //   -- Using a single Snapshot
  //
  ////////////////////////////////////////////////////////
  double experiment_time;
  int check_interval;
  if(argc > 1){
    experiment_time = std::stod(std::string(argv[1]));
  }else{
    experiment_time = 300;
  }

  time_t start, now;

  srand(time(NULL));
  std::string random_value(1024, 'a');
  std::string random_key(4, 'a');

  int initial_size = rand()%10000+1;
  while(initial_size--){
    for (auto& c : random_value) {
      c = rand()%26+'a';
    }
    for (auto& c : random_key) {
      c = rand()%26+'a';
    }
    s = txn_db->Put(write_options, random_key, random_value.c_str());
    assert(s.ok()); 
  }
  
  start = time(NULL);

  // setting snapshot isolation
  txn_options.set_snapshot = true;
  Transaction* txn = txn_db->BeginTransaction(write_options, txn_options);

  const Snapshot* snapshot = txn->GetSnapshot();
  read_options.snapshot = snapshot;
  int cnt = 0;
  while (true) {
    // Write a key OUTSIDE of transaction
    for (auto& c : random_value) {
      c = rand()%26+'a';
    }
    for (auto& c : random_key) {
      c = rand()%26+'a';
    }
    s = txn_db->Put(write_options, random_key, random_value.c_str());
    assert(s.ok());
    double txn_lifetime;
    now = time(NULL);
    cnt++;
    if(cnt == 10000){
      cnt = 0;
      // randomize next key-value pair
      for (auto& c : random_key) {
        c = rand()%26+'a';
      }

      s = txn->Get(read_options, random_key, &value);
      std::string prop;
      txn_db->GetProperty("rocksdb.cf-file-histogram", &prop);
      std::cout<<"<<<"<<std::endl;
      std::cout<<prop<<std::endl;
      std::cout<<">>>"<<std::endl;
    }
    txn_lifetime = (double)(now-start);
    if(txn_lifetime > experiment_time){
      break;
    }
  }

  s = txn->Commit();
  assert(s.ok());
  // Snapshot will be released upon deleting the transaction.
  delete txn;
  // Clear snapshot from read options since it is no longer valid
  read_options.snapshot = nullptr;
  snapshot = nullptr;

  // Cleanup
  delete txn_db;
  DestroyDB(kDBPath, options);
  return 0;
}

#endif  // ROCKSDB_LITE

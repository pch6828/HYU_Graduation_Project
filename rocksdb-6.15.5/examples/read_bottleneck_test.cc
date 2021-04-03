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
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_transaction_example";
#else
std::string kDBPath = "/tmp/rocksdb_transaction_example";
#endif

int main(int argc, char* argv[]) {
  Options options;
  TransactionDBOptions txn_db_options;
  BlockBasedTableOptions table_options;
  options.create_if_missing = true;
  TransactionDB* txn_db;

  // options setting
  table_options.no_block_cache = true;
  
  options.write_buffer_size = 4*1024*1024;
  options.max_bytes_for_level_base = 8*1024*1024;
  options.max_bytes_for_level_multiplier = 2;
  options.level0_file_num_compaction_trigger = 2;
  options.level0_slowdown_writes_trigger = 3;
  options.level0_stop_writes_trigger = 4;
  options.num_levels = 5;
  options.table_factory.reset(NewBlockBasedTableFactory(table_options));

  // delete existing db
  DestroyDB(kDBPath, options);
  rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
  
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
  s = txn_db->Put(write_options, random_key, random_value.c_str());
  assert(s.ok()); 

  // pre-inserting all datas
  for(int a = 0; a < 26; a++){
    for(int b = 0; b < 26; b++){
      for(int c = 0; c < 26; c++){
        for(int d = 0; d < 26; d++){
          random_key[0] = a+'a';
          random_key[1] = b+'a';
          random_key[2] = c+'a';
          random_key[3] = d+'a';
          
          for (auto& c : random_value) {
            c = rand()%26+'a';
          }
          s = txn_db->Put(write_options, random_key, random_value.c_str());
          assert(s.ok()); 
        }
      }
    }
  }
  
  start = time(NULL);
  
  // setting snapshot isolation
  txn_options.set_snapshot = true;
  Transaction* txn = txn_db->BeginTransaction(write_options, txn_options);
  const Snapshot* snapshot = txn->GetSnapshot();
  read_options.snapshot = snapshot;
  int cnt = 0;
  double prev = 0;
  uint64_t average_latency = 0;
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

    // randomize next key-value pair
    for (auto& c : random_value) {
      c = rand()%26+'a';
    }
    for (auto& c : random_key) {
      c = rand()%26+'a';
    }

    rocksdb::get_perf_context()->Reset();
    s = txn->Get(read_options, "abcd", &value);
    assert(s.ok());
    auto pc = rocksdb::get_perf_context();
    now = time(NULL);
    double txn_lifetime = (double)(now-start);
    uint64_t latency = 0;
    latency += pc->get_snapshot_time;
    latency += pc->get_from_memtable_time;
    latency += pc->get_from_output_files_time;
    latency += pc->block_read_time;
    latency += pc->block_checksum_time;
    latency += pc->get_post_process_time;
    // latency += pc->new_table_block_iter_nanos;
    // latency += pc->block_seek_nanos;


    std::cout << txn_lifetime <<"\t"<< latency << std::endl;

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

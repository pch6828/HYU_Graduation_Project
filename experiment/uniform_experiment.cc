#ifndef ROCKSDB_LITE

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "rocksdb/db.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/options.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/slice.h"
#include "rocksdb/table.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
using namespace ROCKSDB_NAMESPACE;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_uniform_experiment";
#else
std::string kDBPath = "/tmp/rocksdb_uniform_experiment";
#endif

int main(int argc, char *argv[])
{
  Options options;
  TransactionDBOptions txn_db_options;
  BlockBasedTableOptions table_options;
  options.create_if_missing = true;
  TransactionDB *txn_db;

  // Setting for Options
  options.write_buffer_size = 4 * 1024 * 1024;
  options.max_bytes_for_level_base = 8 * 1024 * 1024;
  options.max_bytes_for_level_multiplier = 2;
  options.level0_file_num_compaction_trigger = 2;
  options.level0_slowdown_writes_trigger = 3;
  options.level0_stop_writes_trigger = 4;
  options.num_levels = 5;

  options.statistics = rocksdb::CreateDBStatistics();
  options.statistics->set_stats_level(StatsLevel::kExceptTimeForMutex);

  rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
  rocksdb::get_perf_context()->EnablePerLevelPerfContext();

  // Delete Existing DB
  DestroyDB(kDBPath, options);

  // Open DB
  Status s = TransactionDB::Open(options, txn_db_options, kDBPath, &txn_db);
  assert(s.ok());

  WriteOptions write_options;
  ReadOptions read_options;
  TransactionOptions txn_options;
  std::string value;

  // Experiment Time (seconds)
  // Default Value is 100 seconds
  double experiment_time = 100;

  time_t start, now;

  srand(time(NULL));
  std::string random_value(1024, 'a');
  std::string random_key(4, 'a');

  // Initialize DB with Random Key-Value Pairs
  int initial_size = 1000;
  while (initial_size--)
  {
    for (auto &c : random_value)
    {
      c = rand() % 26 + 'a';
    }
    for (auto &c : random_key)
    {
      c = rand() % 26 + 'a';
    }
    s = txn_db->Put(write_options, random_key, random_value.c_str());
    assert(s.ok());
  }
  s = txn_db->Put(write_options, random_key, random_value.c_str());
  assert(s.ok());

  start = time(NULL);

  // Setting Snapshot Isolation
  txn_options.set_snapshot = true;
  Transaction *txn = txn_db->BeginTransaction(write_options, txn_options);

  const Snapshot *snapshot = txn->GetSnapshot();
  read_options.snapshot = snapshot;
  int cnt = 0, get_cnt = 0;

  // Experiment Start
  while (true)
  {
    // Write a key OUTSIDE of transaction
    for (auto &c : random_value)
    {
      c = rand() % 26 + 'a';
    }
    for (auto &c : random_key)
    {
      c = rand() % 26 + 'a';
    }
    s = txn_db->Put(write_options, random_key, random_value.c_str());
    assert(s.ok());
    double txn_lifetime;
    now = time(NULL);
    cnt++;

    // Read/Write Ratio => 1 : 2
    if (cnt == 2)
    {
      cnt = 0;
      get_cnt++;
      // Randomize Key for Get Operation
      for (auto &c : random_key)
      {
        c = rand() % 26 + 'a';
      }

      // Read Old Snapshot
      s = txn->Get(read_options, random_key, &value);
      auto pc = rocksdb::get_perf_context();
      // Get Statistics (Get Operation Latency)

      std::cout << pc->get_snapshot_time / get_cnt << "\t";
      std::cout << pc->get_from_memtable_time / get_cnt << "\t";
      for (int i = 0; i < 5; i++)
      {
        if ((pc->level_to_perf_context)->count(i))
        {
          std::cout << (*(pc->level_to_perf_context))[i].get_from_table_nanos / get_cnt << "\t";
        }
        else
        {
          std::cout << 0 << "\t";
        }
      }
      std::cout << pc->get_post_process_time / get_cnt << "\n";
    }

    // Stop Experiment If Transaction Lifetime Exceeds Pre-defined Experiment
    // Time
    txn_lifetime = (double)(now - start);
    if (txn_lifetime > experiment_time)
    {
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

#endif // ROCKSDB_LITE

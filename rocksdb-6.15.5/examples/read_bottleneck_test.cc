#ifndef ROCKSDB_LITE

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"

#include <iostream>
using namespace ROCKSDB_NAMESPACE;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_transaction_example";
#else
std::string kDBPath = "/tmp/rocksdb_transaction_example";
#endif

int main() {
  // open DB
  Options options;
  TransactionDBOptions txn_db_options;
  options.create_if_missing = true;
  TransactionDB* txn_db;

  rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);

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
  std::string put_value = "ABC";
  s = txn_db->Put(write_options, "abc", put_value.c_str());
  assert(s.ok()); 

  // Set a snapshot at start of transaction by setting set_snapshot=true
  txn_options.set_snapshot = true;
  Transaction* txn = txn_db->BeginTransaction(write_options, txn_options);

  const Snapshot* snapshot = txn->GetSnapshot();
  read_options.snapshot = snapshot;
  int cnt = 0;
  while (true) {
    cnt++;
    // Write a key OUTSIDE of transaction
    put_value.push_back(put_value.front());
    put_value.erase(0, 1);
    s = txn_db->Put(write_options, "abc", put_value.c_str());
    assert(s.ok());
    if (cnt % 10000 == 0) {
      rocksdb::get_perf_context()->Reset();
      s = txn->Get(read_options, "abc", &value);
      assert(s.ok());
      assert(value == "ABC");
      cnt = 0;
      auto pc = rocksdb::get_perf_context();
      std::cout<<pc->get_from_memtable_time+pc->get_from_output_files_time<<std::endl;
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

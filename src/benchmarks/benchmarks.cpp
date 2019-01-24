#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "success.hpp"
#include "lmdb.h"
#include "rewind.h"

const bool USE_LMDB = false;
const bool USE_REWIND = true;

void run_benchmark(bool use_rewind, char* path, int number_of_txn, int number_of_puts_per_txn, int size_of_record) {
    struct stat st = {0};

    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }

    srand(256);

    int rc;
    MDB_env* env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val key;
    MDB_val value;

    if (use_rewind) {
        REQUIRE(SuccessFrom(rew_env_create(&env)));
    } else {
        REQUIRE(SuccessFrom(mdb_env_create(&env)));
    }
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(2048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, path, 0, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    char* value_buffer = (char*)calloc(size_of_record, 1);

    for(int i = 0;i<number_of_txn;i++) {
        MDB_txn *tx;
        mdb_txn_begin(env, nullptr, 0, &tx);

        for(int y = 0;y<number_of_puts_per_txn;y++) {
            int val = rand()%1024;
            MDB_val key;
            MDB_val value;
            int r = rand();

            key.mv_size = sizeof(int);
            key.mv_data = &r;

            value.mv_size = size_of_record;
            value.mv_data = value_buffer;

            REQUIRE( SuccessFrom( mdb_put(tx, dbi, &key, &value, 0) ) );
        }
        mdb_txn_commit(tx);
    }
    mdb_env_close(env);
}

TEST_CASE("\t[LMDB]\t\t\t random writes 10,000 transactions, 1 keys per transaction, 1000 bytes per key", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark1", 10000, 1, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random writes 10,000 transactions, 1 keys per transaction, 1000 bytes per key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark2", 10000, 1, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random writes 1,000 transactions, 10 keys per transaction, 1000 bytes per key", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark3", 1000, 10, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random writes 1,000 transactions, 10 keys per transaction, 1000 bytes per key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark4", 1000, 10, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random writes 1,000 transactions, 100 keys per transaction, 1000 bytes per key", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark5", 1000, 100, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random writes 1,000 transactions, 100 keys per transaction, 1000 bytes per key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark6", 1000, 100, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random writes 1,000 transactions, 1,000 keys per transaction, 1000 bytes per key", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark7", 1000, 1000, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random writes 1,000 transactions, 1,000 keys per transaction, 1000 bytes per key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark8", 1000, 1000, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random writes 1,000 transactions, 10,000 keys per transaction, 1000 bytes per key", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark9", 1000, 10000, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random writes 1,000 transactions, 10,000 keys per transaction, 1000 bytes per key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark10", 1000, 10000, 1000);
}

//TEST_CASE("\t[LMDB]\t\t\t random writes 1,000 transactions, 100,000 keys per transaction, 1000 bytes per key", "[rewind]") {
//    run_benchmark(USE_LMDB, "/tmp/benchmark11", 1000, 100000, 1000);
//}
//
//TEST_CASE("\t[LMDB with Rewind]\t random writes 1,000 transactions, 100,000 keys per transaction, 1000 bytes per key\n", "[rewind]") {
//    run_benchmark(USE_REWIND, "/tmp/benchmark12", 1000, 100000, 1000);
//}

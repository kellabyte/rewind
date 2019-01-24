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

TEST_CASE("LMDB random writes without Rewind", "[rewind]") {
    run_benchmark(USE_LMDB, "/tmp/benchmark1", 1000, 500, 1000);
}

TEST_CASE("LMDB random writes with Rewind", "[rewind]") {
    run_benchmark(USE_REWIND, "/tmp/benchmark2", 1000, 500, 1000);
}

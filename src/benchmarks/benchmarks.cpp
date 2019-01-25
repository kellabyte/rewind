#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "success.hpp"
#include "lmdb.h"
#include "rewind.h"

const bool USE_LMDB = false;
const bool USE_REWIND = true;

void run_benchmark(bool use_rewind, char* benchmark_name, int number_of_txn, int number_of_puts_per_txn, int size_of_record) {
    const char* base_path = "/tmp/";
    size_t path_length = strlen(base_path);
    char* benchmark_path = (char*)malloc(strlen(base_path) + strlen(benchmark_name) + 1);
    strcpy(benchmark_path, base_path);
    strcat(benchmark_path, benchmark_name);

    struct stat st = {0};

    if (stat(benchmark_path, &st) == -1) {
        mkdir(benchmark_path, 0700);
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
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(200048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, benchmark_path, 0, 0664) ) );
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

TEST_CASE("\t[LMDB]\t\t\t random write 10,000 txn, 1 keys/txn, 1,000 bytes/key", "[rewind]") {
    run_benchmark(USE_LMDB, "benchmark1", 10000, 1, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random write 10,000 txn, 1 keys/txn, 1,000 bytes/key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "benchmark2", 10000, 1, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random write 1,000 txn, 10 keys/txn, 1,000 bytes/key", "[rewind]") {
    run_benchmark(USE_LMDB, "benchmark3", 1000, 10, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random write 1,000 txn, 10 keys/txn, 1,000 bytes/key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "benchmark4", 1000, 10, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random write 1,000 txn, 100 keys/txn, 1,000 bytes/key", "[rewind]") {
    run_benchmark(USE_LMDB, "benchmark5", 1000, 100, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random write 1,000 txn, 100 keys/txn, 1,000 bytes/key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "benchmark6", 1000, 100, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random write 1,000 txn, 1,000 keys/txn, 1,000 bytes/key", "[rewind]") {
    run_benchmark(USE_LMDB, "benchmark7", 1000, 1000, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random write 1,000 txn, 1,000 keys/txn, 1,000 bytes/key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "benchmark8", 1000, 1000, 1000);
}

TEST_CASE("\t[LMDB]\t\t\t random write 1,000 txn, 10,000 keys/txn, 1,000 bytes/key", "[rewind]") {
    run_benchmark(USE_LMDB, "benchmark9", 1000, 10000, 1000);
}

TEST_CASE("\t[LMDB with Rewind]\t random write 1,000 txn, 10,000 keys/txn, 1,000 bytes/key\n", "[rewind]") {
    run_benchmark(USE_REWIND, "benchmark10", 1000, 10000, 1000);
}

//TEST_CASE("\t[LMDB]\t\t\t random write 1,000 txn, 100,000 keys/txn, 1,000 bytes/key", "[rewind]") {
//    run_benchmark(USE_LMDB, "benchmark11", 1000, 100000, 1000);
//}
//
//TEST_CASE("\t[LMDB with Rewind]\t random write 1,000 txn, 100,000 keys/txn, 1,000 bytes/key\n", "[rewind]") {
//    run_benchmark(USE_REWIND, "benchmark12", 1000, 100000, 1000);
//}

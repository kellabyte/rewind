#define CATCH_CONFIG_MAIN
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "success.hpp"
#include "lmdb.h"
#include "rewind.h"

TEST_CASE("LMDB random writes without Rewind", "[rewind]") {
    const char* path = "/tmp/benchmark1";

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

    REQUIRE( SuccessFrom( mdb_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(2048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, path, 0, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    int batches = 1000;
    int records_per_patch = 500;
    int size_of_value = 1000;
    char* value_buffer = (char*)malloc(size_of_value);

    for(int i = 0;i<batches;i++) {
        MDB_txn *tx;
        mdb_txn_begin(env, nullptr, 0, &tx);

        for(int y = 0;y<records_per_patch;y++) {
            int val = rand()%1024;
            MDB_val key;
            MDB_val value;
            int r = rand();

            key.mv_size = sizeof(int);
            key.mv_data = &r;

            value.mv_size = size_of_value;
            value.mv_data = value_buffer;

            REQUIRE( SuccessFrom( mdb_put(tx, dbi, &key, &value, 0) ) );
        }
        mdb_txn_commit(tx);
    }
    mdb_env_close(env);
}

TEST_CASE("LMDB random writes with Rewind", "[rewind]") {
    const char* path = "/tmp/benchmark2";

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

    REQUIRE( SuccessFrom( rew_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, path, 0, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    int batches = 1000;
    int records_per_patch = 500;
    int size_of_value = 1000;
    char* value_buffer = (char*)malloc(size_of_value);

    for(int i = 0;i<batches;i++) {
        MDB_txn *tx;
        mdb_txn_begin(env, nullptr, 0, &tx);

        for(int y = 0;y<records_per_patch;y++) {
            int val = rand()%1024;
            MDB_val key;
            MDB_val value;
            int r = rand();

            key.mv_size = sizeof(int);
            key.mv_data = &r;

            value.mv_size = size_of_value;
            value.mv_data = value_buffer;

            REQUIRE( SuccessFrom( mdb_put(tx, dbi, &key, &value, 0) ) );
        }
        mdb_txn_commit(tx);
    }
    mdb_env_close(env);
}

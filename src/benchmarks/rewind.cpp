#define CATCH_CONFIG_MAIN
#include <time.h>
#include <stdlib.h>
#include "catch.hpp"
#include "lmdb.h"
#include "rewind.h"

const int SUCCESS = 0;

TEST_CASE("random writes", "[rewind]") {
    srand(256);

    int rc;
    MDB_env* env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val key;
    MDB_val value;

    rc = mdb_env_create(&env);
    CHECK(rc == SUCCESS);

    rc = mdb_env_set_mapsize(env, size_t(1048576000));
    CHECK(rc == SUCCESS);

    rc = mdb_env_set_maxdbs(env, 1024);
    CHECK(rc == SUCCESS);

    rc = mdb_env_open(env, "./", 0, 0664);
    CHECK(rc == SUCCESS);

    rc = mdb_txn_begin(env, nullptr, 0, &txn);
    CHECK(rc == SUCCESS);

    rc = mdb_open(txn, nullptr, 0, &dbi);
    CHECK(rc == SUCCESS);

    int count = 500000;
    int *values;
    values = (int *)malloc(count*sizeof(int));

    for(int i = 0;i<count;i++) {
        values[i] = rand()%1024;
    }

    for (int i = 0; i<count; i++) {
        int r = rand();

        key.mv_size = sizeof(int);
        key.mv_data = &r;

        value.mv_size = sizeof(values[i]);
        value.mv_data = &values[i];

        rc = mdb_put(txn, dbi, &key, &value, 0);
        CHECK(rc == SUCCESS);
    }

    rc = mdb_txn_commit(txn);
    CHECK(rc == SUCCESS);

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}
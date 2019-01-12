#include "catch.h"
extern "C" {
    #include "lmdb.h"
    #include "rewind.h"
}

const int SUCCESS = 0;

TEST_CASE("initialize rewind", "[rewind]") {
    int rc;
    MDB_env* env;
    MDB_dbi dbi;
    MDB_txn *txn;

    WHEN("creating a new env") {
        rc = mdb_env_create(&env);
        if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

        THEN("set map size") {
            rc = mdb_env_set_mapsize(env, size_t(1048576000));
            if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

            THEN("set maxdbs") {
                rc = mdb_env_set_maxdbs(env, 1024);
                if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

                THEN("env open") {
                    rc = mdb_env_open(env, "/tmp", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664);
                    if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

                    THEN("begin transaction") {
                        rc = mdb_txn_begin(env, NULL, 0, &txn);
                        if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

                        THEN("open dbi") {
                            rc = mdb_dbi_open(txn, NULL, 0, &dbi);
                            if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

                            THEN("open transaction") {
                                rc = mdb_open(txn, NULL, 0, &dbi);
                                if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);

                                THEN("commit transaction") {
                                    rc = mdb_txn_commit(txn);
                                    if (rc != 0) { WARN(mdb_strerror(rc)); } REQUIRE(rc == SUCCESS);
                                }
                            }
                        }
                    }
                }
            }
        }

        mdb_dbi_close(env, dbi);
        mdb_env_close(env);
    }
}
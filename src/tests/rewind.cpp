#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "lmdb.h"
#include "rewind.h"
#include "success.hpp"

TEST_CASE("initialize rewind", "[rewind]") {
    int rc;
    MDB_env* env;
    MDB_dbi dbi;
    MDB_txn *txn;

    REQUIRE( SuccessFrom( mdb_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, "/tmp", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, NULL, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, NULL, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, NULL, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}
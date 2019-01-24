#include "catch.hpp"
#include "lmdb.h"
#include "rewind.h"
#include "success.hpp"

TEST_CASE("initialize LMDB without Rewind", "[rewind]") {
    MDB_env* env = nullptr;
    MDB_dbi dbi = NULL;
    MDB_txn* txn = nullptr;

    REQUIRE( SuccessFrom( mdb_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, "/tmp", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

TEST_CASE("initialize LMDB with Rewind", "[rewind]") {
    MDB_env* env = nullptr;
    MDB_dbi dbi = NULL;
    MDB_txn* txn = nullptr;

    REQUIRE( SuccessFrom( rew_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, "/tmp", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

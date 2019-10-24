#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>
#include "catch.hpp"
#include "lmdb.h"
#include "rewind.h"
#include "success.hpp"

const char* TEST_PATH = "/tmp/rewind_test";

static int delete_directory(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if(remove(pathname) < 0)
    {
        perror("ERROR: remove");
        return -1;
    }
    return 0;
}

void create_test_path(const char* test_path) {
    struct stat st = {0};

    int found = stat(test_path, &st);
    if (found == 0) {
        // Path already exists. Delete it.
        if (nftw(test_path, delete_directory, 10, FTW_DEPTH|FTW_MOUNT|FTW_PHYS) < 0)
        {
            perror("ERROR: ntfw");
            exit(1);
        }
    }
    mkdir(test_path, 0700);
}

TEST_CASE("initialize LMDB without Rewind", "[rewind]") {
    create_test_path(TEST_PATH);

    MDB_env* env = nullptr;
    MDB_dbi dbi = NULL;
    MDB_txn* txn = nullptr;

    REQUIRE( SuccessFrom( mdb_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, TEST_PATH, MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

TEST_CASE("initialize LMDB with Rewind", "[rewind]") {
    create_test_path(TEST_PATH);

    MDB_env* env = nullptr;
    MDB_dbi dbi = NULL;
    MDB_txn* txn = nullptr;

    REQUIRE( SuccessFrom( rew_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, TEST_PATH, MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

TEST_CASE("set key LMDB with Rewind", "[rewind]") {
    create_test_path(TEST_PATH);

    MDB_env* env = nullptr;
    MDB_dbi dbi = NULL;
    MDB_txn* txn = nullptr;
    const char* key_string = "foo";
    const char* value_string = "bar";
    MDB_val key;
    MDB_val value;

    key.mv_size = sizeof(strlen(key_string));
    key.mv_data = &key_string;
    value.mv_size = sizeof(strlen(value_string));
    value.mv_data = &value_string;


    REQUIRE( SuccessFrom( rew_env_create(&env) ) );
    REQUIRE( SuccessFrom( mdb_env_set_mapsize(env, size_t(1048576000) ) ) );
    REQUIRE( SuccessFrom( mdb_env_set_maxdbs(env, 1024) ) );
    REQUIRE( SuccessFrom( mdb_env_open(env, TEST_PATH, MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664) ) );
    REQUIRE( SuccessFrom( mdb_txn_begin(env, nullptr, 0, &txn) ) );
    REQUIRE( SuccessFrom( mdb_dbi_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_open(txn, nullptr, 0, &dbi) ) );
    REQUIRE( SuccessFrom( mdb_put(txn, dbi, &key, &value, 0) ) );
    REQUIRE( SuccessFrom( mdb_txn_commit(txn) ) );

    sleep(3);

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

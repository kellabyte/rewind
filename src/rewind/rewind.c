#include "stdio.h"
#include "lmdb.h"

int re_env_create(MDB_env **env) {
    return mdb_env_create(env);
}
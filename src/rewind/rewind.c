#include "lmdb.h"

struct REW_env {

};

int rew_env_create(MDB_env **env) {
    return mdb_env_create(env);
}

int rew_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data) {
    return mdb_get(txn, dbi, key, data);
}

int rew_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data, unsigned int flags) {
    return mdb_put(txn, dbi, key, data, flags);
}

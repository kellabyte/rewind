#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "lmdb.h"

typedef struct REW_env {
    MDB_env* mdb_log_env;
    MDB_dbi mdb_log_dbi;
    MDB_cursor* mdb_log_cursor;
    MDB_env* mdb_main_env;
    char* path;
    unsigned int last_durable_sequence;
    unsigned int current_sequence;
}REW_env;

int start_sync(REW_env* rew_env);
void* sync_loop(void* data);

int rew_env_create(MDB_env** mdb_main_env) {
    int rc = 0;
    REW_env* rew_env = malloc(sizeof(REW_env));

    // Create the main database file environment.
    rc = mdb_env_create(mdb_main_env);
    if (rc != 0) {
        return rc;
    }
    rew_env->mdb_main_env = mdb_main_env;

    // Create the log database file.
    MDB_env* mdb_log_env;
    rc = mdb_env_create(&mdb_log_env);
    if (rc != 0) {
        return rc;
    }

    rew_env->last_durable_sequence = 0;
    rew_env->current_sequence = 0;
    rew_env->mdb_log_env = mdb_log_env;
    rc = mdb_env_set_userctx(*mdb_main_env, rew_env);
    if (rc != 0) {
        return rc;
    }
    rc = mdb_env_set_userctx(mdb_log_env, rew_env);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int re_env_create(MDB_env** env) {
    return mdb_env_create(env);
}

int re_env_open(MDB_env *env, const char *path, unsigned int flags, mdb_mode_t mode) {
    int rc = 0;

    // Open the log file.
    struct stat st = {0};

    REW_env* rew_env = mdb_env_get_userctx(env);
    if (rew_env != NULL) {
        size_t path_length = strlen(path);
        rew_env->path = (char*)malloc(path_length + 6);
        strcpy(rew_env->path, path);
        strcat(rew_env->path, "/log/");

        if (stat(rew_env->path, &st) == -1) {
            rc = mkdir(rew_env->path, 0700);
            if (rc != 0) {
                // TODO. We aren't returning an MDB valid error here.
                // We should maybe define a Rewind specific error here.
                return rc;
            }
        }

        rc = mdb_env_set_mapsize(rew_env->mdb_log_env, 2048576000);
        if (rc != 0) {
            return rc;
        }

        rc = mdb_env_set_maxdbs(rew_env->mdb_log_env, 1024);
        if (rc != 0) {
            return rc;
        }

        rc = mdb_env_open(rew_env->mdb_log_env, rew_env->path, 0, 0664);
        if (rc != 0) {
            return rc;
        }

        MDB_txn *txn;
        rc = mdb_txn_begin(rew_env->mdb_log_env, NULL, 0, &txn);
        if (rc != 0) {
            return rc;
        }
        rc = mdb_dbi_open(txn, NULL, MDB_INTEGERKEY, &rew_env->mdb_log_dbi);
        if (rc != 0) {
            return rc;
        }
        rc = mdb_txn_commit(txn);
        if (rc != 0) {
            return rc;
        }
        //printf("REW_ENV_OPEN\n");
    }

    // Open the main LMDB database.
    rc = mdb_env_open(env, path, flags, mode);
    if (rc != 0 ) {
        return rc;
    }
    //printf("MDB_ENV_OPEN\n");

    // Start sync thread that synchronizes data between the log and the main database.
    start_sync(rew_env);
    return rc;
}

int re_txn_begin(MDB_env *env, MDB_txn *parent, unsigned int flags, MDB_txn **txn) {
    REW_env* rew_env = mdb_env_get_userctx(env);
    if (rew_env != NULL) {
        int rc = 0;

        rc = mdb_txn_begin(rew_env->mdb_log_env, parent, flags, txn);
        if (rc != 0) {
            return rc;
        }

        rc = mdb_cursor_open(*txn, rew_env->mdb_log_dbi, &rew_env->mdb_log_cursor);
        if (rc != 0) {
            return rc;
        }
        //printf("REW_BEGIN\n");
        return rc;
    } else {
        //printf("MDB_BEGIN\n");
        return mdb_txn_begin(env, parent, flags, txn);
    }
}

int re_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data) {
    return mdb_get(txn, dbi, key, data);
}

int re_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data, unsigned int flags) {
    MDB_env* mdb_log_env = mdb_txn_env(txn);
    if (mdb_log_env == NULL) {
        // TODO: Throw proper error.
        return -1;
    }

    REW_env* rew_env = mdb_env_get_userctx(mdb_log_env);
    if (rew_env != NULL) {
        MDB_val sequence;

        sequence.mv_size = sizeof(unsigned int);
        sequence.mv_data = &rew_env->current_sequence;
        int rc = mdb_cursor_put(rew_env->mdb_log_cursor, &sequence, data, MDB_APPEND);
        if (rc != 0) {
            return rc;
        }
        rew_env->current_sequence++;
        //printf("REW_PUT\n");
        return rc;
    } else {
        //printf("MDB_PUT\n");
        return mdb_put(txn, dbi, key, data, flags);
    }
}

int re_txn_commit(MDB_txn *txn) {
    MDB_env* mdb_env = mdb_txn_env(txn);
    if (mdb_env == NULL) {
        // TODO: Throw error.
        return -1;
    }

    REW_env* rew_env = mdb_env_get_userctx(mdb_env);
    if (rew_env != NULL) {
        mdb_cursor_close(rew_env->mdb_log_cursor);
        //printf("REW_COMMIT\n");
    }
    //printf("MDB_COMMIT\n");
    return mdb_txn_commit(txn);
}

int re_txn_abort(MDB_txn *txn) {
    mdb_txn_abort(txn);
}

void re_dbi_close(MDB_env *env, MDB_dbi dbi) {
    REW_env* rew_env = mdb_env_get_userctx(env);
    mdb_dbi_close(env, dbi);
}

void re_env_close(MDB_env *env) {
    REW_env* rew_env = mdb_env_get_userctx(env);
    if (rew_env != NULL) {
        mdb_env_close(rew_env->mdb_log_env);
    }
    mdb_env_close(env);
}

int start_sync(REW_env* rew_env) {
    pthread_t sync_thread;
    if(pthread_create(&sync_thread, NULL, sync_loop, rew_env)) {
        return 1;
    }
    return 0;
}

void* sync_loop(void* data) {
    REW_env* rew_env = (REW_env*)data;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {

        //MDB_dbi dbi;
        //MDB_cursor *cursor;
        //if (int rc = mdb_cursor_open(d->transaction, d->dbi, &cursor)) {
        //    //Error
        //}
        //
        ////Initialize the key with the key we're looking for
        //MDB_val key = {(size_t)someString.size(), (void *)someString.data()};
        //MDB_val data;
        //
        ////Position the cursor, key and data are available in key
        //if (int rc = mdb_cursor_get(cursor, &key, &data, MDB_SET_RANGE)) {
        //    //No value found
        //    mdb_cursor_close(cursor);
        //    return 0;
        //}
        //
        ////Position the curser at the next position
        //mdb_cursor_get(cursor, &key, &data, MDB_NEXT)
        //
        //mdb_cursor_close(cursor);
        sleep(1);

        int rc = 0;
        MDB_dbi dbi;
        MDB_txn *txn;
        MDB_cursor *log_cursor;
        MDB_val log_cursor_key;
        MDB_val log_cursor_value;
        log_cursor_key.mv_size = sizeof(unsigned int);
        log_cursor_key.mv_data = &rew_env->last_durable_sequence;

        // TODO: Handle error return codes.
        rc = mdb_txn_begin(rew_env->mdb_log_env, NULL, NULL, &txn);
        //rc = mdb_dbi_open(txn, NULL, 0, &dbi);
        rc = mdb_cursor_open(txn, rew_env->mdb_log_dbi, &log_cursor);
        rc = mdb_cursor_get(log_cursor, &log_cursor_key, &log_cursor_value, MDB_FIRST);
        printf("%d\n", log_cursor_value.mv_data);
        rc = mdb_cursor_get(log_cursor, &log_cursor_key, &log_cursor_value, MDB_NEXT);
        printf("%d\n", log_cursor_value.mv_data);

    }
#pragma clang diagnostic pop
    return NULL;
}

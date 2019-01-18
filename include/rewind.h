/*-------------------------------------------------------------------------
 *
 * Overview
 *      Primary include file for Rewind.
 *
 *-------------------------------------------------------------------------
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "lmdb.h"

#ifdef _WIN32
    /* Windows - set up dll import/export decorators. */
#ifdef BUILDING_REWIND_SHARED
    /* Building shared library. */
#define REWIND_EXTERN __declspec(dllexport)
#else
#ifdef USING_REWIND_SHARED
    /* Using shared library. */
#define REWIND_EXTERN __declspec(dllimport)
#else
    /* Building static library. */
#define REWIND_EXTERN /* nothing */
#endif
#endif

#define REWIND_CALLING_CONVENTION __cdecl
#else
    /* Building static library. */
#define REWIND_EXTERN /* nothing */
#define REWIND_CALLING_CONVENTION /* nothing */
#endif

/*
 * LMDB API calls that Rewind intercepts.
 */
#define mdb_env_create(env) re_env_create(env)
int re_env_create(MDB_env **env);

#define mdb_env_open(env, path, flags, mode) re_env_open(env, path, flags, mode)
int re_env_open(MDB_env *env, const char *path, unsigned int flags, mdb_mode_t mode);

#define mdb_txn_begin(env, parent, flags, txn) re_txn_begin(env, parent, flags, txn)
int re_txn_begin(MDB_env *env, MDB_txn *parent, unsigned int flags, MDB_txn **txn);

#define mdb_txn_commit(txn) re_txn_commit(txn)
int re_txn_commit(MDB_txn *txn);

#define mdb_txn_abort(txn) re_txn_abort(txn)
int re_txn_abort(MDB_txn *txn);

#define mdb_put(txn, dbi, key, data, flags) re_put(txn, dbi, key, data, flags)
int re_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data, unsigned int flags);

#define mdb_get(txn, dbi, key, data) re_get(txn, dbi, key, data)
int re_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data);

#define mdb_dbi_close(env, dbi) re_dbi_close(env, dbi)
void re_dbi_close(MDB_env *env, MDB_dbi dbi);

#define mdb_env_close(env) re_env_close(env)
void re_env_close(MDB_env *env);

/*
 * Rewind public API.
 */
/** @brief Opaque structure for a database environment.
 *
 * A DB environment supports multiple databases, all residing in the same
 * shared-memory map.
 */
typedef struct REW_env REW_env;

int rew_env_create(MDB_env** env);

#ifdef __cplusplus
}
#endif
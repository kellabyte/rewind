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
#include <uv.h>
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
#define mdb_env_create(env) rew_env_create(env)
int rew_env_create(MDB_env **env);

#define mdb_put(txn, dbi, key, data, flags) rew_put(txn, dbi, key, data, flags)
int rew_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data, unsigned int flags);

#define mdb_get(txn, dbi, key, data) rew_get(txn, dbi, key, data)
int rew_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data);

/** @brief Opaque structure for a database environment.
 *
 * A DB environment supports multiple databases, all residing in the same
 * shared-memory map.
 */
typedef struct REW_env REW_env;

#ifdef __cplusplus
}
#endif
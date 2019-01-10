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

#ifdef __cplusplus
}
#endif
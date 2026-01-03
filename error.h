/* error.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef error_h
#define error_h

/**
 * @defgroup error  error routines
 * functions
 *
 * \#include "error.h"
 */


/** internalError
 * @ingroup error
 * @param[in] sig
 *
 */
void internalError(int sig);

#endif


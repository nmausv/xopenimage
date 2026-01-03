/* gamma.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C System */
#include <stdlib.h>
#include <stdio.h>     /* fprintf */

/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "gamma.h"     /* declarations */

/******************/
/* gammacorrect() */
/******************/
void
gammacorrect(
 gImage* ingimageP,
 float inGamma,
 unsigned int inVerbose)
{
  if (inVerbose != 0) {
    fprintf(stderr, "gammacorrect() called\n");
  }
}


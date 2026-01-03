/* usageHelp.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h>  /* exit, EXIT_FAILURE */
#include <stdio.h>   /* printf */

/* code base */
#include "usageHelp.h"  /* declarations, consistency */

/* PUBLIC FUNCTIONS */

/***************/
/* usageHelp() */
/***************/
void
usageHelp(void)
{
  printf("\nUsage: xopenimage [global options] {[image options] image_filename ...}\n\n");
  printf("Type \"xopenimage -help [option ...]\" for information on a particular option,\n\
or \"xopenimage -help\" to enter the interactive help facility.\n");

  exit(EXIT_FAILURE);
}


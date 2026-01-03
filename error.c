/* error.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h>   /* exit, abort, EXIT_FAILURE */
#include <stdio.h>    /* printf, fprintf, snprintf */
#include <signal.h>   /* SIGSEGV etc */

/* code base */
#include "version.h"   /* SOURCE: VERSION, PATCHLEVEL */
#include "build.h"     /* COMPILE TIME: BuildDate, BuildSystem */

#include "error.h"     /* declarations, consistency */


/* internal (static) functions */

/****************/
/* signalName() */
/****************/
static char*
signalName(
 int sig)
{
static char buf[32];

  switch (sig) {
   case SIGSEGV:
    return("SEGV");
   case SIGBUS:
    return("BUS");
   case SIGFPE:
    return("FPE");
   case SIGILL:
    return("ILL");
   default:
    snprintf(buf, 32, "Signal %d", sig);
    return(buf);
  }
}



/* PUBLIC functions */

/*******************/
/* internalError() */
/*******************/
void
internalError(
 int sig)
{
static int handling_error = 0;

  switch(handling_error++) {

    case 0:
      printf("\n\n\
An internal error (%s) has occurred.\n\
If you would like to file a bug report,\n\
then please send email to %s with:\n\
a description of how you triggered the bug, \
all output before the failure, \
and the following information:\n\n",
        signalName(sig), "" /* AUTHOR_EMAIL */ );

      printf("xopenimage version %s.%s\n\n", VERSION, PATCHLEVEL);
      if (BuildDate != NULL) {
        printf("Built on:     %s\n", BuildDate);
      }
      if (BuildSystem != NULL) {
        printf("Build system: %s\n", BuildSystem);
      }

      break;

    case 1:
      fprintf(stderr, "\n\n\
An internal error has occurred within the internal error handler.  No more\n\
information about the error is available, sorry.\n");
      exit(EXIT_FAILURE);
      break;
  }

  abort();
}


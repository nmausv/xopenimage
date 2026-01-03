/* xbitmap_fmt.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* since xbitmap is part of X11 */
/*  will use the X11 library for this format */

/* System */
#include <stdlib.h>
#include <stdio.h>  /* printf(), fprintf() */
#include <string.h> /* memcpy() */

/* X11 */
#include <X11/Xlib.h> /* XReadBitmapFileData() */

/* code base */
#include "../gimage.h"   /* 'gImage' struct */

#include "xbitmap_fmt.h" /* enforce declarations */


/*****************/
/* xbitmapLoad() */
/*****************/
gImage*
xbitmapLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage* gimageP = NULL;
int xret = 0;
unsigned int width = 0;
unsigned int height = 0;
int xhot = 0;
int yhot = 0;
unsigned char *xbm_dataP = NULL;
unsigned int linebytes = 0;

  xret = XReadBitmapFileData(inFilepath, &width, &height,
    &xbm_dataP, &xhot, &yhot);
  if (xret != 0) {
/*     fprintf(stderr, "XReadBitmapFileData error %d\n", xret); */
    gimageP = NULL;

  } else {

    linebytes = (width + 7) / 8;
  
    gimageP = newBitImage(width, height);

    memcpy(gimageP->data, xbm_dataP, linebytes * height );

    strncpy(gimageP->title, inFilepath, 255);
    gimageP->title[255]= '\0';

    if (inVerbose) {
      printf("%s, X11 bitmap, size: %d x %d\n",
        inFilepath, width, height);
    } 

  }

  return(gimageP);
}


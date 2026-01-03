/* gimage.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h> /* malloc, calloc */
#include <stdio.h>  /* printf, fprintf, stderr */

/* code base */
#include "gimage.h" /* declarations, consistency */


/* PUBLIC FUNCTIONS */

/*****************/
/* newBitImage() */
/*****************/
gImage*
newBitImage(
 unsigned int inWidth,
 unsigned int inHeight)
{
gImage *gimageP = NULL;

  /* allocate struct */
  gimageP = malloc(sizeof(gImage));
  if (gimageP == NULL) {
    fprintf(stderr, "xopenimage newBitImage malloc fail\n");

  } else {

    gimageP->data = calloc( inHeight * ((inWidth + 7) / 8 ) , sizeof(unsigned char) );
    if (gimageP->data == NULL) {
      fprintf(stderr, "xopenimage newBitImage calloc fail\n");
      free(gimageP);
      gimageP = NULL;

    } else {

      gimageP->gitype   = IBITMAP;
      gimageP->width    = inWidth;
      gimageP->height   = inHeight;
      gimageP->depth    = 1; /* redundant since IBITMAP */
      gimageP->gamma    = 1.0; /* not appropriate for bitmap */
      gimageP->title[0] = '\0';

      gimageP->background[0] = '\0'; /* is used for bitmap */
      gimageP->foreground[0] = '\0'; /* is used for bitmap */

    }
  }
  return(gimageP);
}


/*******************/
/* newRGB24Image() */
/*******************/
gImage*
newRGB24Image(
 unsigned int inWidth,
 unsigned int inHeight)
{
gImage *gimageP = NULL;

  /* allocate struct */
  gimageP = malloc(sizeof(gImage));
  if (gimageP == NULL) {
    fprintf(stderr, "xopenimage newRGB24Image malloc fail\n");
  } else {

    gimageP->data = calloc(inHeight * inWidth * 3, sizeof(unsigned char) );
    if (gimageP->data == NULL) {
      fprintf(stderr, "xopenimage newRGB24Image calloc fail\n");
      free(gimageP);
      gimageP = NULL;

    } else {

      gimageP->gitype   = IRGB24;
      gimageP->width    = inWidth;
      gimageP->height   = inHeight;
      gimageP->depth    = 24; /* redundant since IRGB24 */
      gimageP->gamma    = 1.0;
      gimageP->title[0] = '\0';

    }
  }
  return(gimageP);
}


/*******************/
/* newRGB48Image() */
/*******************/
gImage*
newRGB48Image(
 unsigned int inWidth,
 unsigned int inHeight)
{
gImage *gimageP = NULL;

  /* allocate struct */
  gimageP = malloc(sizeof(gImage));
  if (gimageP == NULL) {
    fprintf(stderr, "xopenimage newRGB48Image malloc fail\n");
  } else {

    gimageP->data = calloc(inHeight * inWidth * 3 * 2, sizeof(unsigned char) );
    if (gimageP->data == NULL) {
      fprintf(stderr, "xopenimage newRGB48Image calloc fail\n");
      free(gimageP);
      gimageP = NULL;

    }  else {

      gimageP->gitype   = IRGB48;
      gimageP->width    = inWidth;
      gimageP->height   = inHeight;
      gimageP->depth    = 48; /* redundant since IRGB48 */
      gimageP->gamma    = 1.0;
      gimageP->title[0] = '\0';

    }
  }
  return(gimageP);
}


/*******************/
/* freeImageData() */
/*******************/
void
freeImageData(gImage *gimageP)
{
  if (gimageP->data != NULL) {
    free(gimageP->data);
    gimageP->data = NULL;
  }
}


/***************/
/* freeImage() */
/***************/
void
freeImage(gImage *gimageP)
{
  if (gimageP != NULL) {
    freeImageData(gimageP);
    free(gimageP);
  }
}


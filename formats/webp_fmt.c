/* webp_fmt.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* WebP does not do bitmaps or 16 bit */
/*  basically only RGB 8 bit */
/*  so gImage RGB24 */

/* System */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  /* strncpy */

/* libWebP  chromium.googlesource.com/webm/libwebp */
#include "webp/decode.h"
/* or if part of system */
/* #include <webp/decode.h> */

/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "webp_fmt.h"  /* enforce declarations */


/* PUBLIC FUNCTIONS */

/**************/
/* webpLoad() */
/**************/
gImage*
webpLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
int status = 0;
FILE *fP = NULL;
gImage *rgiP = NULL;
unsigned char *gP = NULL;
unsigned char *wP = NULL;
unsigned char *w_inP = NULL;
unsigned char *w_outP = NULL;
unsigned char buf12[12];
size_t nread = 0;
size_t w_size = 0;
int g_size = 0;
int w_width = 0;
int w_height = 0;
int i = 0;

  fP = fopen(inFilepath, "r");
  if (fP == NULL) {
    fprintf(stderr, "WebP error fopen %s\n", inFilepath);
    status = (-1);
  } else {
    /* check if WebP  - RIFF WEBP */
    nread = fread(buf12, 1, 12, fP);
    if (nread != 12) {
      fprintf(stderr, "WebP error fread %s\n", inFilepath);
      status = (-1);
    } else if (buf12[ 0] != 'R' || buf12[ 1] != 'I' ||
               buf12[ 2] != 'F' || buf12[ 3] != 'F' ||
               buf12[ 8] != 'W' || buf12[ 9] != 'E' ||
               buf12[10] != 'B' || buf12[11] != 'P') {
      /* not WebP, so silently return */
      status = (-1);
    } else {
      /* read file into heap */
      w_size = 8 + buf12[4] + buf12[5]*256 + buf12[6]*65536 + buf12[7]*16777216;
      w_inP = malloc(w_size);
      if (w_inP == NULL) {
        fprintf(stderr, "WebP error malloc\n");
        status = (-1);
      } else {
        fseek(fP, 0, SEEK_SET);
        nread = fread(w_inP, 1, w_size, fP);
        if (nread != w_size) {
          fprintf(stderr, "WebP error fread %s\n", inFilepath);
          status = (-1);
        }
      }
    }
    fclose(fP);
  }

  if (status == 0) {
    w_outP = WebPDecodeRGB(w_inP, w_size, &w_width, &w_height);
    if (w_outP == NULL) {
      fprintf(stderr, "WebP error DecodeRGB\n");
      status = (-1);
    } else {
      rgiP = newRGB24Image(w_width, w_height);
      if (rgiP == NULL) {
        fprintf(stderr, "WebP error newRGB24Image\n");
        status = (-1);
      } else {
        rgiP->gamma = 2.2; /* check for this ? */
        strncpy(rgiP->title, inFilepath, 255);
        rgiP->title[255]= '\0';

        if (inVerbose) {
          /* maybe try to get info whether lossy or lossless ? */
          printf("%s, WebP, size: %d x %d\n", inFilepath, w_width, w_height);
        }

        g_size = w_height * w_width * 3;
        /* maybe memcpy() here ? */
        wP = w_outP;
        gP = rgiP->data;
        for (i = 0; i < g_size; i++) {
          *gP++ = *wP++;
        }
      }
    }
  }

  if (w_inP != NULL) {
    free(w_inP);
  }
  if (w_outP != NULL) {
    WebPFree(w_outP);
  }

  return(rgiP);
}


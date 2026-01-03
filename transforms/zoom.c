/* zoom.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C System */
#include <stdlib.h>    /* malloc */
#include <stdio.h>     /* fprintf, printf */
#include <math.h>      /* rint, powl */
#include <stdint.h>

/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "zoom.h"      /* declarations */

#include "colorspace.h"
#include "downscale.h"
#include "bitdownscale.h"


/* INTERNAL */
/* structures */
struct rgb24_struct {
 unsigned char  red;
 unsigned char  green;
 unsigned char  blue;
};


struct rgb48_struct {
 unsigned char red[2];
 unsigned char green[2];
 unsigned char blue[2];
};


/* internal (static) functions */

/**************/
/* sRGB2lin() */
/**************/
/* input should be between 0 to 1.0 */
static double
sRGB2lin(
 double in)
{
double lin;
  if (in <= 0.0404482362771082L) {
    lin = in / 12.92L;
  } else {
    lin = powl( ((in + 0.055L) / 1.055L), 2.4L);
  }
  return(lin);
}


/*************/
/* makemap() */
/*************/
/* note carefully: makemap malloc's on heap */
/*  caller's responsibility to free() */
static unsigned int*
makemap(
 unsigned int inPercent,
 unsigned int inLength,
 unsigned int *ioMapLen)
{
double ratio = 1.0;
unsigned int len;
unsigned int *mapP = NULL;
unsigned int i;

  if (inPercent == 0) {
    len = inLength;
    ratio = 1.0;
  } else {
    len = (inLength * inPercent) * 0.01;
    ratio = 100.0 / inPercent;
  }

  mapP = malloc(sizeof(unsigned int) * len);
  if (mapP == NULL) {
    fprintf(stderr, "zoom makemap malloc error\n");
    *ioMapLen = 0;
  } else {
    for (i = 0; i < len; i++) {
      *(mapP + i) = i * ratio;
    }
    *ioMapLen = len;
  }

  return(mapP);
}


/*************/
/* zoombit() */
/*************/
static gImage*
zoombit(
 gImage *ingimageP,
 unsigned int inXzoom,
 unsigned int inYzoom)
{
gImage *rgiP = NULL;
unsigned int *xmap = NULL;
unsigned int *ymap = NULL;
unsigned char *srclineP = NULL;
unsigned char *dstlineP = NULL;
unsigned char *srcP = NULL;
unsigned char *dstP = NULL;
unsigned int srclinelen;
unsigned int dstlinelen;
unsigned int xlen;
unsigned int ylen;
unsigned int x;
unsigned int xsrc;
unsigned int y;
unsigned int ysrc;
unsigned char srcmask;
unsigned char dstmask;
unsigned char bit;
int status = 0;

  if (inXzoom == 0 && inYzoom == 0) {
    rgiP = NULL;

  } else if (inXzoom < 100 && inYzoom < 100) {

    xlen = (inXzoom == 0 ? ingimageP->width  : (ingimageP->width  * inXzoom) * 0.01);
    ylen = (inYzoom == 0 ? ingimageP->height : (ingimageP->height * inYzoom) * 0.01);
    rgiP = newBitImage(xlen, ylen);

    status = bitdownscale(ingimageP->data, ingimageP->width, ingimageP->height,
      rgiP->data, xlen, ylen);
    if (status != 0) {
      fprintf(stderr, "zoombit bitscaledown error\n");
    }

  } else {

    xmap = makemap(inXzoom, ingimageP->width, &xlen);
    ymap = makemap(inXzoom, ingimageP->height, &ylen);
    rgiP = newBitImage(xlen, ylen);

    srclinelen = (ingimageP->width + 7) / 8;
    dstlinelen = (xlen + 7) / 8;

    srclineP = ingimageP->data;
    dstlineP = rgiP->data;

    for (y = 0, ysrc = *(ymap + y); y < ylen; y++) {
      while (ysrc != *(ymap + y)) {
        ysrc++;
        srclineP += srclinelen;
      }
      srcP = srclineP;
      dstP = dstlineP;
      srcmask = 0x01;
      dstmask = 0x01;
      bit = srcmask & (*srcP);
      for (x = 0, xsrc = *(xmap + x); x < xlen; x++) {
        if (xsrc != *(xmap + x) ) {
          do {
            xsrc++;
            if (srcmask == 0x80) {
              srcP++;
              srcmask = 0x01;
            } else {
              srcmask <<= 0x01;
            }
          } while (xsrc != *(xmap + x));
          bit = srcmask & *srcP;
        }
        if (bit != 0) {
          *dstP |= dstmask;
        }
        if (dstmask == 0x80) {
          dstP++;
          dstmask = 0x01;
        } else {
          dstmask <<= 0x01;
        }
      }
      dstlineP += dstlinelen;
    }

    free(xmap);
    free(ymap);
  }

  return(rgiP);
}


/************/
/* zoom24() */
/************/
gImage*
zoom24(
 gImage *ingimageP,
 unsigned int inXzoom,
 unsigned int inYzoom)
{
gImage *rgiP = NULL;
unsigned int *xmap = NULL;
unsigned int *ymap = NULL;
unsigned char *srclineP = NULL;
unsigned char *srcP = NULL;
unsigned char *dstP = NULL;
unsigned char *byteP = NULL;
float *inrgbP = NULL;
float *downrgbP = NULL;
float *floatP = NULL;
int status = 0;
unsigned int i;
unsigned int len;
unsigned int xlen;
unsigned int ylen;
unsigned int srclinelen;
unsigned int x;
unsigned int y;
unsigned int xsrc;
unsigned int ysrc;
double tempd;
struct rgb24_struct rgb24;

  if (inXzoom == 0 && inYzoom == 0) {
    rgiP = NULL;

  } else if (inXzoom < 100 && inYzoom < 100) {
    /* downscale */

    xlen = (inXzoom == 0 ? ingimageP->width  : (ingimageP->width  * inXzoom) * 0.01);
    ylen = (inYzoom == 0 ? ingimageP->height : (ingimageP->height * inYzoom) * 0.01);

    inrgbP = malloc(sizeof(float) * 3 * ingimageP->width * ingimageP->height);
    downrgbP = malloc(sizeof(float) * 3 * xlen * ylen);
    if (inrgbP == NULL || downrgbP == NULL) {
      fprintf(stderr, "zoom24: malloc error\n");
      if (inrgbP != NULL) free(inrgbP);
      if (downrgbP != NULL) free(downrgbP);
      rgiP = NULL;
    } else {

      /* convert original to linearized float */
      byteP = ingimageP->data;
      floatP = inrgbP;
      len = ingimageP->width * ingimageP->height * 3;
      for (i = 0; i < len; i++) {
        /* gImage is in encoded (gamma) sRGB, so need to linearize */
        /* sRGB linearization is same for red, green, and blue */
        /* gImage.data is integer=byte, so can use as index to sRGBlin array */
        *floatP++ = sRGBlin[*byteP++];
        /* sRGBlin double -to-> float */
      }

      /* downscale, in float */
      status = downscale(3, inrgbP, ingimageP->width, ingimageP->height,
        downrgbP, xlen, ylen);
      free(inrgbP);
      if (status != 0) {
        fprintf(stderr, "zoom24: downscale error\n");
        rgiP = NULL;

      } else {
        /* output: return linear to sRGB encoded (gamma), 8bit per RGB */
        rgiP = newRGB24Image(xlen, ylen);
        byteP = rgiP->data;
        floatP = downrgbP;
        len = xlen * ylen * 3;
        for (i = 0; i < len; i++) {
          tempd = *floatP++; /* downrgbP float -to-> tempd double */
          *byteP++ = rint(255.0 * lin2sRGB(tempd));
        }
      }
      free(downrgbP);
    }

  } else {
    /* at least one (x,y) expansion */

    srclinelen = (ingimageP->width * 3);

    xmap = makemap(inXzoom, ingimageP->width, &xlen);
    ymap = makemap(inYzoom, ingimageP->height, &ylen);

    rgiP = newRGB24Image(xlen, ylen);

    srclineP = ingimageP->data; /* initialized, but will be changed */
    dstP = rgiP->data; /* initialized, but will be changed */
    for (y = 0, ysrc = *(ymap + y); y < ylen; y++) {

      while (ysrc != *(ymap + y)) {
        ysrc++;
        srclineP += srclinelen;
      }

      srcP = srclineP;
      rgb24 = *((struct rgb24_struct *) srcP);
      for (x = 0, xsrc = *(xmap + x); x < xlen; x++) {

        if (xsrc != *(xmap + x)) {
          do {
            xsrc++;
            srcP += 3;
          } while (xsrc != *(xmap + x));
          rgb24 = *((struct rgb24_struct *) srcP);
        }
        *(dstP)   = rgb24.red;
        *(dstP+1) = rgb24.green;
        *(dstP+2) = rgb24.blue;
        dstP += 3;
      }

    }

    free(xmap);
    free(ymap);
  }

  return(rgiP);
}


/************/
/* zoom48() */
/************/
gImage*
zoom48(
 gImage *ingimageP,
 unsigned int inXzoom,
 unsigned int inYzoom)
{
gImage *rgiP = NULL;
unsigned int *xmap = NULL;
unsigned int *ymap = NULL;
unsigned int xlen;
unsigned int ylen;
unsigned int i;
unsigned int len;
uint16_t *u16P = NULL;
int status = 0;
float *inrgbP = NULL;
float *downrgbP = NULL;
float *floatP = NULL;
double tempd;
unsigned int srclinelen;
unsigned int ysrc;
unsigned int xsrc;
unsigned char *srclineP = NULL;
unsigned char *srcP = NULL;
unsigned int y;
unsigned int x;
unsigned char *dstP = NULL;
struct rgb48_struct rgb48;

  if (inXzoom == 0 && inYzoom == 0) {
    rgiP = NULL;

  } else if (inXzoom < 100 && inYzoom < 100) {
    /* downscale */

    xlen = (inXzoom == 0 ? ingimageP->width  : (ingimageP->width  * inXzoom) * 0.01);
    ylen = (inYzoom == 0 ? ingimageP->height : (ingimageP->height * inYzoom) * 0.01);

    inrgbP = malloc(sizeof(float) * 3 * ingimageP->width * ingimageP->height);
    downrgbP = malloc(sizeof(float) * 3 * xlen * ylen);
    if (inrgbP == NULL || downrgbP == NULL) {
      fprintf(stderr, "zoom48: malloc error\n");
      if (inrgbP != NULL) free(inrgbP);
      if (downrgbP != NULL) free(downrgbP);
      rgiP = NULL;
    } else {

      /* convert original uint16_t sRGB gamma coded to linearized float */
      u16P = (uint16_t*) ingimageP->data;
      floatP = inrgbP;
      len = ingimageP->width * ingimageP->height * 3;
      for (i = 0; i < len; i++) {
        /* gImage is in encoded (gamma) sRGB, so need to linearize */
        /* sRGB linearization is same for red, green, and blue */
        *floatP++ = sRGB2lin( (*u16P++) / 65535.0 );
      }

      /* downscale, linear, float -to-> float */
      status = downscale(3, inrgbP, ingimageP->width, ingimageP->height,
        downrgbP, xlen, ylen);
      free(inrgbP);
      if (status != 0) {
        fprintf(stderr, "zoom48: downscale error\n");
        rgiP = NULL;

      } else {
        /* output: return linear to sRGB encoded (gamma), 8bit per RGB */
        rgiP = newRGB48Image(xlen, ylen);
        u16P = (uint16_t*) rgiP->data;
        floatP = downrgbP;
        len = xlen * ylen * 3;
        for (i = 0; i < len; i++) {
          tempd = *floatP++; /* downrgbP float -to-> tempd double */
          *u16P++ = rint(65535.0 * lin2sRGB(tempd));
        }
      }
      free(downrgbP);
    }

  } else {
    /* at least one (x,y) expansion */

    srclinelen = (ingimageP->width * 3 * 2);

    xmap = makemap(inXzoom, ingimageP->width, &xlen);
    ymap = makemap(inYzoom, ingimageP->height, &ylen);

    rgiP = newRGB48Image(xlen, ylen);

    srclineP = ingimageP->data;
    dstP = rgiP->data;

    for (y = 0, ysrc = *(ymap + y); y < ylen; y++) {

      while (ysrc != *(ymap + y)) {
        ysrc++;
        srclineP += srclinelen;
      }

      srcP = srclineP;
      rgb48 = *((struct rgb48_struct *) srcP);
      for (x = 0, xsrc = *(xmap + x); x < xlen; x++) {

        if (xsrc != *(xmap + x)) {
          do {
            xsrc++;
            srcP += 6;
          } while (xsrc != *(xmap + x));

          rgb48 = *((struct rgb48_struct *) srcP);
        }
        *(dstP)     = rgb48.red[0];
        *(dstP + 1) = rgb48.red[1];
        *(dstP + 2) = rgb48.green[0];
        *(dstP + 3) = rgb48.green[1];
        *(dstP + 4) = rgb48.blue[0];
        *(dstP + 5) = rgb48.blue[1];
        dstP += 6;
      }
    } 

    free(xmap);
    free(ymap);
  }

  return(rgiP);
}


/* PUBLIC FUNCTIONS */

/**********/
/* zoom() */
/**********/
gImage*
zoom(
 gImage *ingimageP,
 unsigned int inXzoom,
 unsigned int inYzoom,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;

  if (inXzoom == 0 && inYzoom == 0) {
    return(ingimageP);
  }

  if (inVerbose != 0) {
    if (inXzoom == 0) {
      printf(" Zoom Y by %d%%\n", inYzoom);
    } else if (inYzoom == 0) {
      printf(" Zoom X by %d%%\n", inXzoom);
    } else if (inXzoom == inYzoom) {
      printf(" Zoom by %d%%\n", inXzoom);
    } else {
      printf(" Zoom X by %d%% and Y by %d%%\n", inXzoom, inYzoom);
    }
  }

  switch(ingimageP->gitype) {
   case IBITMAP:
    rgiP = zoombit(ingimageP, inXzoom, inYzoom);
    break;
   case IRGB24:
    rgiP = zoom24(ingimageP, inXzoom, inYzoom);
    break;
   case IRGB48:
    rgiP = zoom48(ingimageP, inXzoom, inYzoom);
    break;
   default:
    fprintf(stderr, "zoom error invalid image type\n");
  }

  return(rgiP);
}


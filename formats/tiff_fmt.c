/* tiff_fmt.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C System */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  /* strncpy */

/* TIFF */
#include "tiff.h"
#include "tiffio.h"  /* TIFFOpen, TIFFGetField, TIFFRead */


/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "tiff_fmt.h"  /* enforce declarations */


/* internal static variables */

/* bitwise reverse lookup table */
static unsigned char reversed[256] = {
0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF};


/* internal static functions */

/***************/
/* tiffCheck() */
/***************/
/* check first 4 bytes of file */
/* return -1 (true) if prelim valid TIFF */
/* return 0 (false) if invalid TIFF */
static int
tiffCheck(
 const char *inFilepath)
{
int valid = 0; /* default to false */
FILE *fP = NULL;
unsigned char buf[4];
size_t nread;

  fP = fopen(inFilepath, "rb");
  if (fP == NULL) {
    fprintf(stderr, "error fopen %s\n", inFilepath);
  } else {
    nread = fread(buf, 1, 4, fP);
    fclose(fP);
    if (nread == 4) {
      if (buf[0] == 0x49) {
        if (buf[1] == 0x49 && buf[2] == 0x2A && buf[3] == 0) {
          valid = (-1);
        }
      } else if (buf[0] == 0x4D) {
        if (buf[1] == 0x4D && buf[2] == 0 && buf[3] == 0x2A) {
          valid = (-1);
        }
      }
    }
  }
  return(valid);
}

/****************/
/* tiffBitmap() */
/****************/
/* note!! */
/*  X11 Xbitmap convention is that left pixel is least sig bit */
/*   so that is what the generic image format assumes */
/*  NetPBM is left pixel is most sig bit, so needs conversion */
/*  TIFF "FillOrder" default is "1" is left pixel is most sig bit */
/*  so TIFF bilevel (like NetPBM bitmap) needs conversion */
static gImage*
tiffBitmap(
 TIFF *inTiffP,
 const char *inFilepath,
 unsigned int inVerbose)
{
int tstatus = 0;
int tiff_w;
int tiff_h;
unsigned short tiff_photometric;
unsigned short scanlinesize;
unsigned char *scanlineP = NULL;
int iy;
int i;
gImage *rgiP = NULL;
unsigned char *gP = NULL;

  if (inTiffP != NULL) {
    TIFFGetField(inTiffP, TIFFTAG_IMAGEWIDTH, &tiff_w);
    TIFFGetField(inTiffP, TIFFTAG_IMAGELENGTH, &tiff_h);
    TIFFGetField(inTiffP, TIFFTAG_PHOTOMETRIC, &tiff_photometric);
    if (tiff_w <= 0 || tiff_h <= 0) {
      fprintf(stderr, "TIFF: width and height must be > 0\n");
    } else {
      scanlinesize = TIFFScanlineSize(inTiffP);
      scanlineP = malloc(scanlinesize);
      if (scanlineP == NULL) {
        fprintf(stderr, "TIFF malloc error\n");
      } else {
        rgiP = newBitImage(tiff_w, tiff_h);
        if (rgiP == NULL) {
          fprintf(stderr, "TIFF newBitmapImage fail\n");
        } else {
          if (inVerbose) {
            printf("%s, TIFF bitmap, size: %d x %d\n", inFilepath, tiff_w, tiff_h);
          }
          gP = rgiP->data;
          for (iy = 0; iy < tiff_h; iy++) {
            tstatus = TIFFReadScanline(inTiffP, scanlineP, iy, 0);
            if (tstatus < 0) {
              fprintf(stderr, "TIFF premature end of data\n");
              break;
            }
            if (tiff_photometric == PHOTOMETRIC_MINISWHITE) {
              for (i = 0; i < scanlinesize; i++) {
                *gP++ = reversed[scanlineP[i]];
              }
            } else {
              for (i = 0; i < scanlinesize; i++) {
                *gP++ = ~reversed[scanlineP[i]];
              }
            }
          }
        }
        free(scanlineP);
      }
    }
  }

  return(rgiP);
}


/***************/
/* tiffRGB24() */
/***************/
static gImage*
tiffRGB24(
 TIFF *inTiffP,
 const char *inFilepath,
 unsigned int inVerbose)
{
int tstatus = 0;
int tiff_w;
int tiff_h;
int ix;
int iy;
gImage *rgiP = NULL;
uint32_t *tiff_RGBA = NULL;
unsigned char *gP = NULL;
unsigned char *tP = NULL;

  if (inTiffP != NULL) {
    TIFFGetField(inTiffP, TIFFTAG_IMAGEWIDTH, &tiff_w);
    TIFFGetField(inTiffP, TIFFTAG_IMAGELENGTH, &tiff_h);
    if (tiff_w <= 0 || tiff_h <= 0) {
      fprintf(stderr, "TIFF: width and height must be > 0\n");
    } else {
      /* tiff_RGBA = _TIFFCheckMalloc(inTiffP, tiff_w * tiff_h, sizeof(uint32_t), "RGBA buffer"); */
      tiff_RGBA = _TIFFmalloc(tiff_w * tiff_h * sizeof(uint32_t));
      if (tiff_RGBA == NULL) {
        /* fprintf(stderr, "_TIFFCheckMalloc error\n"); */
        fprintf(stderr, "_TIFFmalloc error\n");
      } else {
        tstatus = TIFFReadRGBAImageOriented(inTiffP, tiff_w, tiff_h,
          tiff_RGBA, ORIENTATION_TOPLEFT, 0);
        if (tstatus == 0) {
          fprintf(stderr, "TIFFReadRGBAImageOriented error\n");
        } else {
          if (inVerbose) {
            printf("%s, TIFF RGB 24bit, size: %d x %d\n", inFilepath, tiff_w, tiff_h);
          }
          rgiP = newRGB24Image(tiff_w, tiff_h);
          if (rgiP == NULL) {
            fprintf(stderr, "newRGB24Image error\n");
          } else {
            rgiP->gamma = 2.2;
            /* convert the TIFF data to gImage data */
            gP = rgiP->data;
            tP = (unsigned char *)tiff_RGBA;
            for (iy = 0; iy < tiff_h; iy++) {
              for (ix = 0; ix < tiff_w; ix++) {
                *gP++ = *tP++;  /* red */
                *gP++ = *tP++;  /* green */
                *gP++ = *tP++;  /* blue */
                tP++;           /* alpha */
              }
            }
          }
        }
        _TIFFfree(tiff_RGBA);
      }
    }
  }
  return(rgiP);
}


/***************/
/* tiffRGB48() */
/***************/
/* note!! */
/* 16 bits per sample (RGB 3 colors then 48 bits) */
/* requires multiple bytes for sample, so endianness matters */
/* TIFF format foundationally allows both big and little endian */
/*  first 2 bytes are "II" for little = Intel or "MM" for big = Motorola */
/* libTIFF says it converts to architecture's native format */
/*  (libTIFF says it does this for bitmaps, but does not) */
/* here, assume libTIFF does what it says, and 16bit is in native format */
static gImage*
tiffRGB48(
 TIFF *inTiffP,
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;
int tstatus;
int tiff_w;
int tiff_h;
int i;
int iy;
unsigned short tiff_photometric;
unsigned short scanlinesize;
unsigned char *scanlineP = NULL;
uint16_t *tP = NULL;
uint16_t *gP = NULL;
uint16_t u16;

  if (inTiffP != NULL) {
    TIFFGetField(inTiffP, TIFFTAG_IMAGEWIDTH, &tiff_w);
    TIFFGetField(inTiffP, TIFFTAG_IMAGELENGTH, &tiff_h);
    TIFFGetField(inTiffP, TIFFTAG_PHOTOMETRIC, &tiff_photometric);
    if (tiff_w <= 0 || tiff_h <= 0) {
      fprintf(stderr, "TIFF: width and height must be > 0\n");
    } else {
      scanlinesize = TIFFScanlineSize(inTiffP);
      scanlineP = malloc(scanlinesize);
      if (scanlineP == NULL) {
        fprintf(stderr, "TIFF malloc error\n");
      } else {
        rgiP = newRGB48Image(tiff_w, tiff_h);
        if (rgiP == NULL) {
          fprintf(stderr, "TIFF newRGB48Image fail\n");
        } else {
          rgiP->gamma = 2.2; /* presume TIFF is sRGB, could check? */
          if (tiff_photometric == PHOTOMETRIC_RGB) {
            if (inVerbose) {
              printf("%s, TIFF RGB 48bit, size: %d x %d\n", inFilepath, tiff_w, tiff_h);
            }
            gP = (uint16_t *)rgiP->data;
            for (iy = 0; iy < tiff_h; iy++) {
              tstatus = TIFFReadScanline(inTiffP, scanlineP, iy, 0);
              if (tstatus == 0) { fprintf(stderr, "TIFF library ReadScanline %d\n", tstatus); }
              tP = (uint16_t *)scanlineP;
              for (i = 0; i < scanlinesize; i+=2) {
                *gP++ = *tP++;
              }
            }
          } else if (tiff_photometric == PHOTOMETRIC_MINISBLACK ||
                     tiff_photometric == PHOTOMETRIC_MINISWHITE) {
            if (inVerbose) {
              printf("%s, TIFF grayscale 16bit, size: %d x %d\n", inFilepath, tiff_w, tiff_h);
            }
            gP = (uint16_t *)rgiP->data;
            for (iy = 0; iy < tiff_h; iy++) {
              tstatus = TIFFReadScanline(inTiffP, scanlineP, iy, 0);
              if (tstatus == 0) { fprintf(stderr, "TIFF library ReadScanline %d\n", tstatus); }
              tP = (uint16_t *)scanlineP;
              switch(tiff_photometric) {
               case PHOTOMETRIC_MINISBLACK:
                for (i = 0; i < scanlinesize; i+=2) {
                  u16 = *tP++;
                  *gP++ = u16; *gP++ = u16; *gP++ = u16;
                }
                break;
               case PHOTOMETRIC_MINISWHITE:
                for (i = 0; i < scanlinesize; i+=2) {
                  u16 = 65535 - (*tP++);
                  *gP++ = u16; *gP++ = u16; *gP++ = u16;
                }
                break;
              }
            }
          }
        }
        free(scanlineP);
      }
    }
  }

  return(rgiP);
}


/* PUBLIC FUNCTIONS */


/**************/
/* tiffLoad() */
/**************/
gImage*
tiffLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;
TIFF *tiffP = NULL;
unsigned short tiff_bitspersample;

  if (tiffCheck(inFilepath) != 0) {

    tiffP = TIFFOpen(inFilepath, "r");
    if (tiffP != NULL) {

      TIFFGetField(tiffP, TIFFTAG_BITSPERSAMPLE, &tiff_bitspersample);
      if (tiff_bitspersample == 0) {
        fprintf(stderr, "TIFF error BITSPERSAMPLE of zero\n");
      } else if (tiff_bitspersample == 1) {
        rgiP = tiffBitmap(tiffP, inFilepath, inVerbose);
      } else if (tiff_bitspersample <= 8) {
        rgiP = tiffRGB24(tiffP, inFilepath, inVerbose);
      } else if (tiff_bitspersample <= 16) {
        rgiP = tiffRGB48(tiffP, inFilepath, inVerbose);
      } else {
        fprintf(stderr, "TIFF invalid bits per sample\n");
      }

      TIFFClose(tiffP);
    }
  }

  if (rgiP != NULL) {
    strncpy(rgiP->title, inFilepath, 255);
    rgiP->title[255] = '\0';
  }

  return(rgiP);
}


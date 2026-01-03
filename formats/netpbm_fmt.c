/* netpbm_fmt.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h>
#include <stdio.h>  /* printf, fprintf, fopen, fclose, fread, fgetc */
#include <string.h> /* strncpy */

/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "netpbm_fmt.h" /* declarations */

/* #define s */

/* careful: need to make sure that b is not zero */
#define PM_SCALE(a, b, c) (long)((a) * (c)) / (b)

/* these are all negative */
#define NOTINT (-1)
#define COMMENT (-2)
#define SPACE (-3)
#define NEWLINE (-4)

/* enum ? */
#define BADREAD (0)    /* read error */
#define NOTPBM (1)     /* not a pbm file */
#define PBMNORMAL (2)  /* pbm normal type file */
#define PBMRAWBITS (3) /* pbm raw bits type file */
#define PGMNORMAL (4)  /* pgm normal type file */
#define PGMRAWBITS (5) /* pgm raw bits type file */
#define PPMNORMAL (6)  /* ppm normal type file */
#define PPMRAWBITS (7) /* ppm raw bits type file */

/* Internal (static) memory allocations */
static int Initialized = 0;
static int IntTable[256];

/* Internal (static) non-public functions */

/*********************/
/* initializeTable() */
/*********************/
static void
initializeTable(void)
{
int i = 0;

  for (i = 0; i < 256; i++) {
    IntTable[i] = NOTINT;
  }

  IntTable['#']  = COMMENT;
  IntTable[' ']  = SPACE;
  IntTable['\t'] = SPACE;
  IntTable['\r'] = SPACE; /* ? NEWLINE if MacOS ? */
  IntTable['\n'] = NEWLINE;

  IntTable['0'] = 0;
  IntTable['1'] = 1;
  IntTable['2'] = 2;
  IntTable['3'] = 3;
  IntTable['4'] = 4;
  IntTable['5'] = 5;
  IntTable['6'] = 6;
  IntTable['7'] = 7;
  IntTable['8'] = 8;
  IntTable['9'] = 9;

  Initialized = (-1);
}

/*****************/
/* pbmReadChar() */
/*****************/
/* gets next char that is not a comment */
/* returns char c, or -1 if end-of-file */
static int
pbmReadChar(
 FILE *inFileP)
{
int c = 0;

  c = fgetc(inFileP);

  if (c == EOF) {
    fclose(inFileP);
    c = -1;
  } else if (IntTable[c] == COMMENT) {
    do {
      c = fgetc(inFileP);
      if (c == EOF) {
        /* ?? fclose(inFileP) ?? */
        c = -1;
        break;
      }
    } while (IntTable[c] != NEWLINE);
  }

  return (c);
}

/****************/
/* pbmReadInt() */
/****************/
/* return int or -1 if end-of-file */
static int
pbmReadInt(
 FILE *inFileP)
{
int c = 0;
int value = 0;

  for (;;) {
    c = pbmReadChar(inFileP);
    if (c < 0) {
      return (-1);
    }
    if (IntTable[c] >= 0) {
      break;
    }
  };

  value = IntTable[c];
  for (;;) {
    c = pbmReadChar(inFileP);
    if (c < 0) {
      return (-1);
    }
    if (IntTable[c] < 0) {
      return (value);
    }
    value = (value * 10) + IntTable[c];
  }
}

/***********/
/* isPBM() */
/***********/
static int
isPBM(
 FILE *inFileP,
 const char *inFilepath,
 unsigned int *ioWidth,
 unsigned int *ioHeight,
 unsigned int *ioMaxval,
 unsigned int inVerbose)
{
unsigned char buf[4] = {'\x00', '\x00', '\x00', '\x00'};
int w;
int h;
int max;

  if (Initialized == 0) {
    initializeTable();
  }

  if (fread(buf, 1, 2, inFileP) != 2) {
    return(NOTPBM);
  }

  if (buf[0] != 'P') {
    return(NOTPBM);
  }

  /* P1 - bitmap, ASCII */
  if (buf[1] == '1') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = 1;
    if (inVerbose) {
      printf("%s, PBM ASCII, size: %d x %d \n", inFilepath, w, h);
    }
    return(PBMNORMAL);
  }
  /* P4 - bitmap, binary */
  if (buf[1] == '4') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = 1;
    if (inVerbose) {
      printf("%s, PBM raw, size: %d x %d\n", inFilepath, w, h);
    }
    return(PBMRAWBITS);
  }
  /* P2 - grayscale, ASCII */
  if (buf[1] == '2') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    max = pbmReadInt(inFileP);
    if (max <= 0) {
      fprintf(stderr, " NetPBM maxval must be > 0\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = max;
    if (inVerbose) {
      printf("%s, PGM ASCII, %d levels, size: %d x %d\n", inFilepath,
        max+1, w, h);
    }
    return(PGMNORMAL);
  }
  /* P5 - grayscale, binary */
  if (buf[1] == '5') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    max = pbmReadInt(inFileP);
    if (max != 255 && max != 65535) {
      fprintf(stderr, " NetPBM P5 maxval must be 255 or 65535\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = max;
    if (inVerbose) {
      printf("%s, PGM raw, %d levels, size: %d x %d\n", inFilepath,
        max+1, w, h);
    }
    return(PGMRAWBITS);
  }
  /* P3 - color, ASCII */
  if (buf[1] == '3') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    max = pbmReadInt(inFileP);
    if (max <= 0) {
      fprintf(stderr, " NetPBM maxval must be > 0\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = max;
    if (inVerbose) {
      printf("%s, PPM ASCII, %d levels, size: %d x %d\n", inFilepath,
        max+1, w, h);
    }
    return(PPMNORMAL);
  }
  /* P6 - color, binary */
  if (buf[1] == '6') {
    w = pbmReadInt(inFileP);
    h = pbmReadInt(inFileP);
    if ( (w <= 0) || (h <= 0) ) {
      fprintf(stderr, " NetPBM width and height must be > 0\n");
      return(NOTPBM);
    }
    max = pbmReadInt(inFileP);
    if (max != 255 && max != 65535) {
      fprintf(stderr, " NetPBM P6 maxval must be 255 or 65535\n");
      return(NOTPBM);
    }
    *ioWidth = w;
    *ioHeight = h;
    *ioMaxval = max;
    if (inVerbose) {
      printf("%s, PPM raw, %d levels, size: %d x %d\n", inFilepath,
        max+1, w, h);
    }
    return(PPMRAWBITS);
  }

  return(NOTPBM);
}


/* PUBLIC FUNCTIONS */


/*************/
/* pbmLoad() */
/*************/
gImage*
pbmLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
FILE          *fileP = NULL;
gImage        *gimageP = NULL;
unsigned char *dstlineP = NULL;
unsigned char *dstP = NULL;
unsigned char dstmask = 0;
unsigned char srcmask = 0;
int src;
int blo;
int bhi;
int pbm_type;
int red, grn, blu;
unsigned int width;
unsigned int height;
unsigned int maxval;
unsigned int linelen;
unsigned int x;
unsigned int y;
size_t size;

  fileP = fopen(inFilepath, "r");

  if (fileP == NULL) {
    gimageP = NULL;

  } else if ((pbm_type = isPBM(fileP, inFilepath, &width, &height, &maxval, inVerbose)) ==
             NOTPBM) {
    fclose(fileP);
    gimageP = NULL;
  } else {

    switch (pbm_type) {

     case PBMNORMAL:
      /* P1, bitmap, ASCII */
      gimageP = newBitImage(width, height);
      linelen = (width + 7) / 8;
      /* assumes gimageP->data has been zero'd, calloc() */
      dstlineP = gimageP->data;
      for (y = 0; y < height; y++) {
        dstP = dstlineP;
        dstmask = 0x01; /* LSB for X11 destination */
        for (x = 0; x < width; x++) {
          do {
            src = pbmReadChar(fileP);
            if (src < 0) {
              fprintf(stderr, "%s: Short image\n", inFilepath);
              fclose(fileP);
              return (gimageP);
            }
            if (IntTable[src] == NOTINT) {
              fprintf(stderr, "%s: Bad image data\n", inFilepath);
              fclose(fileP);
              return (gimageP);
            }
          } while (IntTable[src] < 0);

          switch (IntTable[src]) {
          case 1:
            /* 1 -> modify dstP */
            *dstP |= dstmask;
            /* fall-thru */
          case 0:
            /* 0 -> don't change dstP */
            /* either 0 or 1, prepare for next dstmask */
            if (dstmask == 0x80) {
              dstmask = 0x01;
              dstP++;
            } else {
              dstmask <<= 1;
            }
            break;
          default:
            fprintf(stderr, "%s: Bad image data\n", inFilepath);
            fclose(fileP);
            return (gimageP);
          }
        } /* end for x up to width */
        dstlineP += linelen;
      } /* end for y up to height */
      break;

     case PBMRAWBITS:
      /* P4, bitmap, binary */
      /* NetPBM convention left-to-right starts at most sig bit */
      /*  need to convert to LSB first */
      /*  which is gImage and X11 standard */
      /* src will go from 0x80 down to 0x01 */
      /* dst will go from 0x01 up to 0x80 */
      gimageP = newBitImage(width, height);
      dstlineP = gimageP->data;
      linelen = (width + 7) / 8;
      srcmask = 0;
      for (y = 0; y < height; y++) {
        /* offset by one since we always increment the destination byte */
        dstP = dstlineP - 1;
        /* dstP possibly not allocated memory */
        for (x = 0; x < width; x++) {
          if (x % 8 == 0) { /* byte boundary */
            dstmask = 0x01;
            srcmask = 0x80;
            dstP += 1;
            src = fgetc(fileP);
            if (src == EOF) {
              fprintf(stderr, "%s: Short image\n", inFilepath);
              fclose(fileP);
              return (gimageP);
            }
          } else {
            dstmask <<= 1;
            srcmask >>= 1;
          }
          if (src & srcmask) {
            *dstP |= dstmask;
          }
        } /* end for x up to width */
        dstlineP += linelen;
      } /* end for y up to height */
      break;

     case PGMNORMAL:
      /* P2, grayscale, ASCII */
      if (maxval == 0) {
        fprintf(stderr, "NetPBM, maxval 0, trying to divide by zero\n");
        fclose(fileP);
        return (NULL);
      }
      gimageP = newRGB24Image(width, height);
      gimageP->gamma = 2.2;
      dstP = gimageP->data;
      size = height * width;
      for (y = 0; y < size; y++) {
        src = pbmReadInt(fileP);
        if (src < 0) {
          fprintf(stderr, "%s: Short image\n", inFilepath);
          fclose(fileP);
          return (gimageP);
        }
        /* maxval could be > 255 */
        /* this scales src down to [0,255] for RGB24 */
        src = PM_SCALE(src, maxval, 0xff);
        *(dstP++) = src; /* red */
        *(dstP++) = src; /* green */
        *(dstP++) = src; /* blue */
      }
      break;

     case PGMRAWBITS:
      /* P5, grayscale, binary */
      /*  maxval 255 or 65535 only */
      if (maxval == 0) {
        fprintf(stderr, "NetPBM, maxval 0, trying to divide by zero\n");
        fclose(fileP);
        return (NULL);
      } else if (maxval == 255) {
        gimageP = newRGB24Image(width, height);
        gimageP->gamma = 2.2;
        dstP = gimageP->data;
        size = height * width;
        for (y = 0; y < size; y++) {
          src = fgetc(fileP);
          if (src == EOF) {
            fprintf(stderr, "%s: Short image\n", inFilepath);
            fclose(fileP);
            freeImage(gimageP);
            return (NULL);
          }
          *(dstP++) = src; /* red */
          *(dstP++) = src; /* green */
          *(dstP++) = src; /* blue */
        }
      } else if (maxval == 65535) {
        gimageP = newRGB48Image(width, height);
        dstP = gimageP->data;
        size = height * width;
        for (y = 0; y < size; y++) {
          bhi = fgetc(fileP);
          blo = fgetc(fileP);
          if (blo == EOF) {
            fprintf(stderr, "%s: Short image\n", inFilepath);
            fclose(fileP);
            freeImage(gimageP);
            return(NULL);
          }
          /* NetPBM is MSB, assuming here X11 and native are LSB */
          *dstP++ = blo; *dstP++ = bhi; /* red */
          *dstP++ = blo; *dstP++ = bhi; /* green */
          *dstP++ = blo; *dstP++ = bhi; /* blue */
        }
      } else {
        fprintf(stderr, "NetPBM grayscale binary, maxval must be 255 or 65535\n");
        fclose(fileP);
        return(NULL);
      }
      break;

     case PPMNORMAL:
      /* P3, color RGB, ASCII */
      if (maxval == 0) {
        fprintf(stderr, "NetPBM, maxval 0, trying to divide by zero\n");
        fclose(fileP);
        return (NULL);
      }
      gimageP = newRGB24Image(width, height);
      gimageP->gamma = 2.2;
      dstP = gimageP->data;
      size = height * width;
      for (y = 0; y < size; y++) {
        if (((red = pbmReadInt(fileP)) == EOF) ||
            ((grn = pbmReadInt(fileP)) == EOF) ||
            ((blu = pbmReadInt(fileP)) == EOF)) {
          fprintf(stderr, "%s: Short image\n", inFilepath);
          fclose(fileP);
          return (gimageP);
        }
        /* this scales rgb down to [0,255] for RGB24 */
        /* could change so if maxval > 255, use RGB48 */
        *(dstP++) = PM_SCALE(red, maxval, 0xff);
        *(dstP++) = PM_SCALE(grn, maxval, 0xff);
        *(dstP++) = PM_SCALE(blu, maxval, 0xff);
      }
      break;

     case PPMRAWBITS:
      /* P6, color RGB, binary */
      if (maxval == 0) {
        fprintf(stderr, "NetPBM, maxval 0, trying to divide by zero\n");
        fclose(fileP);
        return (NULL);
      } else if (maxval == 255) {
        gimageP = newRGB24Image(width, height);
        gimageP->gamma = 2.2;
        size = height * width * 3;
        if (fread(gimageP->data, 1, size, fileP) != size) {
          fprintf(stderr, "%s: Short image\n", inFilepath);
          fclose(fileP);
          freeImage(gimageP);
          return (NULL);
        }
      } else if (maxval == 65535) {
        gimageP = newRGB48Image(width, height);
        gimageP->gamma = 2.2;
        size = height * width * 3 * 2;
        if (fread(gimageP->data, 1, size, fileP) != size) {
          fprintf(stderr, "%s: Short image\n", inFilepath);
          fclose(fileP);
          freeImage(gimageP);
          return(NULL);
        }
        /* NetPBM file format is big-endian */
        /*  internal gimage, need convert to host */
      } else {
        fprintf(stderr, "NetPBM color binary, maxval must be 255 or 65535\n");
        fclose(fileP);
        return(NULL);
      }
      break;
    } /* end switch on pbm_type */

    if (gimageP != NULL) {
      strncpy(gimageP->title, inFilepath, 255);
      gimageP->title[255] = '\0';
    }
  }

  fclose(fileP);

  return (gimageP);
}


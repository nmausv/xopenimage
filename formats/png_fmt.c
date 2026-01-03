/* png_fmt.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* PNG types and depths */
/*
type	depth allowed
0	1,2,4,8,16	Each pixel is a grayscale sample.
2	8,16		Each pixel is an R,G,B triple.
3	1,2,4,8		Each pixel is a palette index;
			a PLTE chunk must appear.
4	8,16		Each pixel is a grayscale sample,
			followed by an alpha sample.
6	8,16		Each pixel is an R,G,B triple,
			followed by an alpha sample.
*/

/* gImage only {bitmap, RGB24, and RGB48} */

/* conversions:
PNG			gImage
gray,depth=1		bitmap
gray,depths=2,4,8	RGB24
gray,depth=16		RGB48 (wasteful, pointless)
RGB,depth=8		RGB24
RGB,depth=16		RGB48
palette,depth=2,4,8	RGB24
grayalpha,8		RGB24
grayalpha,16		RGB48 (super pointless)
RGBalpha,8		RGB24
RGBalpha,16		RGB48
*/

/* from PNG specification 1.2 */
/*
"Pixels are always packed into scanlines with no wasted bits between pixels.
Pixels smaller than a byte never cross byte boundaries; they are packed
into bytes with the leftmost pixel in the high-order bits of a byte, the
rightmost in the low-order bits. Permitted bit depths and pixel types are
restricted so that in all cases the packing is simple and efficient.
PNG permits multi-sample pixels only with 8- and 16-bit samples, so
multiple samples of a single pixel are never packed into one byte. All
16-bit samples are stored in network byte order (MSB first)."
*/


/* System */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  /* strncpy */
#include <stdint.h>  /* for uint16_t */

/* libPNG   www.libPNG.org */
#include <png.h>
/* or if local */
/* #include "png.h" */

/* code base */
#include "../gimage.h"  /* 'gImage' struct */

#include "png_fmt.h" /* enforce declarations */


/* internal static variables */

/* lookup table, bitwise reversed, then complemented */
static unsigned char comprev[256] = {
0xFF,0x7F,0xBF,0x3F,0xDF,0x5F,0x9F,0x1F,0xEF,0x6F,0xAF,0x2F,0xCF,0x4F,0x8F,0x0F,
0xF7,0x77,0xB7,0x37,0xD7,0x57,0x97,0x17,0xE7,0x67,0xA7,0x27,0xC7,0x47,0x87,0x07,
0xFB,0x7B,0xBB,0x3B,0xDB,0x5B,0x9B,0x1B,0xEB,0x6B,0xAB,0x2B,0xCB,0x4B,0x8B,0x0B,
0xF3,0x73,0xB3,0x33,0xD3,0x53,0x93,0x13,0xE3,0x63,0xA3,0x23,0xC3,0x43,0x83,0x03,
0xFD,0x7D,0xBD,0x3D,0xDD,0x5D,0x9D,0x1D,0xED,0x6D,0xAD,0x2D,0xCD,0x4D,0x8D,0x0D,
0xF5,0x75,0xB5,0x35,0xD5,0x55,0x95,0x15,0xE5,0x65,0xA5,0x25,0xC5,0x45,0x85,0x05,
0xF9,0x79,0xB9,0x39,0xD9,0x59,0x99,0x19,0xE9,0x69,0xA9,0x29,0xC9,0x49,0x89,0x09,
0xF1,0x71,0xB1,0x31,0xD1,0x51,0x91,0x11,0xE1,0x61,0xA1,0x21,0xC1,0x41,0x81,0x01,
0xFE,0x7E,0xBE,0x3E,0xDE,0x5E,0x9E,0x1E,0xEE,0x6E,0xAE,0x2E,0xCE,0x4E,0x8E,0x0E,
0xF6,0x76,0xB6,0x36,0xD6,0x56,0x96,0x16,0xE6,0x66,0xA6,0x26,0xC6,0x46,0x86,0x06,
0xFA,0x7A,0xBA,0x3A,0xDA,0x5A,0x9A,0x1A,0xEA,0x6A,0xAA,0x2A,0xCA,0x4A,0x8A,0x0A,
0xF2,0x72,0xB2,0x32,0xD2,0x52,0x92,0x12,0xE2,0x62,0xA2,0x22,0xC2,0x42,0x82,0x02,
0xFC,0x7C,0xBC,0x3C,0xDC,0x5C,0x9C,0x1C,0xEC,0x6C,0xAC,0x2C,0xCC,0x4C,0x8C,0x0C,
0xF4,0x74,0xB4,0x34,0xD4,0x54,0x94,0x14,0xE4,0x64,0xA4,0x24,0xC4,0x44,0x84,0x04,
0xF8,0x78,0xB8,0x38,0xD8,0x58,0x98,0x18,0xE8,0x68,0xA8,0x28,0xC8,0x48,0x88,0x08,
0xF0,0x70,0xB0,0x30,0xD0,0x50,0x90,0x10,0xE0,0x60,0xA0,0x20,0xC0,0x40,0x80,0x00
};


/* internal static functions */

/***************/
/* pngBitmap() */
/***************/
static gImage*
pngBitmap(
 png_structp in_p_imgP,
 png_infop in_p_infoP,
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;
int status = 0;
int p_h;
int p_w;
int p_rowbytes;
int y;
int x;
unsigned char *gP = NULL;
/* libPNG types */
png_bytep *row_pointers = NULL;
png_byte *p_byteP = NULL;

  /* necessary ? why ? */
  png_read_update_info(in_p_imgP, in_p_infoP);

  p_w = png_get_image_width(in_p_imgP, in_p_infoP);
  p_h = png_get_image_height(in_p_imgP, in_p_infoP);

  p_rowbytes = png_get_rowbytes(in_p_imgP, in_p_infoP);

  /* malloc for libPNG to read into */
  row_pointers = malloc(sizeof(png_bytep) * p_h);
  if (row_pointers == NULL) {
    fprintf(stderr, "PNG error malloc\n");
    status = (-1);
  } else {
    for (y = 0; y < p_h; y++) {
      row_pointers[y] = malloc(p_rowbytes);
      if (row_pointers[y] == NULL) {
        fprintf(stderr, "PNG error malloc\n");
        status = (-1);
        break;
      }
    }
  }

  if (status == 0) {

    /* read the PNG */
    png_read_image(in_p_imgP, row_pointers);

    /* copy the image from PNG to gImage */
    rgiP = newBitImage(p_w, p_h);
    if (rgiP == NULL) {
      fprintf(stderr, "PNG error newBitImage\n");
      status = (-1);
    } else {

      if (inVerbose) {
        printf("%s, PNG bitmap, size: %d x %d\n", inFilepath, p_w, p_h);
      }

      gP = rgiP->data;
      for (y = 0; y < p_h; y++) {
        p_byteP = row_pointers[y];
        for (x = 0; x < p_rowbytes; x++) {
          /* PNG bitmap: left is most-sig-bit, reverse, and complement */
          *gP++ = comprev[*p_byteP++];
        }
      }
    }

    /* cleanup */
    png_read_end(in_p_imgP, in_p_infoP);
    for (y = 0; y < p_h; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);

  }

  return(rgiP);
}


/**************/
/* pngRGB24() */
/**************/
static gImage*
pngRGB24(
 png_structp in_p_imgP,
 png_infop in_p_infoP,
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;
int status = 0;
int p_type;
int p_depth;
int p_h;
int p_w;
int p_rowbytes;
int y;
int x;
unsigned char *gP = NULL;
/* libPNG types */
png_bytep *row_pointers = NULL;
png_byte *p_byteP = NULL;

  /* largely following libPNG example.c */

  p_type  = png_get_color_type(in_p_imgP, in_p_infoP);
  p_depth = png_get_bit_depth(in_p_imgP, in_p_infoP);

  /* remove alpha */
  /*  RGBA -> RGB, gray_alpha -> gray */
  png_set_strip_alpha(in_p_imgP);

  if (p_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(in_p_imgP);
  } else if (p_type == PNG_COLOR_TYPE_GRAY && p_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(in_p_imgP);
  }

  /* convert gray to RGB */
  if (p_type == PNG_COLOR_TYPE_GRAY || p_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(in_p_imgP);
  }

  png_read_update_info(in_p_imgP, in_p_infoP);

  p_h = png_get_image_height(in_p_imgP, in_p_infoP);
  p_w = png_get_image_width(in_p_imgP, in_p_infoP);

  /* note well: get rowbytes after setting all conversions */
  p_rowbytes = png_get_rowbytes(in_p_imgP, in_p_infoP);

  /* malloc for libPNG to read into */
  row_pointers = malloc(sizeof(png_bytep) * p_h);
  if (row_pointers == NULL) {
    fprintf(stderr, "PNG error malloc\n");
    status = (-1);
  } else {
    for (y = 0; y < p_h; y++) {
      row_pointers[y] = malloc(p_rowbytes);
      if (row_pointers[y] == NULL) {
        fprintf(stderr, "PNG error malloc\n");
        status = (-1);
        break;
      }
    }
  }

  if (status == 0) {

    /* read the PNG */
    png_read_image(in_p_imgP, row_pointers);

    /* copy the image from PNG to gImage */
    rgiP = newRGB24Image(p_w, p_h);
    if (rgiP == NULL) {
      fprintf(stderr, "PNG error newRGB24Image\n");
      status = (-1);
    } else {

      rgiP->gamma = 2.2; /* check for this ? */

      printf("%s, ", inFilepath);
      if (inVerbose) {
        switch(p_type) {
         case 0:
          printf("PNG grayscale"); break;
         case 2:
          printf("PNG RGB"); break;
         case 3:
          printf("PNG palette"); break;
         case 4:
          printf("PNG gray+alpha"); break;
         case 6:
          printf("PNG RGB+alpha"); break;
        }
        printf(", depth %d, size: %d x %d\n", p_depth, p_w, p_h);
      }

      gP = rgiP->data;
      for (y = 0; y < p_h; y++) {
        p_byteP = row_pointers[y];
        for (x = 0; x < p_rowbytes; x++) {
          *gP++ = *p_byteP++;
        }
      }
    }

    /* cleanup */
    png_read_end(in_p_imgP, in_p_infoP);
    for (y = 0; y < p_h; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);

  }

  return(rgiP);
}


/**************/
/* pngRGB48() */
/**************/
static gImage*
pngRGB48(
 png_structp in_p_imgP,
 png_infop in_p_infoP,
 const char *inFilepath,
 unsigned int inVerbose)
{
gImage *rgiP = NULL;
int status = 0;
int p_type;
int p_depth;
int p_h;
int p_w;
int p_rowbytes;
int y;
int x;
uint16_t *gP = NULL;
/* libPNG types */
png_bytep *row_pointers = NULL;
png_byte *pRGB16 = NULL;

  /* should only be gray16, RGB16, gray_alpha16, or RGBA16 */

  /* largely following libPNG example.c */

  p_type  = png_get_color_type(in_p_imgP, in_p_infoP);
  p_depth = png_get_bit_depth(in_p_imgP, in_p_infoP);

  /* remove alpha */
  /*  RGBA -> RGB, gray_alpha -> gray */
  png_set_strip_alpha(in_p_imgP);

  /* convert gray to RGB */
  if (p_type == PNG_COLOR_TYPE_GRAY || p_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(in_p_imgP);
  }

  png_read_update_info(in_p_imgP, in_p_infoP);

  p_h = png_get_image_height(in_p_imgP, in_p_infoP);
  p_w = png_get_image_width(in_p_imgP, in_p_infoP);

  /* note well: get rowbytes after setting all conversions */
  p_rowbytes = png_get_rowbytes(in_p_imgP, in_p_infoP);

  /* malloc for libPNG to read into */
  row_pointers = malloc(sizeof(png_bytep) * p_h);
  if (row_pointers == NULL) {
    fprintf(stderr, "PNG error malloc\n");
    status = (-1);
  } else {
    for (y = 0; y < p_h; y++) {
      row_pointers[y] = malloc(p_rowbytes);
      if (row_pointers[y] == NULL) {
        fprintf(stderr, "PNG error malloc\n");
        status = (-1);
        break;
      }
    }
  }

  if (status == 0) {

    /* read the PNG */
    png_read_image(in_p_imgP, row_pointers);

    /* copy the image from PNG to gImage */
    rgiP = newRGB48Image(p_w, p_h);
    if (rgiP == NULL) {
      fprintf(stderr, "PNG error newRGB48Image\n");
      status = (-1);
    } else {

      rgiP->gamma = 2.2; /* check for this ? */

      printf("%s, ", inFilepath);
      if (inVerbose) {
        switch(p_type) {
         case 0:
          printf("PNG grayscale"); break;
         case 2:
          printf("PNG RGB"); break;
         case 4:
          printf("PNG gray+alpha"); break;
         case 6:
          printf("PNG RGB+alpha"); break;
        }
        printf(", depth %d, size: %d x %d\n", p_depth, p_w, p_h);
      }

      gP = (uint16_t*) rgiP->data;
      for (y = 0; y < p_h; y++) {
        pRGB16 = row_pointers[y];
        for (x = 0; x < p_rowbytes; x += 2) {
          /* PNG big-endian to native 16bit */
          *gP++ = pRGB16[x] * 256 + pRGB16[x+1];
        }
      }
    }

    /* cleanup */
    png_read_end(in_p_imgP, in_p_infoP);
    for (y = 0; y < p_h; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);

  }

  return(rgiP);
}


/* PUBLIC FUNCTIONS */


/**************/
/* pngLoad() */
/**************/
gImage*
pngLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
int status = 0;
gImage *rgiP = NULL;
FILE *fP = NULL;
unsigned char head8[8];
size_t nread;
int p_depth;
int p_ret;
/* libPNG types */
png_structp  p_imgP = NULL;
png_infop    p_infoP = NULL;

  /* largely following the recommended sequence of libPNG example.c */

  fP = fopen(inFilepath, "r");
  if (fP == NULL) {
    fprintf(stderr, "PNG error fopen %s\n", inFilepath);
    status = (-1);
  }

  if (status == 0) {

    /* libPNG way to quickly check if PNG file */
    nread = fread(head8, 1, 8, fP);
    if (nread != 8) {
      fprintf(stderr, "PNG error read first 8 bytes\n");
      status = (-1);
    } else {
      /* this does not even require png_structp yet */
      if (png_sig_cmp(head8, 0, 8) != 0) {
        /* not a PNG file, silently return */
        status = (-1);
      }
    }
  }

  if (status == 0) {
    p_imgP = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);
    /* NULL -> use default libPNG error and warning functions */
    if (p_imgP == NULL) {
      fprintf(stderr, "PNG error create read struct\n");
      status = (-1);
    }
  }

  if (status == 0) {
    p_infoP = png_create_info_struct(p_imgP);
    if (p_infoP == NULL) {
      fprintf(stderr, "PNG error create info struct\n");
      status = (-1);
    }
  }

  if (status == 0) {
    p_ret = setjmp(png_jmpbuf(p_imgP));
    if (p_ret != 0) {
      fprintf(stderr, "PNG error setjmp\n");
      status = (-1);
    }
  }

  if (status == 0) {

    png_init_io(p_imgP, fP);

    png_set_sig_bytes(p_imgP, 8);

    png_read_info(p_imgP, p_infoP);

    /* troubles me - would like to have way to check these for errors */
  }

  if (status == 0) {

    p_depth = png_get_bit_depth(p_imgP, p_infoP);
    if (p_depth <= 0) {
      fprintf(stderr, "PNG error bit depth 0 or negative\n");
      status = (-1);
    } else if (p_depth == 1) {
      rgiP = pngBitmap(p_imgP, p_infoP, inFilepath, inVerbose);
    } else if (p_depth <= 8) {
      rgiP = pngRGB24(p_imgP, p_infoP, inFilepath, inVerbose);
    } else if (p_depth <= 16) {
      rgiP = pngRGB48(p_imgP, p_infoP, inFilepath, inVerbose);
    } else {
      fprintf(stderr, "PNG error bit depth greater than 16\n");
      status = (-1);
    }

    if (rgiP != NULL) {
      strncpy(rgiP->title, inFilepath, 255);
      rgiP->title[255] = '\0';
    }

  }

  /* if p_imgP or p_infoP are NULL, this shouldn't be problem */
  png_destroy_read_struct(&p_imgP, &p_infoP, NULL);
  p_imgP = NULL;
  p_infoP = NULL;

  if (fP != NULL) {
    fclose(fP);
    fP = NULL;
  }

  return(rgiP);
}


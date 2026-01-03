/* jpeg_fmt.c */

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
#include <string.h>     /* strncpy */

/* libJPEG  Independent JPEG Group IJG, preference version 6b */
#include "jpeglib.h"
/* or if part of system */
/* #include <jpeglib.h> */

/* code base */
#include "../gimage.h" /* 'gImage' struct */

#include "jpeg_fmt.h"  /* enforce declarations */

/* PUBLIC FUNCTIONS */

/**************/
/* jpegLoad() */
/**************/
gImage*
jpegLoad(
 const char *inFilepath,
 unsigned int inVerbose)
{
int status = 0;
gImage *rgiP = NULL;
unsigned char *gP = NULL;
FILE *fP = NULL;
size_t nread;
unsigned char buf[2];
int jpeg_ret = 0;
unsigned int jpeg_w;
unsigned int jpeg_h;
unsigned int jpeg_comps;
int jpeg_rowstride = 0;
unsigned char *rowP = NULL;
int i;
/* JPEG specific */
struct jpeg_error_mgr jerr;
struct jpeg_decompress_struct dinfo;
JSAMPARRAY buffer = NULL;

  fP = fopen(inFilepath, "r");
  if (fP == NULL) {
    fprintf(stderr, "JPEG error fopen %s\n", inFilepath);
    status = (-1);
  } else {
    nread = fread(buf, 1, 2, fP);
    if (nread != 2) {
      fprintf(stderr, "JPEG error fread %s\n", inFilepath);
      status = (-1);
    } else if (buf[0] != 0xFF || buf[1] != 0xD8) {
      /* not JPEG, so silently return */
      status = (-1);
    } else {
      fseek(fP, 0, SEEK_SET);
    }
  }

  if (status == 0) {

    dinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&dinfo);
    jpeg_stdio_src(&dinfo, fP);
    jpeg_ret = jpeg_read_header(&dinfo, TRUE);
    if (jpeg_ret != JPEG_HEADER_OK) {
      fprintf(stderr, "JPEG error jpeg_read_header returned %d\n", jpeg_ret);
    }
    jpeg_start_decompress(&dinfo);

    jpeg_w = dinfo.output_width;
    jpeg_h = dinfo.output_height;
    jpeg_comps = dinfo.output_components;

    rgiP = newRGB24Image(jpeg_w, jpeg_h);
    if (rgiP == NULL) {
      fprintf(stderr, "JPEG error newRGB24Image\n");
    } else {

      if (inVerbose) {
        printf("%s, JPEG, %d components, size: %d x %d\n",
          inFilepath, jpeg_comps, jpeg_w, jpeg_h); 
      }

      rgiP->gamma = 2.2; /* check for this ? */

      strncpy(rgiP->title, inFilepath, 255);
      rgiP->title[255] = '\0';

      gP = rgiP->data;

      jpeg_rowstride = dinfo.output_width * dinfo.output_components;
      buffer = (*dinfo.mem->alloc_sarray)
        ((j_common_ptr) &dinfo, JPOOL_IMAGE, jpeg_rowstride, 1);

      while (dinfo.output_scanline < dinfo.output_height) {

        jpeg_read_scanlines(&dinfo, buffer, 1);
        rowP = buffer[0];
        if (jpeg_comps == 3) {
          /* RGB JPEG */
          for (i = 0; i < jpeg_rowstride; i++) {
            *gP++ = *rowP++;
          }
        } else if (jpeg_comps == 1) {
          /* grayscale JPEG */
          for (i = 0; i < jpeg_rowstride; i++) {
            *gP++ = *rowP;
            *gP++ = *rowP;
            *gP++ = *rowP;
            rowP++;
          } 
        }
      }

    }
    jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);

    fclose(fP);

  }

  return(rgiP);
}


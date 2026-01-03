/* fileformats.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* Standard C library */
#include <stdlib.h>
#include <stdio.h>     /* fprintf, perror */
#include <string.h>    /* strlen, strncmp */
#include <errno.h>     /* errno */

/* POSIX Issue 1 */
#include <sys/stat.h>  /* stat, struct stat, S_IFMT, S_IFDIR */
#include <unistd.h>    /* access, R_OK */


/* code base */
#include "gimage.h"      /* 'gImage' struct */

#include "fileformats.h" /* declarations, consistency */

#include "options.h"     /* getOption, Option, OptionSet */

#include "formats/xbitmap_fmt.h"
#include "formats/netpbm_fmt.h"
#include "formats/tiff_fmt.h"
#include "formats/jpeg_fmt.h"
#include "formats/png_fmt.h"
#include "formats/webp_fmt.h"


/* INTERNAL */

struct fileformats FileFormats[] = {
 { tiffLoad,    "tiff",      "TIFF"},
 { jpegLoad,    "jpeg",      "JPEG"},
 { pngLoad,     "png",       "PNG"},
 { webpLoad,    "webp",      "WebP"},
 { pbmLoad,     "pbm",       "NetPBM pbm,pgm,ppm"},
 { xbitmapLoad, "xbm",       "XBitMap xbm"},
 { NULL,         NULL,        NULL}
};


/* INTERNAL (static) FUNCTIONS */

/*****************/
/* fileIsValid() */
/*****************/
/* return: */
/*  -1 true - file exists, not a directory, and access allowed */
/*   0 false - error */
int
fileIsValid(
 const char *inFilepath)
{
int ret = 0;
struct stat filestat;

  if (inFilepath == NULL) {
    ret = 0;
  } else if (strcmp(inFilepath, "stdin") == 0) {
    /* stdin ? */
    ret = -1;
  } else if (stat(inFilepath, &filestat) == 0) {
    if ( (filestat.st_mode & S_IFMT) == S_IFDIR) {
      /* is directory, so not valid */
      ret = 0;
    } else if (access(inFilepath, R_OK) == 0) {
      /* file should be okay to read: not a directory, and access is allowed */
      ret = -1;
    }
  } else {
    ret = 0;
  }

  return(ret);
}


/* PUBLIC FUNCTIONS */

/***************/
/* loadImage() */
/***************/
/* load a file into gImage (generic image) */
gImage*
loadImage(
 OptionSet *globalopts,
 OptionSet *options,
 const char *filepath,
 unsigned int verbose)
{
Option *opt = NULL;
gImage *gimageP = NULL;
int     i;
int     formatmatched = 0;

  if (fileIsValid(filepath) == 0) {
    /* fileIsValid is FALSE */

    if (errno == ENOENT) {
      fprintf(stderr, "%s: file not found\n", filepath);
    } else {
      perror(filepath);
    }
    gimageP = NULL; /* extra sure */

  } else {
    /* fileIsValid is TRUE */

    /* see if there is a "format" option, and if there is, use it */
    opt = getOption(globalopts, FORMAT);
    if (opt == NULL) {
      opt = getOption(options, FORMAT);
    }
    if (opt != NULL) {
      for (i = 0; FileFormats[i].loader != NULL; i++) {
        if (!strncmp(FileFormats[i].format_id, opt->info.format_id, strlen(opt->info.format_id))) {
          /* specified format_id matched, so try to use that loader */
          formatmatched = -1;
          gimageP = FileFormats[i].loader(filepath, verbose);
          if (gimageP == NULL) {
            fprintf(stderr, "%s does not look like a \"%s\" format.\n",
              filepath, opt->info.format_id); 
            /* can fall through, try to iterate over known formats */
          }
          break;
        }
      }
      if (formatmatched == 0) {
        fprintf(stderr, "\"%s\" is not a supported file format (will try to guess the format)\n", opt->info.format_id);
      }
    }

    /* file is valid */
    /*  either no format option, so gimageP is NULL */
    /*  or format did not work, so gimageP still NULL */
    /*  or format worked, and loaded (gimageP not NULL) */
    if (gimageP == NULL) {
      /* try each format in order of FileFormats array */
      for (i = 0; FileFormats[i].loader != NULL; i++) {
        gimageP = FileFormats[i].loader(filepath, verbose);
        if (gimageP != NULL) {
          break;
        }
      }
    }
 
    if (gimageP == NULL) {
      fprintf(stderr, "%s: unknown or unsupported file format\n", filepath);
    }
  }

  return(gimageP);
}


/**********************/
/* supportedFormats() */
/**********************/
/* tell user what file formats we support */
void
supportedFormats(void)
{
int i = 0;

  printf("format_id   Description\n");
  printf("---------   -----------\n");
  for (i = 0; FileFormats[i].format_id != NULL; i++) {
    printf("%-9s   %s\n", FileFormats[i].format_id, FileFormats[i].description);
  }
}


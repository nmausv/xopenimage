/* fileformats.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef fileformats_h
#define fileformats_h

/**
 * @defgroup fileformats
 *
 * \#include "fileformats.h"
 */


#include "gimage.h"  /* gImage struct */
#include "options.h" /* OptionSet */


struct fileformats {
  gImage* (*loader)(const char *, unsigned int);
  char*   format_id;
  char*   description;
};


/** supportedFormats
 * @ingroup fileformats
 */
void supportedFormats(void);


/** loadImage
 * @ingroup fileformats
 * @param[in] globalopts
 * @param[in] options
 * @param[in] filename
 * @param[in] verbose
 * @return gImage
 *
 * load a file into a gImage (generic image)
 * default is to iterate over the supported file formats
 */
gImage* loadImage(OptionSet *globalopts, OptionSet *options, const char *filename, unsigned int verbose);


#endif


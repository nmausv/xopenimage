/* gimage.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef gimage_h
#define gimage_h

/**
 * @defgroup gimage gImage utilities
 *
 * \#include "gimage.h"
 */


/* defines */
/*  change to enum? */
#define IBAD (0)
#define IBITMAP (1)
#define IRGB24 (2)
#define IRGB48 (3)

/* custom generic 'gImage' structure */

typedef struct gimage_struct {
 unsigned int   gitype;     /* type of gimage: IBITMAP IRGB24 IRGB48 */
 unsigned int   depth;      /* depth: bitmap 1, color 24 or 48 */
 unsigned int   width;      /* width in pixels */
 unsigned int   height;     /* height in pixels */
 float          gamma;      /* gamma correction */
 char           title[256]; /* name of gimage */
 char           background[256]; /* color string for bitmap background */
 char           foreground[256]; /* color string for bitmap foreground */
 unsigned char *data;       /* data */
} gImage;



/* macros */
#define BITMAPP(IMAGE) ((IMAGE)->gitype == IBITMAP)
#define RGB24P(IMAGE)  ((IMAGE)->gitype == IRGB24)
#define RGB48P(IMAGE)  ((IMAGE)->gitype == IRGB48)



/** newBitImage
 * @ingroup gimage
 * @param[in] width
 * @param[in] height
 * @return new gImage
 */
gImage* newBitImage(unsigned int width, unsigned int height);


/** newRGB24Image
 * @ingroup gimage
 * @param[in] width
 * @param[in] height
 * @return new gImage
 */
gImage* newRGB24Image(unsigned int width, unsigned int height);


/** newRGB48Image
 * @ingroup gimage
 * @param[in] width
 * @param[in] height
 * @return new gImage
 */
gImage* newRGB48Image(unsigned int width, unsigned int height);


/** freeImageData
 * @ingroup gimage
 * @param[in] gimageP
 */
void freeImageData(gImage *gimageP);


/** freeImage
 * @ingroup gimage
 * @param[in] gimageP
 */
void freeImage(gImage *gimageP);


#endif


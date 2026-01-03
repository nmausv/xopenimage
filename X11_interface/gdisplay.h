/* gdisplay.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef gdisplay_h
#define gdisplay_h

/**
 * @defgroup gdisplay  generic interface for display routines
 * declarations and interface for generic display 
 *
 * \#include "gdisplay.h"
 */


#include "../gimage.h"  /* 'gImage' struct */
#include "../options.h" /* OptionSet */


typedef struct gdisplay_struct* gdisplay;


/** gdinit
 * @ingroup gdisplay
 */
gdisplay gdinit(const char *string);

/** gdwidth
 * @ingroup gdisplay
 */
unsigned int gdwidth(gdisplay gd);

/** gdheight
 * @ingroup gdisplay
 */
unsigned int gdheight(gdisplay gd);

/** gdbyteorderLSB
 * @ingroup
 * @return -1(true) for LSB, 0(false) for MSB
 */
int gdbyteorderLSB(gdisplay gd);


/** gdbitorderLSB
 * @ingroup
 * @return -1(true) for LSB, 0(false) for MSB
 */
int gdbitorderLSB(gdisplay gd);


/** gdImageInWindow
 * @ingroup gdisplay
 */
char gdImageInWindow(gdisplay gd, gImage *ingimageP,
 OptionSet *global_options, OptionSet *image_options,
 int argc, char *argv[], unsigned int verbose);


/** gdfinish
 * @ingroup gdisplay
 */
void gdfinish(gdisplay gd);


#endif


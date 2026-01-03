/* colorspace.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef colorspace_h
#define colorspace_h

/**
 * @defgroup colorspace Colorspace routines
 * routines for manipulating colors within and
 * between colorspaces
 *
 * \#include "colorspace.h"
 */

/* linearization - assuming 8bit component starting point */
/*  lookup table (array) much faster */
extern long double sRGBlin[256];
extern long double aRGBlin[256];


/** lin2sRGB
 * @ingroup colorspace
 * @param[in] l linear sRGB value, range [0...1]
 * @return 'component' sRGB value, range [0...1]
 *
 * Converts a linearized sRGB value to a 'component' sRGB value.
 * 'component' value is in range [0...1]
 * For typical use,, the component still needs to converted to 8bit integer,
 * such as: rint(rgb*255);
 */
double lin2sRGB(double l);

/** lin2aRGB
 * @ingroup colorspace
 * @param[in] l linear AdobeRGB value, range [0...1]
 * @return 'component' AdobeRGB value, range [0...1]
 *
 * Converts a linearized AdobeRGB value to a 'component' AdobeRGB value.
 * 'component' value is in range [0...1]
 * For typical use, the component still needs to converted to 8bit integer,
 * such as: rint(rgb*255);
 */
double lin2aRGB(double l);


/* between colorspaces */

/* sRGB */

/** XYZ2sRGB
 * @ingroup colorspace
 * @param[in] X X in CIEXYZ
 * @param[in] Y Y in CIEXYZ
 * @param[in] Z Z in CIEXYZ
 * @param[out] R red in sRGB, range [0...1]
 * @param[out] G green in sRGB, range [0...1]
 * @param[out] B blue in sRGB, range [0...1]
 *
 * Converts CIEXYZ to (linearized) sRGB triplet
 */
void XYZ2sRGB(double X, double Y, double Z, double *R, double *G, double *B);

/** sRGB2XYZ
 * @ingroup colorspace
 * @param[in] R red in sRGB, range[0...1]
 * @param[in] G green in sRGB, range [0...1]
 * @param[in] B blue in sRGB, range [0...1]
 * @param[out] X X in CIEXYZ
 * @param[out] Y Y in CIEXYZ
 * @param[out] Z Z in CIEXYZ
 *
 * Converts (linearized) sRGB triplet to CIEXYZ
 */
void sRGB2XYZ(double R, double G, double B, double *X, double *Y, double *Z);


/* AdobeRGB */

/** XYZ2aRGB
 * @ingroup colorspace
 * @param[in] X X in CIEXYZ
 * @param[in] Y Y in CIEXYZ
 * @param[in] Z Z in CIEXYZ
 * @param[out] R red in AdobeRGB, range [0...1]
 * @param[out] G green in AdobeRGB, range [0...1]
 * @param[out] B blue in AdobeRGB, range [0...1]
 *
 * Converts CIEXYZ to (linearized) AdobeRGB triplet
 */
void XYZ2aRGB(double X, double Y, double Z, double *R, double *G, double *B);

/** aRGB2XYZ
 * @ingroup colorspace
 * @param[in] R red in AdobeRGB, range [0...1]
 * @param[in] G green in AdobeRGB, range [0...1]
 * @param[in] B blue in AdobeRGB, range [0...1]
 * @param[out] X X in CIEXYZ
 * @param[out] Y Y in CIEXYZ
 * @param[out] Z Z in CIEXYZ
 *
 * Converts (linearized) AdobeRGB triplet to CIEXYZ
 */
void aRGB2XYZ(double R, double G, double B, double *X, double *Y, double *Z);


/** a2sRGB
 * @ingroup colorspace
 * @param[in] aR red in AdobeRGB, range [0...1]
 * @param[in] aG green in AdobeRGB, range [0...1]
 * @param[in] aB blue in AdobeRGB, range [0...1]
 * @param[out] sR red in sRGB, range [0...1]
 * @param[out] sG green in sRGB, range [0...1]
 * @param[out] sB blue in sRGB, range [0...1]
 *
 * Convert (linearized) AdobeRGB to (linearized) sRGB<br>
 * Converts from the wide gamut AdobeRGB to the smaller, but standard sRGB
 */
void a2sRGB(double aR, double aG, double aB, double *sR, double *sG, double *sB);


/* CIELAB */

/** XYZ2Lab
 * @ingroup colorspace
 * @param[in] X X in CIEXYZ
 * @param[in] Y Y in CIEXYZ
 * @param[in] Z Z in CIEXYZ
 * @param[out] L L in CIELAB
 * @param[out] a a in CIELAB
 * @param[out] b b in CIELAB
 *
 * Convert from CIEXYZ to CIELAB
 */
void XYZ2Lab(double X, double Y, double Z, double *L, double *a, double *b);

/** Lab2XYZ
 * @ingroup colorspace
 * @param[in] L L in CIELAB
 * @param[in] a a in CIELAB
 * @param[in] b b in CIELAB
 * @param[out] X X in CIEXYZ
 * @param[out] Y Y in CIEXYZ
 * @param[out] Z Z in CIEXYZ
 *
 * Convert from CIELAB to CIEXYZ
 */
void Lab2XYZ(double L, double a, double b, double *X, double *Y, double *Z);


#endif


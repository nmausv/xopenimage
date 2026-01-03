/* bitdownscale.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* downscale a bitmap image */

/* C System */
#include <stdlib.h>
#include <stdio.h>  /* fprintf */
#include <math.h>   /* modf */

/* code base */
#include "bitdownscale.h" /* declaration consistency */

/* PUBLIC FUNCTIONS */

/******************/
/* bitdownscale() */
/******************/
/* dstP must already be allocated */
int
bitdownscale(
 const unsigned char *srcP,
 unsigned int src_xdim,
 unsigned int src_ydim,
 unsigned char *dstP,
 unsigned int dst_xdim,
 unsigned int dst_ydim)
{
int status = 0;
unsigned int srowbytes;
unsigned int drowbytes;
unsigned int xs, ys, xd, yd;
unsigned char bitset;
unsigned char byte;
double  sx1,  sy1;
/* f - fractional part */
double fsx0, fsx1, fsy0, fsy1;
/* i - integer    part */
double isx0, isx1, isy0, isy1;
double src_area;
double fx, fy;
double sum;

  /* input checking */
  if ( (src_ydim == 0) || (src_xdim == 0) ||
       (dst_ydim == 0) || (dst_xdim == 0) ) {
    fprintf(stderr, "bitdownscale error: no dimensions permitted to be zero\n");
    status = -1;
  } else if ( (dst_ydim > src_ydim) || (dst_xdim > src_xdim) ) {
    fprintf(stderr, "bitdownscale error: dst dimensions cannot be larger than src\n");
    status = -1;
  } else { 

    srowbytes = (src_xdim + 7) / 8;
    drowbytes = (dst_xdim + 7) / 8;

    src_area = ((double) src_xdim * src_ydim) / ((double) dst_xdim * dst_ydim);

    isy0 = 0.0;
    fsy0 = 0.0;
    for (yd = 0; yd < dst_ydim; yd++) {

       sy1 = ((yd + 1.0) / dst_ydim) * src_ydim;
      fsy1 = modf(sy1, &isy1);

      isx0 = 0.0;
      fsx0 = 0.0;
      for (xd = 0; xd < dst_xdim; xd++) {

         sx1 = ((xd + 1.0) / dst_xdim) * src_xdim;
        fsx1 = modf(sx1, &isx1);

        sum = 0.0;

        ys = isy0;
        fy = 1.0 - fsy0;
        do {
          xs = isx0;
          fx = 1.0 - fsx0;

          do {

            /* convert from xs,ys to byte and bit of src */
            /*  if bit set, multiply by (fx * fy) and add to sum */
            byte = *(srcP + ys * srowbytes + (xs / 8) );
            bitset = byte & (0x01 << (xs % 8));
            if (bitset != 0) {
              sum += (fx * fy);
            }

            xs++;
            fx = (xs < isx1 ? 1.0 : fsx1);
          } while (xs < sx1);

          ys++;
          fy = (ys < isy1 ? 1.0 : fsy1);
        } while (ys < sy1);

        /* if average >= 0.5, set bit */
        /*  determine byte and bit from xd,yd */ 
        if ((sum / src_area) >= 0.5) {
          bitset = 0x01 << (xd % 8);
          *(dstP + yd * drowbytes + (xd / 8)) |= bitset;
        }

        isx0 = isx1;
        fsx0 = fsx1;
      }

      isy0 = isy1;
      fsy0 = fsy1;
    }

  }

  return(status);
}


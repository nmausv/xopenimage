/* downscale.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* Feature test switches */
#define _POSIX_C_SOURCE 1

/* System headers */
#include <stdlib.h>
#include <stdio.h> /* fprintf() */
#include <math.h>  /* modf() */

/* Local headers */
#include "downscale.h"

/* Macros */
   /* NONE */
/* File scope variables */
   /* NONE */
/* External variables */
   /* NONE */
/* External functions */
   /* NONE */
/* Structures and unions */
   /* NONE */
/* Signal catching functions */
   /* NONE */

/* Functions */

/***************/
/* downscale() */
/***************/
/* assumes RGB in range [0,1] (not integer 0-255) */
/* ideally would be linearized RGB floating point */
/* assumes 2 dimensional square pixels */
/* assumes 3 components (no good for images with alpha or grayscale) */
/* process along rowlines = x-direction */
int
downscale(
 unsigned char samp_per_pixel,
 const float *src,
 unsigned int src_xdim,
 unsigned int src_ydim,
 float *dst,
 unsigned int dst_xdim,
 unsigned int dst_ydim)
{
int status = 0;

/* internal floating point in double precision */
double yratio, xratio;
double area;
double sum1, sum2, sum3;
double fy, fx;

double sy1, sx1; /* (fractional) coord of dst pixel in src coord system */
  /* x=0 represents left edge of left most pixel,
     x=src_xdim-1 represents left edge of last pixel in row,
     x=src_xdim represents right edge of last pixel in row */
double fsy0, fsx0, fsy1, fsx1; /* fractional part of coords */
double isy0, isx0, isy1, isx1; /* integer part of coords */

/* the only integer values, the indices */
unsigned int yd, xd;   /* dst pixel indices */
unsigned int ys, xs;   /* src pixel indices */

  /* input checking */
  /*  must avoid divide by zero */
  if ( (src_ydim == 0) || (src_xdim == 0) ||
       (dst_ydim == 0) || (dst_xdim == 0) ) {
    fprintf(stderr, "error: dimensions cannot be zero\n");
    status = -1;
  } else {

    yratio = (double)src_ydim / dst_ydim;
    xratio = (double)src_xdim / dst_xdim;
    area = yratio * xratio; /* must not be zero */


    isy0 = 0.0;
    fsy0 = 0.0;
    for (yd = 0; yd < dst_ydim; yd++) {
       sy1 = ((yd+1.0)/dst_ydim) * src_ydim;
      fsy1 = modf(sy1, &isy1);

      isx0 = 0.0;
      fsx0 = 0.0;
      for (xd = 0; xd < dst_xdim; xd++) {

         sx1 = ((xd+1.0)/dst_xdim) * src_xdim;
        fsx1 = modf(sx1, &isx1);

        /* assumes 3 components */
        sum1 = 0.0;
        sum2 = 0.0;
        sum3 = 0.0;

        ys = isy0;
        fy = 1.0 - fsy0;
        do {

          xs = isx0;
          fx = 1.0 - fsx0;
          do {

            sum1 += src[3*(ys*src_xdim + xs)   ] * fy * fx;
            sum2 += src[3*(ys*src_xdim + xs) +1] * fy * fx;
            sum3 += src[3*(ys*src_xdim + xs) +2] * fy * fx;

            xs++;
            fx = (xs < isx1 ? 1.0 : fsx1);
          } while (xs < sx1);

          ys++;
          fy = (ys < isy1 ? 1.0 : fsy1);
        } while (ys < sy1);

        /* set dest pixel */
        /* if need function, something like putPixel(xd, yd, R, G, B) */
        dst[3*(yd*dst_xdim + xd)    ] = sum1 / area;
        dst[3*(yd*dst_xdim + xd) + 1] = sum2 / area;
        dst[3*(yd*dst_xdim + xd) + 2] = sum3 / area;

        isx0 = isx1;
        fsx0 = fsx1;
      }

      isy0 = isy1;
      fsy0 = fsy1;
    }
  }

  return(status);
}


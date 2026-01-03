/* gdisplay.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* implementation for X11 */
/*  of functions declared in gdisplay.h */

/* system */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> /* for uint16_t */

/* X11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h> /* XVisualInfo */

/* code base */
#include "gdisplay.h"

#include "../gimage.h" /* gImage */


/* INTERNAL */
/* structures */
struct gdisplay_struct {
 Display* xdisplayP;
 int      xbyteLSB; /* 0=false=MSBFirst, -1=true=LSBFirst */
 int      xscrnum;
 Visual*  xvisP;
 int      xscrnwidth;
 int      xscrnheight;
 Window   ximgwin;
 GC       xgc;
};


/* static internal functions */

/***************/
/* gi4bitmap() */
/***************/
static XImage*
gi4bitmap(
 gImage *ingiP,
 gdisplay ingdP)
{
/* X11 types */
XImage *xiP = NULL;
Colormap xdefcmap;
XColor xc_exact;
XColor xc_screen;
XGCValues xgcv;
unsigned long xgcmask;
/* standard types */
unsigned char *xidataP = NULL;
unsigned int rowbytes;
unsigned int depth = 1;
int pad = 8;
int bytes_per_line = 0; /* if 0, XCreateImage will then figure it out in created structure */
int i;

  rowbytes = (ingiP->width + 7) / 8;
  xidataP = malloc(rowbytes * ingiP->height);
  if (xidataP == NULL) {
    fprintf(stderr, "gi4bitmap malloc error\n");
    xiP = NULL;
  } else {

    /* modify GC (ingdP->xgc) to set background, foreground */
    /*  GC was created in gdinit */
    if (ingiP->background[0] != '\0' || ingiP->foreground[0] != '\0') {
      xdefcmap = DefaultColormap(ingdP->xdisplayP, ingdP->xscrnum);
      xgcmask = 0;
      if (ingiP->background[0] != '\0') {
        XAllocNamedColor(ingdP->xdisplayP, xdefcmap, ingiP->background, &xc_exact, &xc_screen);
        xgcv.background = xc_screen.pixel;
        xgcmask |= GCBackground;
      }
      if (ingiP->foreground[0] != '\0') {
        XAllocNamedColor(ingdP->xdisplayP, xdefcmap, ingiP->foreground, &xc_exact, &xc_screen);
        xgcv.foreground = xc_screen.pixel;
        xgcmask |= GCForeground;
      }
      XChangeGC(ingdP->xdisplayP, ingdP->xgc, xgcmask, &xgcv);
    }

    for (i = 0; i < rowbytes * ingiP->height; i++) {
      xidataP[i] = ingiP->data[i];
    }
    xiP = XCreateImage(ingdP->xdisplayP, ingdP->xvisP, depth, XYBitmap, 0,
            xidataP, ingiP->width, ingiP->height, pad, bytes_per_line);
  }

  return(xiP);
}


/**************/
/* gi4rgb24() */
/**************/
static XImage*
gi4rgb24(
 gImage *ingiP,
 gdisplay ingdP)
{
XImage *rxiP = NULL;
unsigned char *xidataP = NULL;
unsigned char *gP = NULL;
unsigned char *xP = NULL;
unsigned int w;
unsigned int h;
unsigned int x;
unsigned int y;

  w = ingiP->width;
  h = ingiP->height;

  xidataP = malloc(4 * h * w);
  if (xidataP == NULL) {
    fprintf(stderr, "gi4rgb24 malloc fail\n");
    rxiP = NULL;
  } else {
    gP = ingiP->data;
    xP = xidataP;
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
        /* X11 order by 'mask' red is byte 2, green byte 1, blue byte 0 */
        *xP++ = *(gP+2); /* blue */
        *xP++ = *(gP+1); /* green */
        *xP++ = *gP;     /* red */
        *xP++ = 0;       /* pad */
        gP+= 3;
      }
    }

    rxiP = XCreateImage(ingdP->xdisplayP, ingdP->xvisP, 24, ZPixmap, 0,
             xidataP, w, h, 8, 0);
  }

  return(rxiP);
}


/**************/
/* gi4rgb48() */
/**************/
static XImage*
gi4rgb48(
 gImage *ingiP,
 gdisplay ingdP)
{
XImage *rxiP = NULL;
unsigned char *xidataP = NULL;
unsigned char *xP = NULL;
uint16_t *gP = NULL;
unsigned int w;
unsigned int h;
unsigned int ix;
unsigned int iy;

  w = ingiP->width;
  h = ingiP->height;

  xidataP = malloc(4 * h * w);
  if (xidataP == NULL) {
    fprintf(stderr, "gi4rgb48 malloc fail\n");
    rxiP = NULL;
  } else {
    gP = (uint16_t *)ingiP->data;
    xP = xidataP;
    for (iy = 0; iy < ingiP->height; iy++) {
      for (ix = 0; ix < ingiP->width; ix++) {
        *xP++ = *(gP+2) / 256; /* blue */
        *xP++ = *(gP+1) / 256; /* green */
        *xP++ = *gP / 256;     /* red */
        *xP++ = 0;             /* pad */
        gP += 3;
      }
    }

    rxiP = XCreateImage(ingdP->xdisplayP, ingdP->xvisP, 24, ZPixmap, 0,
           xidataP, w, h, 8, 0);
  }

  return(rxiP);
}



/******************/
/* errorHandler() */
/******************/
/* called by X11, but not public to other code */
int
errorHandler(
 Display* inxdisP,
 XErrorEvent* inxerrP)
{
char errText[256];

  XGetErrorText(inxdisP, inxerrP->error_code, errText, 255);
  errText[255] = '\0'; /* safety - null terminate */
  fprintf(stderr, "X11 error: %s on 0x%x\n",
    errText, inxerrP->resourceid);

  abort();

  /* does not actually return */
  return(-1);
}


/* PUBLIC FUNCTIONS */

/************/
/* gdinit() */
/************/
gdisplay
gdinit(
 const char* inStr)
{
gdisplay     rgdP = NULL;
XVisualInfo  xvi;
unsigned long xpxl_white;
XGCValues xgcvals;
int backing;
int xret = 0;

  rgdP = malloc( sizeof(struct gdisplay_struct) );
  if (rgdP == NULL) {
    fprintf(stderr, "gdinit: malloc() fail\n");
  } else {
    rgdP->xdisplayP = XOpenDisplay(inStr);
    if (rgdP->xdisplayP == NULL) {
      fprintf(stderr, "XOpenDisplay fail\n");
      free(rgdP);
      rgdP = NULL;
    } else {
      XSetErrorHandler(errorHandler);
      switch(ImageByteOrder(rgdP->xdisplayP)) {
       case LSBFirst: rgdP->xbyteLSB = -1; break;
       case MSBFirst: rgdP->xbyteLSB = 0; break;
      }
      rgdP->xscrnum = DefaultScreen(rgdP->xdisplayP);
    }
  }

  if (rgdP != NULL) {
    /* check for TrueColor 24 */
    xret = XMatchVisualInfo(rgdP->xdisplayP, rgdP->xscrnum, 24, TrueColor, &xvi);
    if (xret == 0) {
      fprintf(stderr, "X11 does not have a TrueColor 24 Visual\n");
      XCloseDisplay(rgdP->xdisplayP);
      free(rgdP);
      rgdP = NULL;
    } else {
      rgdP->xvisP = xvi.visual;
    }
  }

  if (rgdP != NULL) {
    rgdP->xscrnwidth = DisplayWidth(rgdP->xdisplayP, rgdP->xscrnum);
    rgdP->xscrnheight = DisplayHeight(rgdP->xdisplayP, rgdP->xscrnum);

/*
    backing = DoesBackingStore(ScreenOfDisplay(rgdP->xdisplayP, rgdP->xscrnum));
    switch(backing) {
     case Always:     printf("BackingStore Always\n"); break;
     case WhenMapped: printf("BackingStore WhenMapped\n"); break;
     case NotUseful:  printf("BackingStore NotUseful\n"); break;
    }
*/
  }

  if (rgdP != NULL) {
    xpxl_white = WhitePixel(rgdP->xdisplayP, rgdP->xscrnum);

    /* create the window, will change attributes and map later */
    rgdP->ximgwin = XCreateSimpleWindow(rgdP->xdisplayP, RootWindow(rgdP->xdisplayP, rgdP->xscrnum),
      128, 128, 128, 128, 0, xpxl_white, xpxl_white);
    /* check if error ? */
    xret = XSelectInput(rgdP->xdisplayP, rgdP->ximgwin,
      ExposureMask | KeyPressMask | StructureNotifyMask);
    if (xret != 1) {
      fprintf(stderr, "XSelectInput error %d\n", xret);
    }

    xgcvals.foreground = BlackPixel(rgdP->xdisplayP, rgdP->xscrnum);
    xgcvals.background = xpxl_white;
    rgdP->xgc = XCreateGC(rgdP->xdisplayP, rgdP->ximgwin,
      GCForeground | GCBackground, &xgcvals);
    /* check if XCreateGC error ? */
  }

  return(rgdP);
}


/*************/
/* gdwidth() */
/*************/
unsigned int
gdwidth(
 gdisplay ingdP)
{
unsigned int w;

  if (ingdP != NULL) {
    w = DisplayWidth(ingdP->xdisplayP, ingdP->xscrnum);
  } else {
    w = 0;
  }
  return(w);
}


/**************/
/* gdheight() */
/**************/
unsigned int
gdheight(
 gdisplay ingdP)
{
unsigned int h;

  if (ingdP != NULL) {
    h = DisplayHeight(ingdP->xdisplayP, ingdP->xscrnum);
  } else {
    h = 0;
  }
  return(h);
}


/********************/
/* gdbyteorderLSB() */
/********************/
int
gdbyteorderLSB(
 gdisplay ingdP)
{
int ret = 0;
  if (ImageByteOrder(ingdP->xdisplayP) == LSBFirst) {
    ret = -1;
  }
  return(ret);
}


/*******************/
/* gdbitorderLSB() */
/*******************/
int
gdbitorderLSB(
 gdisplay ingdP)
{
int ret = 0;
  if (BitmapBitOrder(ingdP->xdisplayP) == LSBFirst) {
    ret = -1;
  }
  return(ret);
}


/**************/
/* gdfinish() */
/**************/
void
gdfinish(
 gdisplay ingdP)
{
  XDestroyWindow(ingdP->xdisplayP, ingdP->ximgwin);
  XFreeGC(ingdP->xdisplayP, ingdP->xgc);
  XCloseDisplay(ingdP->xdisplayP);
  free(ingdP);
}


/*********************/
/* gdImageInWindow() */
/*********************/
char
gdImageInWindow(
 gdisplay      ingdP,
 gImage       *ingiP,
 OptionSet    *global_options,
 OptionSet    *image_options,
 int           argc,
 char         *argv[],
 unsigned int  verbose)
{
XImage *xiP = NULL;
XEvent xevt;
KeySym xkeysym;
XComposeStatus xcompst;
int keycnt = 0;
int xret = 0;
int status = 0;
char buf[16];
char r;

  /* create the XImage */
  switch(ingiP->gitype) {
   case IBITMAP: xiP = gi4bitmap(ingiP, ingdP); break;
   case IRGB24:  xiP = gi4rgb24(ingiP, ingdP); break;
   case IRGB48:  xiP = gi4rgb48(ingiP, ingdP); break;
   default: fprintf(stderr, "?invalid gimage type\n");
  }

  XResizeWindow(ingdP->xdisplayP, ingdP->ximgwin, ingiP->width, ingiP->height);

  xret = XMapWindow(ingdP->xdisplayP, ingdP->ximgwin);
  if (xret != 1) {
    fprintf(stderr, "XMapWindow error %d\n", xret);
  }

  do {

    xret = XNextEvent(ingdP->xdisplayP, &xevt);
    if (xret != 0) {
      fprintf(stderr, "XNextEvent error %d\n", xret);
    }

    switch(xevt.type) {

     case Expose:
      if (xevt.xexpose.count != 0) {
        break;
      }
      xret = XPutImage(ingdP->xdisplayP, ingdP->ximgwin, ingdP->xgc,
        xiP, 0, 0, 0, 0, ingiP->width, ingiP->height);
      break;

     case KeyPress:
      keycnt = XLookupString((XKeyEvent*)&xevt, buf, 8, &xkeysym, &xcompst);
      if (buf[0] == 'q' || buf[0] == 'Q') {
        status = 1;
        r = 'q';
      } else if (buf[0] == ' ' || buf[0] == 'n' || buf[0] == 'N') {
        status = 1;
        r = 'n';
      } else if (buf[0] == 'p' || buf[0] == 'P') {
        status = 1;
        r = 'p';
      } else if (buf[0] == '>') {
        status = 1;
        r = '>';
      } else if (buf[0] == '<') {
        status = 1;
        r = '<';
      }
      break;

    } /* end switch xevt.type */

  } while (status == 0);

  XUnmapWindow(ingdP->xdisplayP, ingdP->ximgwin);

  XDestroyImage(xiP);
  xiP = NULL;

  return(r);
}


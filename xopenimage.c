/* xopenimage.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h>
#include <stdio.h>       /* printf, fprintf */
#include <string.h>      /* strcmp() */
#include <signal.h>      /* signal() */
/* POSIX Issue 1 */
#include <unistd.h>      /* fork() */
/* POSIX Issue 4 */
#include <strings.h>     /* bcopy() */

/* code base */
#include "gimage.h"      /* 'gImage' struct */

#include "fileformats.h" /* loadImage */
#include "error.h"       /* internalError */
#include "usageHelp.h"   /* usageHelp */

/* transforms */
#include "transforms/zoom.h"
/* not yet working */
#include "transforms/gamma.h"
#include "transforms/rotate.h"


/* interface to display */
#include "X11_interface/gdisplay.h"
 /* gdisplay type, gdinit(), gdfinish(), gdwidth(), gdheight(),
    imageInWindow() */


/* global variables - ick */
char *ProgramName = "xopenimage";


/* Internal (static) functions */

/**********************/
/* doProcessOnImage() */
/**********************/
/* this may modify ingiP */
static gImage*
doProcessOnImage(
 gImage* ingiP,
 Option* inOption,
 unsigned int inVerbose)
{
gImage* rgiP = ingiP;

  switch (inOption->type) {

   case BACKGROUND:
    if (rgiP->depth == 1) {
      strncpy(rgiP->background, inOption->info.background, 255);
      rgiP->background[255] = '\0';
      /* gdisplay module parses the color string */
    }
    break;

   case FOREGROUND:
    if (rgiP->depth == 1) {
      strncpy(rgiP->foreground, inOption->info.foreground, 255);
      rgiP->foreground[255] = '\0';
      /* gdisplay module parses the color string */
    }
    break;

   case GAMMA:
    gammacorrect(rgiP, inOption->info.gamma, inVerbose);
    break;

   case ROTATE:
    rgiP = rotate(ingiP, inOption->info.rotate, inVerbose);
    break;

   case TITLE:
    /* overwrite */
    strncpy(rgiP->title, inOption->info.title, 255);
    rgiP->title[255] = '\0';
    break;

   case ZOOM:
    rgiP = zoom(ingiP, inOption->info.zoom.x,
                inOption->info.zoom.y, inVerbose);
    /* this function is called by processImage() */
    /*  if rgiP is not same as ingiP, processImage() frees old one */ 
    break;

   default:
    break;
  }

  return(rgiP);
}


/******************/
/* processImage() */
/******************/
/* process a list of options on an image */
static gImage*
processImage(
 gImage *ingiP,
 OptionSet *global_options,
 OptionSet *image_options)
{
Option*       opt = NULL;
gImage*       rgiP = NULL;
gImage*       tmpgimageP = NULL;
unsigned int  verbose;

  rgiP = ingiP;

  verbose = (getOption(global_options, VERBOSE) != NULL);

  /* go through the global options and process them */
  for (opt = global_options->options; opt != NULL; opt = opt->next) {

    /* if option already exists locally for this image, ignore it */
    if (getOption(image_options, opt->type)) {
      continue;
    }  
    tmpgimageP = doProcessOnImage(rgiP, opt, verbose);
    if (tmpgimageP != rgiP) {
      freeImage(rgiP);
      rgiP = tmpgimageP;
    }
  }

  /* go through local options */
  for (opt = image_options->options; opt != NULL; opt = opt->next) {
    tmpgimageP = doProcessOnImage(rgiP, opt, verbose);
    if (tmpgimageP != rgiP) {
      freeImage(rgiP);
      rgiP = tmpgimageP;
    }
  }

  return(rgiP);
}



/**********/
/* main() */
/**********/
int
main(
 int argc,
 char *argv[])
{
/* code base types / structs */
gImage       *dispgimageP = NULL;
gImage       *newgimageP = NULL;
gImage       *tmpgimageP = NULL;
OptionSet    *global_options = NULL;
OptionSet    *image_options = NULL;
OptionSet    *optset = NULL;
OptionSet    *tmpset = NULL;
Option       *opt = NULL;
gdisplay      gdP;
/* standard types */
char         *tag = NULL;
char         *dname = NULL;
char          gdret;
unsigned int  screenwidth = 0;
unsigned int  screenheight = 0;
unsigned int  winwidth = 0;
unsigned int  winheight = 0;
/* unsigned int  fullscreen;*/  /* flag(bool) for fullscreen */
unsigned int  shrinktofit; /* flag(bool) for fit in screen */
unsigned int  verbose;     /* flag(bool) for verbose reporting */
int i;

  /* set up internal error handlers */
  signal(SIGSEGV, internalError);
  signal(SIGBUS,  internalError);
  signal(SIGFPE,  internalError);
  signal(SIGILL,  internalError);

  ProgramName = argv[0];

  if (argc < 2) {
    usageHelp();
    /* does not return */
  }

  /* defaults and other initial settings.  some of these depend on what
   * our name was when invoked. */

  processOptions(argc, argv, &global_options, &image_options);

  verbose = (getOption(global_options, VERBOSE) != NULL);

  /* if no images are specified
   * then this invocation is a no-op */
  if ( (image_options->next == NULL) &&
       (getOption(image_options, NAME) == NULL) ) {
    fprintf(stderr, "%s: No images were specified.\n", argv[0]);
    usageHelp();
    /* NOTREACHED */
  }

  opt = getOption(global_options, DISPLAY);
  dname = (opt != NULL ? opt->info.display : NULL);
  gdP = gdinit(dname);
  if (gdP == NULL) {
    fprintf(stderr, "Cannot open display %s\n", dname);
    exit(EXIT_FAILURE);
  }

  screenwidth  = gdwidth(gdP);
  screenheight = gdheight(gdP);


  /* background ourselves if the user asked us to */
#ifndef NO_FORK
  if (getOption(global_options, FORK))
    switch(fork()) {
     case -1:
      perror("fork");
      /* FALLTHRU */
     case 0:
      break;
     default:
      exit(EXIT_SUCCESS);
    }
#endif /* !NO_FORK */

/*  fullscreen = (getOption(global_options, FULLSCREEN) != NULL);
*/
  shrinktofit = (getOption(global_options, SHRINKTOFIT) != NULL);

  opt = getOption(global_options, GEOMETRY);
  if (opt != NULL) {
    winwidth  = opt->info.geometry.w;
    winheight = opt->info.geometry.h;
  } else {
    winwidth  = 0;
    winheight = 0;
  }

  /* load in each named image */
  for (optset = image_options; optset != NULL; optset = optset->next) {

get_another_image:

    if (! (opt = getOption(optset, NAME))) {

      /* ??? -onroot is gone */
      /* this gets post-processing accomplished for -onroot */
      if (dispgimageP != NULL) {
        dispgimageP = processImage(dispgimageP, global_options, optset);
      }
      continue;

    } else {

      newgimageP = loadImage(global_options, optset, opt->info.name, verbose);

      if (newgimageP == NULL) {
        continue;
      }
    }

    /* retitle the image if we were asked to */
    opt = getOption(optset, TITLE);
    if (opt != NULL) {
      strncpy(newgimageP->title, opt->info.title, 255);
      newgimageP->title[255] = '\0';
    }

    /* if necessary, fix zoom */
    if ((optset == image_options) && shrinktofit &&
        !getOption(optset, ZOOM)) {

      opt = newOption(ZOOM);

      opt->info.zoom.x = opt->info.zoom.y = 
        (newgimageP->width  - (screenwidth  * 0.9) >
         newgimageP->height - (screenheight * 0.9) ?
         ((float)screenwidth  * 0.9)
         / (float)newgimageP->width  * 100.0 :
         ((float)screenheight * 0.9)
         / (float)newgimageP->height * 100.0);
      addOption(optset, opt);
    }

    newgimageP = processImage(newgimageP, global_options, optset);

    dispgimageP = newgimageP;

redisplay_in_window:

    gdret = gdImageInWindow(gdP, dispgimageP, global_options,
           optset, argc, argv, verbose);

    switch(gdret) {

     case '\0':
      /* window got nuked */
      gdfinish(gdP);
      gdP = NULL;
      exit(EXIT_FAILURE);

     case '\003':
     case 'q':
      /* quit */
      gdfinish(gdP);
      gdP = NULL;
      exit(EXIT_SUCCESS);

     case ' ':
     case 'n':
      /* next image */
      opt = getOption(optset->next, GOTO);
      if (opt != NULL) {
        /* there is a GOTO target */
        tag = opt->info.go_to;
        printf("tag = %s\n", tag);

        for (tmpset = image_options; tmpset; tmpset= tmpset->next) {
          if ((opt = getOption(tmpset, NAME)) &&
              !strcmp(tag, opt->info.name)) {
            optset = tmpset;
            freeImage(dispgimageP);
            dispgimageP = NULL;
            goto get_another_image; /* goto ick */
          }
        }
        fprintf(stderr, "Target for -goto %s was not found\n", tag);
      }

      break;

     case 'p':
      /* previous image */
      /* todo: check if first image, can't go previous */

      for (tmpset = image_options; tmpset && (tmpset->next != optset);
        tmpset = tmpset->next) {
        /* EMPTY */
        ;
      }
      if (tmpset == NULL) {
        goto redisplay_in_window; /* goto ick */
      }
      optset = tmpset;

      goto get_another_image; /* goto ick */
      /* does not fall through, because 'goto' */

     case '<':
      /* < for smaller */

      opt = getOption(optset, ZOOM);
      if (opt == NULL) {
        opt = newOption(ZOOM);
        opt->info.zoom.x = opt->info.zoom.y = 50.0;
        addOption(optset, opt);
      } else {
        opt->info.zoom.x = opt->info.zoom.x ? opt->info.zoom.x * 0.5 : 50;
        opt->info.zoom.y = opt->info.zoom.y ? opt->info.zoom.y * 0.5 : 50;
      }

      tmpgimageP = dispgimageP;
      dispgimageP = zoom(dispgimageP, 50, 50, verbose);
      if (tmpgimageP != dispgimageP) {
        freeImage(tmpgimageP);
        tmpgimageP = NULL;
      }

      goto redisplay_in_window; /* goto ick */
      /* does not fall through, because 'goto' */

     case '>':
      /* > for bigger */

      opt = getOption(optset, ZOOM);
      if (opt == NULL) {
        opt = newOption(ZOOM);
        opt->info.zoom.x = opt->info.zoom.y = 200.0;
        addOption(optset, opt);
      } else {
        opt->info.zoom.x = opt->info.zoom.x ? opt->info.zoom.x * 2.0 : 200;
        opt->info.zoom.y = opt->info.zoom.y ? opt->info.zoom.y * 2.0 : 200;
      }

      tmpgimageP = dispgimageP;
      dispgimageP = zoom(dispgimageP, 200, 200, verbose);
      if (tmpgimageP != dispgimageP) {
        freeImage(tmpgimageP);
        tmpgimageP = NULL;
      }
      goto redisplay_in_window; /* goto ick */
      /* does not fall through, because 'goto' */

     default:
      printf("UI unrecognized command?\n");

    } /* end switch on return from gdImageInWindow() */

/*
    freeImage(dispgimageP);
    dispgimageP = NULL;
*/
/* need to understand relationship newgimageP, dispgimageP, tempgimageP */

  } /* end of for loop of options */

  /* graceful end */

  gdfinish(gdP);
  gdP = NULL;

  return(EXIT_SUCCESS);
}


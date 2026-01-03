/* options.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef options_h
#define options_h

/**
 * @defgroup options
 * functions for handling options
 *
 * \#include "options.h"
 */

/* enums, structures, and typedefs */

/* enum with the options in it.  If you add one to this you also have to
 * add its information to Options[] in options.c before it becomes available.
 */
typedef enum option_id {

  /* global options */

  OPT_NOTOPT= 0, OPT_BADOPT, OPT_SHORTOPT, OPT_IGNORE,
  DISPLAY, FORK, FULLSCREEN, GEOMETRY, HELP, QUIET,
  SHRINKTOFIT, SUPPORTED, VERBOSE, VER_NUM,

  /* local options */

  BACKGROUND, FOREGROUND, FORMAT, GAMMA, GLOBAL, GOTO,
  INVERT, NAME, ROTATE, TITLE, XZOOM, YZOOM, ZOOM
} OptionId;

/* option structure
 *  (could? make this opaque, hide definition?)
 */
typedef struct option {
  enum option_id type;
  union {
    struct {
      unsigned int x, y;      /* location to load image at */
    } at;
    char         *background; /* background color for mono images */
    char         *display;    /* display name */
    char         *foreground; /* foreground color for mono images */
    char         *format_id;  /* file format of image */
    float         gamma;      /* gamma value */
    struct {
      char *string;
      unsigned int w;
      unsigned int h;
    } geometry;
    char         *go_to;      /* label to go to */
    char         *name;       /* name of image */
    unsigned int  rotate;     /* # of degrees to rotate image */
    char         *title;      /* title of image */
    struct {
      unsigned int x, y;      /* zoom factors */
    } zoom;
  } info;
  struct option *next;
} Option;

/* image name and option structure used when processing arguments
 */
typedef struct option_set {
  Option            *options; /* image processing options */
  struct option_set *next;
} OptionSet;

/* option information array
 */
typedef struct option_array {
  char     *name;        /* name of the option minus preceeding '-' */
  OptionId  option_id;   /* OptionId of this option */
  char     *args;        /* arguments this option uses or NULL if none */
  char     *description; /* description of this option */
} OptionArray;



/* FUNCTIONS */

/** addOption
 * @ingroup options
 * @param[in] optset
 * @param[in] newopt
 *
 * add newopt to optset.
 * optset must already exist/ already allocated.
 * optset will be modified.
 */
void addOption(OptionSet *optset, Option *newopt);

/** getNextTypeOption
 * @ingroup options
 * @param[in] optstring
 * @param[in] name
 * @param[in] value
 * 
 */
int getNextTypeOption(char **opt_string, char **name, char **value);

/** getOption
 * @ingroup options
 * @param[in] optset
 * @param[in] type
 * @return Option
 *
 * searches optset for an Option matching type
 */
Option* getOption(OptionSet *optset, OptionId type);

/** newOption
 * @ingroup options
 * @param[in] type
 * @return Option
 *
 * allocates a new Option of specified type
 */
Option* newOption(OptionId type);

/** processOptions
 * @ingroup options
 * @param[in] argc
 * @param[in] argv
 * @param[out] global on return contains global OptionSet
 * @param[out] image on return contains image OptionSet
 */
void processOptions(int argc, char *argv[], OptionSet **global, OptionSet **image);

#endif


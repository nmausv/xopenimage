/* options.c */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

/* C standard library */
#include <stdlib.h> /* malloc, free */
#include <stdio.h>  /* printf, fprintf, stderr */
#include <string.h> /* strlen, strncmp, index */

/* code base */
#include "version.h"     /* SOURCE: VERSION, PATCHLEVEL */

#include "options.h"     /* declarations, consistency */

#include "fileformats.h" /* supportedFormats() */
#include "usageHelp.h"   /* usageHelp */


/* options array and definitions.  If you add something to this you also
 * need to add its OptionId in options.h.
 */

static OptionArray Options[] = {

  /* global options */

  { "display",    DISPLAY,    "display_string", "\
Indicate the X display you would like to use.", },
  { "fork",       FORK,       NULL, "\
Background automatically.  Turns on -quiet.", },
  { "fullscreen", FULLSCREEN, NULL, "\
Use the whole screen for displaying an image.", },
  { "geometry",   GEOMETRY,   "window_geometry", "\
Specify the size of the display window.  Ignored if -fullscreen is given.", },
  { "help",       HELP,       "[option ...]", "\
Give help on a particular option or series of options.  If no option is\n\
supplied, a list of available options is given.", },
  { "quiet",      QUIET,      NULL, "\
Turn off verbose mode.", },
  { "shrink",      SHRINKTOFIT, NULL, "\
Shrink an image larger than screen to fit.", }, 
  { "supported",  SUPPORTED,  NULL, "\
Give a list of the supported file formats.", },
  { "verbose",    VERBOSE,    NULL, "\
Turn on verbose mode.", },
  { "version",    VER_NUM,    NULL, "\
Show the version number of this version of xopenimage.", },

  /* image options */

  { "background", BACKGROUND, "color", "\
Set the background pixel color for a monochrome image.  See -foreground and\n\
-invert.", },
  { "foreground", FOREGROUND, "color", "\
Set the foreground pixel color for a monochrome image.  See -background and\n\
-invert.", },
  { "format",     FORMAT,     "format_id", "\
Specifies the file format of image so that run-time determination is unnecessary.\n\
This generally speeds up loading, but is usually not necessary.\n\
Use the -supported option to get a list a file formats supported\n\
along with their format_id.", },
  { "gamma",      GAMMA,      "value", "\
Adjust the image's colors by a gamma value.  1.0 is the default, which does\n\
nothing.  Values under 1.0 darken the image, values higher brighten it.", },
  { "global",     GLOBAL,     NULL, "\
Force the next option to apply to all images rather that to the current\n\
image.", },
  { "goto",       GOTO,       "image_name", "\
Start displaying a particlar image.  This is useful for creating image display\n\
loops.  If two images with the target name exist, the first in the argument list\n\
will be used.", },
  { "invert",     INVERT,     NULL, "\
Invert a monochrome image.  This is the same as specifying \"-foreground black\"\n\
and \"-background white\".", },
  { "name",       NAME,       NULL, "\
Specify that the next argument is to be the name of an image.\n\
This is useful for loading images whose names look to be options.", },
  { "rotate",     ROTATE,     "degrees", "\
Rotate the image by 90, 180, or 270 degrees.", },
  { "title",      TITLE,      "image_title", "\
Title for the image (used in title bars).", },
  { "xzoom",      XZOOM,      "percentage", "\
Zoom the image along the X axis by a percentage.  See -zoom.", },
  { "yzoom",      YZOOM,      "percentage", "\
Zoom the image along the X axis by a percentage.  See -zoom.", },
  { "zoom",       ZOOM,       NULL, "\
Zoom the image along both axes. Values smaller than 100 will reduce the\n\
size of the image, values greater than 100 will enlarge it.  See also\n\
-xzoom and -yzoom.", },
  { NULL,         OPT_NOTOPT, NULL, NULL }
};


/* Internal (static) functions */


/****************/
/* optionName() */
/****************/
/* uses Options defined above in lexical scope */
/* used by both getFloat and getInteger */
static char*
optionName(
 OptionId inType)
{
int i = 0;

  for (i = 0; Options[i].name != NULL; i++) {
    if (Options[i].option_id == inType) {
      return(Options[i].name);
    }
  }
  return("<unknown option>");
}


/**************/
/* getFloat() */
/**************/
static float
getFloat(
 OptionId inType,
 char *inString)
{
float fret = 0.0;

  if (sscanf(inString, "%f", &fret) != 1) {
    fprintf(stderr, "Bad floating point argument for the %s option\n",
            optionName(inType));
    usageHelp();
    /* NOTREACHED */
  }

  return(fret);
}


/****************/
/* getInteger() */
/****************/
static int
getInteger(
 OptionId inType,
 char *inString)
{
int iret = 0;  
int scan_status = 0;

  if (strncmp(inString, "0x", 2) == 0) {
    scan_status = sscanf(inString, "%x", &iret);
  } else {
    scan_status = sscanf(inString, "%d", &iret);
  }  

  if (scan_status != 1) {
    fprintf(stderr, "Bad integer argument for the %s option\n",
          optionName(inType));
    usageHelp();
    /* NOTREACHED */
  }

  return(iret);
}


/*****************/
/* listOptions() */
/****************/
/* uses Options defined above in lexical scope */
static void
listOptions(void)
{
int i;
int width = 0;

  printf("\nThe options are:\n\n");
  for (i = 0; Options[i].name != NULL; i++) {
    width += strlen(Options[i].name) + 2;
    if (width > 78) {
      printf("\n");
      width = strlen(Options[i].name) + 2;
    }
    printf("%s%s", Options[i].name, (Options[i + 1].name ? ", " : "\n\n"));
  }
}


/******************/
/* helpOnOption() */
/******************/
/* uses Options defined above in lexical scope */
/* used by help() */
static int
helpOnOption(
 char *inOption)
{
int i = 0;
int foundone = 0;

  if (*inOption == '-') {
    inOption++;
  }

  for (i = 0; Options[i].name != NULL; i++) {
    if (!strncmp(Options[i].name, inOption, strlen(inOption))) {
      printf("Option: %s\nUsage: %s -%s %s\nDescription:\n%s\n\n",
             Options[i].name,
             "xopenimage",
             Options[i].name, (Options[i].args ? Options[i].args : ""),
             Options[i].description);
      foundone = 1;
    }
  }

  if (!foundone) {
    printf("No option \"%s\".\n", inOption);
  }

  return(foundone);
}


/*****************/
/* optionUsage() */
/*****************/
/* uses optionName() and helpOnOption() */
static void
optionUsage(
 OptionId inType)
{
  helpOnOption(optionName(inType));
  exit(EXIT_FAILURE);
}


/**********/
/* help() */
/**********/
/* uses helpOnOption() and listOptions() */
static void
help(
 char *option)
{
char buf[BUFSIZ];

  /* batch help facility */

  if (option) {
    if (!helpOnOption(option))
      printf("\
Type \"xopenimage -help [option ...]\" to get help on a particular option or\n\
\"xopenimage -help\" to enter the interactive help facility.\n\n");
    return;
  }

  /* interactive help facility */

  printf("\nxopenimage Interactive Help Facility\n\n");
  printf("\
Type \"?\" for a list of options, or \".\" or \"quit\" to leave the interactive\n\
help facility.\n");
  for (;;) {
    printf("help> ");
    buf[BUFSIZ - 1]= '\0';
    if (fgets(buf, BUFSIZ - 1, stdin) == NULL) {
      printf("quit\n");
      exit(EXIT_SUCCESS);
    }
    while(buf[0] && buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1]= '\0';

    /* help keywords
     */

    if (!strcmp(buf, "")) {
      printf("Type \"?\" for a list of options\n");
      continue;
    }
    if (!strcmp(buf, "?")) {
      ;
    } else if (!strcmp(buf, "quit") || !strcmp(buf, ".")) {
      exit(EXIT_SUCCESS);
    } else if (helpOnOption(buf)) {
      continue;
    }
    listOptions();
    printf("\
You may get this list again by typing \"?\" at the \"help>\" prompt, or leave\n\
the interactive help facility with \".\" or \"quit\".\n");
  }
}


/****************/
/* killOption() */
/****************/
/* this kills all options of a particular type in a particular set */
static void
killOption(
 OptionSet *inOptset,
 OptionId inType)
{
Option *opt = NULL;

  if (!inOptset) {
    return;
  }
  for (opt = inOptset->options; opt != NULL; opt = opt->next) {
    if (opt->type == inType) {
      opt->type = OPT_IGNORE;
    }
  }
}


/******************/
/* newOptionSet() */
/******************/
static OptionSet*
newOptionSet(void)
{
OptionSet *optset = NULL;

  optset = malloc(sizeof(OptionSet));
  if (optset == NULL) {
    fprintf(stderr, "malloc fail in newOptionSet\n");
  }
  optset->options = NULL;
  optset->next = NULL;
  return(optset);
}


/******************/
/* optionNumber() */
/******************/
/* uses Options defined above in lexical scope */
static OptionId
optionNumber(
 char *arg)
{
int i;
int j;

  if ((*arg) != '-') {
    return(OPT_NOTOPT);
  }

  for (i = 0; Options[i].name; i++) {
    if (!strncmp(arg + 1, Options[i].name, strlen(arg) - 1)) {
      for (j = i + 1; Options[j].name; j++) {
        if (!strncmp(arg + 1, Options[j].name, strlen(arg) - 1)) {
          return(OPT_SHORTOPT);
        }
      }
      return(Options[i].option_id);
    }
  }
  return(OPT_BADOPT);
}


/* PUBLIC FUNCTIONS */

/***************/
/* addOption() */
/***************/
void
addOption(
 OptionSet *optset,
 Option *newopt)
{
Option *opt;

  if (optset->options) {
    for (opt= optset->options; opt->next; opt= opt->next)
      /* EMPTY */
      ;
    opt->next= newopt;
  }
  else
    optset->options= newopt;
}


/***********************/
/* getNextTypeOption() */
/***********************/
/* this is used by readers to parse options strings */
/*  of the form name[=value][,...] */
int
getNextTypeOption(
 char **opt_string,
 char **name,
 char **value)
{
static char option_name[BUFSIZ];
static char option_value[BUFSIZ];
char *p;

  /* if no string, don't get anything
   */
  if (!opt_string || !*opt_string || !**opt_string)
    return 0;

  /* look for comma, the option delimiter
   */
  p = index(*opt_string, ',');
  if (p != NULL) {
    strncpy(option_name, *opt_string, p - *opt_string);
    option_name[p - *opt_string] = '\0';
    *opt_string = p + 1; /* increment to next option */
  }
  else {
    strcpy(option_name, *opt_string);
    *opt_string += strlen(*opt_string); /* increment to end of string */
  }
  *name = option_name;

  /* look for equals sign, the start of a value
   */
  p = index(option_name, '=');
  if (p != NULL) {
    strcpy(option_value, p + 1);
    *p = '\0'; /* stomp equals sign */
    *value = option_value;
  }
  else
    *value = NULL; /* no value */

  return(1);
}


/***************/
/* getOption() */
/***************/
/* return the option structure for a typed option */
Option*
getOption(
 OptionSet *optset,
 OptionId type)
{
Option *opt;

  if (optset)
    for (opt= optset->options; opt; opt= opt->next)
      if (type == opt->type)
        return(opt);
  return(NULL);
}


/***************/
/* newOption() */
/***************/
Option*
newOption(OptionId type)
{
Option *opt;

  opt = malloc(sizeof(Option));
  if (opt == NULL) {
    fprintf(stderr, "malloc fail in newOption\n");
  }
  opt->type = type;
  opt->next = NULL;
  return(opt);
}


/********************/
/* processOptions() */
/********************/
/* process the argument list into an option array. */
/*  the first option in the returned list is the set of global options. */
void
processOptions(
 int argc,
 char *argv[],
 OptionSet **rglobal,
 OptionSet **rimage)
{
OptionSet *global_options = NULL;
OptionSet *image_options = NULL;
OptionSet *curset = NULL;
Option *newopt = NULL;
int i = 0;
int global_opt = 0;

  global_options = newOptionSet();
  newopt = newOption(VERBOSE);
  addOption(global_options, newopt);

  image_options = newOptionSet();
  curset = image_options;

  for (i = 1; i < argc; i++) {

    newopt = newOption(optionNumber(argv[i]));

    switch (newopt->type) {

     case OPT_BADOPT:
      printf("%s: Bad option\n", argv[i]);
      usageHelp();
      /* NOTREACHED */

     case OPT_IGNORE:
      free(newopt);
      continue;

     case NAME:
      if (++i >= argc) {
        optionUsage(NAME);
      }
      /* FALLTHRU */

     case OPT_NOTOPT:
      /* NOTOPT means presume this is a filename for an image */
      newopt->type = NAME;
      newopt->info.name = argv[i];
      addOption(curset, newopt);
      curset->next = newOptionSet();
      curset = curset->next;
      continue;

     case OPT_SHORTOPT:
      printf("%s: Not enough characters to identify option\n", argv[i]);
      usageHelp();
      /* NOTREACHED */

    /* process options global to everything */

     case DISPLAY:
      if (++i >= argc) {
        optionUsage(DISPLAY);
      }
      newopt->info.display = argv[i];
      global_opt = 1;
      break;

     case FORK:
#ifdef NO_FORK
      fprintf(stderr, "%s is not supported under this operating system",
              optionName(FORK));
      continue;
#else
      global_opt = 1;
      break;
#endif

     case FULLSCREEN:
      global_opt = 1;
      break;

     case GEOMETRY:
      if (++i >= argc) {
        optionUsage(GEOMETRY);
      }
      if (sscanf(argv[i], "%dx%d",
                 &newopt->info.geometry.w, &newopt->info.geometry.h) != 2) {
        optionUsage(GEOMETRY);
      }
      newopt->info.geometry.string = argv[i];
      global_opt = 1;
      break;

     case GLOBAL:
      global_opt = 1; /* next option will be put on global argument list */
      continue;

     case HELP:
      if (++i >= argc) {
        help(NULL);
      } else {
        do {
          help(argv[i++]);
        } while (i < argc); 
      }
      exit(EXIT_SUCCESS);

     case QUIET:
      killOption(global_options, VERBOSE);
      global_opt = 1;
      break;

     case SHRINKTOFIT:
      global_opt = 1;
      break;

     case SUPPORTED:
      supportedFormats();
      exit(EXIT_SUCCESS);

     case VERBOSE:
      global_opt = 1;
      break;

     case VER_NUM:
      printf("xopenimage version %s.%s\n", VERSION, PATCHLEVEL);
      exit(EXIT_SUCCESS);

    /* process options local to an image */

     case BACKGROUND:
      if (++i >= argc) {
        optionUsage(BACKGROUND);
      }
      newopt->info.background = argv[i];
      break;

     case FOREGROUND:
      if (++i >= argc) {
        optionUsage(FOREGROUND);
      }
      newopt->info.foreground = argv[i];
      break;

     case FORMAT:
      if (++i >= argc) {
        optionUsage(FORMAT);
      }
      newopt->info.format_id = argv[i];
      break;

     case GAMMA:
      if (++i >= argc) {
        optionUsage(GAMMA);
      }
      newopt->info.gamma = getFloat(GAMMA, argv[i]);
      break;

     case GOTO:
      if (++i >= argc) {
        optionUsage(GOTO);
      }
      newopt->info.go_to = argv[i];
      break;

     case INVERT:
      killOption(curset, FOREGROUND);
      killOption(curset, BACKGROUND);
      newopt->type = FOREGROUND;
      newopt->info.foreground = "white";  /* <-- ? okay ? */
      if (global_opt) {
        addOption(global_options, newopt);
      } else {
        addOption(curset, newopt);
      }
      newopt = newOption(BACKGROUND);
      newopt->info.background = "black";  /* <-- ? okay ? */
      break;

     case ROTATE:
      if (++i >= argc) {
        optionUsage(ROTATE);
      }
      newopt->info.rotate = getInteger(ROTATE, argv[i]);
      if (newopt->info.rotate % 90) {
        fprintf(stderr, "Argument to %s must be a multiple of 90 (ignored)\n",
                optionName(ROTATE));
        newopt->type= OPT_IGNORE;
      } else {
        while (newopt->info.rotate < 0)
          newopt->info.rotate += 360;
        while (newopt->info.rotate > 360)
          newopt->info.rotate -= 360;
      }
      break;

     case TITLE:
      if (++i >= argc) {
        optionUsage(TITLE);
      }
      newopt->info.title = argv[i];
      break;

     case XZOOM:
      if (++i >= argc) {
        optionUsage(XZOOM);
      }
      newopt->type = ZOOM;
      newopt->info.zoom.x = getInteger(XZOOM, argv[i]);
      newopt->info.zoom.y = 0;
      break;

     case YZOOM:
      if (++i >= argc) {
        optionUsage(YZOOM);
      }
      newopt->type = ZOOM;
      newopt->info.zoom.x = 0;
      newopt->info.zoom.y = getInteger(YZOOM, argv[i]);
      break;

     case ZOOM:
      if (++i >= argc) {
        optionUsage(ZOOM);
      }
      newopt->info.zoom.x = newopt->info.zoom.y = getInteger(ZOOM, argv[i]);
      if (global_opt) {
        addOption(global_options, newopt);
      } else {
        addOption(curset, newopt);
      }
      newopt = newOption(YZOOM);
      newopt->info.zoom.y = getInteger(ZOOM, argv[i]);
      break;

     default:

      /* this should not happen! */

      fprintf(stderr, "%s: Internal error parsing arguments\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    if (global_opt) {
      addOption(global_options, newopt);
      global_opt = 0;
    } else {
      addOption(curset, newopt);
    }

  }

  *rglobal = global_options;
  *rimage  = image_options;

}


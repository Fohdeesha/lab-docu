/***************************************************************************
 *                                                                         *
 *  Copyright 2012 LSI Corporation.  All rights reserved.                  *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Corporation.  The  *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Corporation.                                    *
 *                                                                         *
 ***************************************************************************
*/
#include "inc/getopt.h"

/* This function gets the next option letter from the console input.
   It is supposed to be equivalent to the UNIX getopt function.
   Options are preceeded by a dash, currently a slash is not allowed.
   Multiple options after one dash are valid, and arguments after
   the option with a colon between are supported.*/

/* Common varibles. */

int     opterr = 1, /* error messages printed? default: yes */
        optind = 1, /* index into parent argv vector  */
        optopt;     /* character checked for validity */

char    *optarg;    /* argument associated with option */

#define EMSG    ""

char    *progname;  /* may also be defined elsewhere  */


static void
error(char *pch)    /* error message routine */
{
    if (!opterr)
        return;     /* without printing */

    fprintf(stderr, "%s: %s: %c\n",
            ((NULL != progname) ? progname : "getopt"), pch, optopt);
}


int
getopt(int argc, char *argv[], char *ostr)
{
    static char     *place = EMSG;  /* option letter processing */
    char            *oli;           /* option letter list index */

    progname = argv[0];

    if (!*place)
    {   /* update scanning pointer */
        if (optind >= argc ||
            (*(place = argv[optind]) != '-' &&
             *(place = argv[optind]) != '/') ||
             !*++place)
        {
            return EOF;
        }
        if (*place == '-')
        {   /* found "--"; ignore; (end of options) */
            ++optind;
            return EOF;
        }
    }

    /* option letter okay? */
    optopt = (int)*place;
    ++place;
    oli = strchr(ostr, optopt);
    if ((optopt == (int)':') ||
        !(oli = strchr(ostr, optopt)))
    {
        if (!*place)
            ++optind;

        error("illegal option");
        return BADCH;
    }

    if (*++oli != ':')  /* Check for argument after option */
    {   /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++optind;
    }
    else
    {   /* need an argument */
        if (*place)
            optarg = place;     /* no white space */
        else
            if (argc <= ++optind)
            {   /* no arg */
                place = EMSG;
                error("option requires an argument");
                return BADCH;
            }
            else
                optarg = argv[optind];  /* white space */
        place = EMSG;
        ++optind;
    }
    return optopt;  /* return option letter */
}

/* vi: set sw=4 ts=4 sts=4 et :iv */

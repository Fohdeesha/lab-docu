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

#if !EFI
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#endif

#ifndef _GETOPT_
#define _GETOPT_

int getopt(int argc, char *argv[], char *optstring);

extern char *optarg;		/* returned arg to go with this option */
extern int optind;		/* index to next argv element to process */
extern int opterr;		/* should error messages be printed? */
extern int optopt;

#define BADCH ('?')

#endif /* _GETOPT */

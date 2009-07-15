/*
 * ---------- header ----------------------------------------------------------
 *
 * project       infinite
 *
 * license       GPL
 *
 * file          /home/mycure/infinite/sources/pig/pig.h
 *
 * created       julien quintard   [mon oct 22 12:27:48 2007]
 * updated       julien quintard   [wed nov 14 11:29:19 2007]
 */

/**
 * \file pig.h
 * Public header file for pig
 */

/**
 * \page PIG - POSIX Infinite Gateway
 *
 * \section pig_intro INTRODUCTION
 * Pig layer is used to interface with FUSE FS abstraction.
 *
 */



#ifndef PIG_H
#define PIG_H

/*
 * ---------- macros ----------------------------------------------------------
 */

/**
 * Version de FUSE utilisee.
 */
#define FUSE_USE_VERSION 26

/*
 * for pread()/pwrite()
 */
#ifdef linux
# define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/xattr.h>

#endif

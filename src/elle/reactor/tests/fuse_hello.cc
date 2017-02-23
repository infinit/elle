/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include <iostream>
#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <elle/reactor/fuse.hh>
#include <elle/reactor/scheduler.hh>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strncmp(path, hello_path, strlen(hello_path)) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	char fpath[1024];
	for (int i=0; i<20; ++i)
	{
	  sprintf(fpath, "%s%i", hello_path + 1, i);
	  filler(buf, fpath, NULL, 0);
	}
	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strncmp(path, hello_path, strlen(hello_path)) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
  std::cerr << "reading file..." << std::endl;
  reactor::sleep(1_sec);
  std::cerr << "..." << std::endl;
	size_t len;
	(void) fi;
	if(strncmp(path, hello_path, strlen(hello_path)) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

static struct fuse_operations hello_oper;


fuse* f;
bool goon = true;

static void loop()
{
  std::cerr << "entering loop" << std::endl;
  reactor::fuse_loop_mt(f);
  std::cerr << "exiting loop" << std::endl;
  goon = false;
}

static void ping()
{
  while (goon)
  {
    std::cerr << "ping" << std::endl;
    reactor::sleep(5_sec);
  }
}

int main(int argc, char *argv[])
{
  hello_oper.getattr	= hello_getattr;
	hello_oper.readdir	= hello_readdir;
	hello_oper.open		= hello_open;
	hello_oper.read		= hello_read;
  reactor::Scheduler s;
  //char* mount_point = 0;
  //int multithread = 0;
  //f = fuse_setup(argc, argv, &hello_oper, sizeof(hello_oper),
  //                     &mount_point, &multithread, NULL);
  std::vector<std::string> opts;
  opts.push_back(argv[0]);
  for (int i=2; i<argc; ++i)
    opts.push_back(argv[i]);
  f = reactor::fuse_create(argv[1], opts, &hello_oper, sizeof(hello_oper), 0);
  reactor::Thread thread(s, "loop", []{loop();});
  reactor::Thread thread2(s, "ping", []{ping();});
	s.run();
	reactor::fuse_destroy(f, argv[1]);
}
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <string.h>

extern "C"
{
  struct dirent*
  readdir(DIR *dirp)
  {
    typedef struct dirent* (*Original)(DIR*);
    Original original = reinterpret_cast<Original>(dlsym(RTLD_NEXT, "readdir"));
    struct dirent* res = (*original)(dirp);
    while (res && strcmp(res->d_name, ".drake") == 0)
      res = (*original)(dirp);
    return res;
  }
}

#ifndef FUSE_HH
# define FUSE_HH

#include <string>
#include <vector>

struct fuse;
struct fuse_operations;

namespace reactor
{
  /// Run the fuse main loop in current thread
  void fuse_loop(fuse* f);
  /// Run the fuse main loop, process each request in a new thread.
  void fuse_loop_mt(fuse* f);
  fuse* fuse_create(std::string const& mountpoint,
                    std::vector<std::string> const& arguments,
                    const struct fuse_operations *op, size_t op_size,
                    void* user_data);
  void fuse_destroy(fuse* f, std::string const& mountpoint);
}

#endif
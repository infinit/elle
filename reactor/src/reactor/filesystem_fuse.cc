#include <reactor/filesystem.hh>

#include <unordered_map>

#include <reactor/fuse.hh>
#include <reactor/thread.hh>

#include <boost/filesystem.hpp>

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 26

#include <fuse/fuse.h>

ELLE_LOG_COMPONENT("reactor.filesystem.fuse");

namespace std
{
  template<> struct hash<boost::filesystem::path>
  {
    size_t operator()(const boost::filesystem::path& p) const
    {
      return boost::filesystem::hash_value(p);
    }
  };
}

namespace reactor
{
  namespace filesystem
  {
    static int fusop_getattr(const char *path, struct stat *stbuf)
    {
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.stat(stbuf);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
		{
		  try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.list_directory(
          [&](std::string const& filename, struct stat* stbuf) {
            filler(buf, filename.c_str(), stbuf, 0);
        });
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
		}

		static int fusop_open(const char *path, struct fuse_file_info *fi)
		{
		  try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        auto handle = p.open(fi->flags);
        fi->fh = (decltype(fi->fh)) handle.release();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
		}

		static int fusop_read(const char *path, char *buf, size_t size, off_t offset,
		                      struct fuse_file_info *fi)
		{
		  try
		  {
		    Handle* handle = (Handle*)fi->fh;
		    return handle->read(elle::WeakBuffer(buf, size), size, offset);
		  }
		  catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error reading %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
		}
		
		static int fusop_write(const char *path, const char *buf, size_t size, off_t offset,
		                       struct fuse_file_info *fi)
		{
		  try
		  {
		    Handle* handle = (Handle*)fi->fh;
		    handle->write(elle::WeakBuffer((void*)buf, size), size, offset);
		  }
		  catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error writing %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
		}
		
		static int fusop_release(const char *path, struct fuse_file_info *fi)
		{
		  try
		  {
		    Handle* handle = (Handle*)fi->fh;
		    delete handle;
		  }
		  catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error releasing %s: %s", path, e);
        return -e.error_code();
      }
		  return 0;
		}
		
    class FileSystemImpl
    {
    public:
      Path& fetch_recurse(boost::filesystem::path const& path);
      std::unique_ptr<Operations> _operations;
      bool _full_tree;
      fuse* _handle;
      std::string _where;
      std::unique_ptr<Thread> _poller;
      std::unordered_map<boost::filesystem::path, std::unique_ptr<Path>> _cache;
    };

    FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
    : _impl(new FileSystemImpl{std::move(op), full_tree, nullptr})
    {
    }
    FileSystem::~FileSystem()
    {
      if (_impl->_handle)
        unmount();
      delete _impl;
    }
    void FileSystem::mount(boost::filesystem::path const& where,
                           std::vector<std::string> const& options)
    {
      if (_impl->_handle)
        throw elle::Error("Filesystem already mounted");
      _impl->_where = where.string();
      fuse_operations ops;
      memset(&ops, 0, sizeof(ops));
      ops.getattr = fusop_getattr;
      ops.readdir = fusop_readdir;
      ops.open = fusop_open;
      ops.read = fusop_read;
      ops.write = fusop_write;
      ops.release = fusop_release;
      _impl->_handle = fuse_create(where.string(), options, &ops, sizeof(ops), this);
      fuse* handle = _impl->_handle;
      _impl->_poller.reset(new Thread("fuse loop", [handle] { reactor::fuse_loop_mt(handle);}));
    }
    void FileSystem::unmount()
    {
      if (!_impl->_handle)
        throw elle::Error("Filesystem is not mounted");
      fuse_destroy(_impl->_handle, _impl->_where);
      _impl->_handle = nullptr;
    }
    Path& FileSystemImpl::fetch_recurse(boost::filesystem::path const& path)
    {
      ELLE_DEBUG("fetch_recurse on %s", path);
      auto it = _cache.find(path);
      if (it != _cache.end())
      {
        ELLE_DEBUG("cache hit");
        return *it->second;
      }
      else
      {
        ELLE_DEBUG("cache miss");
        if (path == "/")
        {
          auto p = _operations->path("/");
          auto ptr = p.get();
          ELLE_ASSERT(ptr);
          _cache[path] = std::move(p);
          return *ptr;
        }
        Path& parent = fetch_recurse(path.parent_path());
        auto p = parent.child(path.filename().string());
        auto ptr = p.get();
        ELLE_ASSERT(ptr);
        _cache[path] = std::move(p);
        return *ptr;
      }
    }
    Path& FileSystem::path(std::string const& spath)
    {
      ELLE_ASSERT(_impl);
      if (_impl->_full_tree)
      {
        return _impl->fetch_recurse(spath);
      }
      else
      {
        auto it = _impl->_cache.find(spath);
        if (it == _impl->_cache.end())
        {
          auto res = _impl->_operations->path(spath);
          auto ptr = res.get();
          _impl->_cache[spath] = std::move(res);
          return *ptr;
        }
        else
          return *it->second;
      }
    }
  }
}
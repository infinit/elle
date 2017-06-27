#include <elle/reactor/filesystem.hh>

namespace elle
{
  namespace reactor
  {
    namespace filesystem
    {
      FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
        : _operations(std::move(op))
        , _full_tree(full_tree)
      {}

      FileSystem::~FileSystem() = default;

      void
      FileSystem::mount(bfs::path const& where,
                        std::vector<std::string> const& options)
      {
        throw elle::Error("Not implemented");
      }

      void
      FileSystem::unmount()
      {}
    }
  }
}

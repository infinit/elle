#include <fstream>

#include <boost/filesystem.hpp>

#include <elle/log.hh>
#include <elle/serialize/BinaryArchive.hxx>
#include <elle/io/Path.hh>

namespace elle
{
  namespace concept
  {
    template <__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::load(elle::io::Path const& path)
    {
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("load(%s) in read mode", path);

      std::ifstream in(path.string(),
                       std::ios_base::in | std::ios_base::binary);

      if (!in.good())
        elle::err("unable to open the file '%s'", path);

      try
      {
        ELLE_DEBUG_SCOPE("deserializing the instance '%p' from the file '%s'",
                         this, path);

        typedef serialize::_Serializable<Archive> interface_t;
        ELLE_ASSERT_NEQ(dynamic_cast<interface_t*>(this), nullptr);
        static_cast<interface_t*>(this)->deserialize(in);
      }
      catch (std::exception const& e)
      {
        elle::err("unable to deserialize the instance from "
                  "the file: %s", e.what());
      }
    }

    template<__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::store(elle::io::Path const& path) const
    {
      namespace bfs = boost::filesystem;
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("store(%s)", path);

      auto dir = bfs::path{path.string()}.parent_path();
      bfs::create_directories(dir);

      std::ofstream out(path.string(),
                        std::ios_base::out | std::ios_base::binary);

      if (!out.good())
        elle::err("unable to open the file '%s'", path);

      try
      {
        ELLE_DEBUG_SCOPE("serializing the instance '%p' to the file '%s'",
                         this, path);

        typedef serialize::_Serializable<Archive> interface_t;
        ELLE_ASSERT_NEQ(dynamic_cast<interface_t const*>(this), nullptr);
        static_cast<interface_t const*>(this)->serialize(out);
      }
      catch (std::exception const& e)
      {
        elle::err("unable to serialize the instance to the file: %s",
                  e.what());
      }
    }

    template <__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::erase(elle::io::Path const& path)
    {
      namespace bfs = boost::filesystem;
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("erase(%s)", path);

      bfs::remove(path.string());
    }

    template <__ESS_ARCHIVE_TPL(Archive)>
    Boolean
    Fileable<Archive>::exists(elle::io::Path const& path)
    {
      namespace bfs = boost::filesystem;
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("exists(%s)", path);

      return bfs::exists(path.string());
    }
  }
}


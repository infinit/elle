#ifndef ELLE_CONCEPT_FILEABLE_HXX
# define ELLE_CONCEPT_FILEABLE_HXX

# include <fstream>

# include <elle/log.hh>

# include <elle/io/File.hh>
# include <elle/io/Path.hh>

# include <elle/serialize/BinaryArchive.hxx>

namespace elle
{
  namespace concept
  {

    template<__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::load(elle::io::Path const& path)
    {
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("load(%s) in read mode", path);

      std::ifstream in(path.string(),
                       std::ios_base::in | std::ios_base::binary);

      if (!in.good())
        throw elle::Exception(
          elle::sprintf("unable to open the file '%s'", path));

      try
        {
          ELLE_DEBUG_SCOPE("deserializing the instance '%p' from the file '%s'",
                           this, path);

          typedef serialize::_Serializable<Archive> interface_t;
          ELLE_ASSERT_NE(dynamic_cast<interface_t*>(this), nullptr);
          static_cast<interface_t*>(this)->deserialize(in);
        }
      catch (std::exception const& e)
        {
          throw elle::Exception(
            elle::sprintf("unable to deserialize the instance from "
                          "the file: %s", e.what()));
        }
    }

    template<__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::store(elle::io::Path const& path) const
    {
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("store(%s)", path);

      if (elle::io::File::Dig(path) == elle::Status::Error)
        throw elle::Exception(
          elle::sprintf("unable to dig the path '%s'", path));

      std::ofstream out(path.string(),
                        std::ios_base::out | std::ios_base::binary);

      if (!out.good())
        throw elle::Exception(
          elle::sprintf("unable to open the file '%s'", path));

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
          throw elle::Exception(
            elle::sprintf("unable to serialize the instance to the file: %s",
                          e.what()));
        }
    }

    template<__ESS_ARCHIVE_TPL(Archive)>
    void
    Fileable<Archive>::erase(elle::io::Path const& path)
    {
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("erase(%s)", path);

      if (elle::io::File::Erase(path) == elle::Status::Error)
        throw elle::Exception(
          elle::sprintf("unable to erase the file '%s'", path));
    }

    template<__ESS_ARCHIVE_TPL(Archive)>
    Boolean
    Fileable<Archive>::exists(elle::io::Path const& path)
    {
      ELLE_LOG_COMPONENT("elle.concept.Fileable");

      ELLE_DEBUG_SCOPE("exists(%s)", path);

      return (elle::io::File::Exist(path));
    }

  }
}

#endif

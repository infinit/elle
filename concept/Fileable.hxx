#ifndef ELLE_CONCEPT_FILEABLE_HXX
# define ELLE_CONCEPT_FILEABLE_HXX

# include <fstream>

# include <elle/log.hh>

# include <elle/io/File.hh>
# include <elle/io/Path.hh>

// XXX?
# include <elle/serialize/BinaryArchive.hxx>

# include <elle/standalone/Report.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace concept
  {

    template<__ECS_ARCHIVE_TPL(Archive)>
      Status Fileable<Archive>::Load(elle::io::Path const& path)
      {
        ELLE_LOG_TRACE_COMPONENT("elle.concept");
        ELLE_LOG_TRACE("Loading %p to from file %s", this, path.str())
        {

          ELLE_LOG_TRACE("Open file %s in read mode", path.str());
          std::ifstream in(path.str(), std::ios_base::in | std::ios_base::binary);
          if (!in.good())
            {
              escape("Cannot open in file '%s'", path.str().c_str());
              // XXX
              //throw std::runtime_error("Cannot read '"+  path.str().c_str() +"'");
            }

          try
            {
              ELLE_LOG_TRACE("Deserializing %p from file", this)
                {
                  typedef contract::_Serializable<Archive> interface_t;
                  assert(dynamic_cast<interface_t*>(this) != nullptr);
                  static_cast<interface_t*>(this)->deserialize(in);
                }
            }
          catch (std::exception const& err)
            {
              escape("%s", (
                  "Cannot load from '" + path.str() + "': " +
                  std::string(err.what())
              ).c_str());
              return elle::Status::Error;
              // XXX
              //throw std::runtime_error(
              //    "Cannot load from '" + path.str() + "': " +
              //    std::string(err.what())
              //);
            }
        }
        return elle::Status::Ok;
      }

    template<__ECS_ARCHIVE_TPL(Archive)>
      Status Fileable<Archive>::Store(elle::io::Path const& path) const
      {
        ELLE_LOG_TRACE_COMPONENT("elle.concept");
        ELLE_LOG_TRACE_SCOPE("Saving %p to file %s", this, path.str());

        if (elle::io::File::Dig(path) == elle::Status::Error)
          escape("unable to dig the chain of directories");

        std::ofstream out(path.str(), std::ios_base::out | std::ios_base::binary);
        if (!out.good())
          {
            escape("Cannot open out file '%s'", path.str().c_str());

            // XXX
            //throw std::runtime_error("Cannot read '"+  path.str().c_str() +"'");
          }

        try
          {
            typedef contract::_Serializable<Archive> interface_t;
            assert(dynamic_cast<interface_t const*>(this) != nullptr);
            static_cast<interface_t const*>(this)->serialize(out);
          }
        catch (std::exception const& err)
          {
            escape("%s", (
                "Cannot store to '" + path.str() + "': " +
                std::string(err.what())
            ).c_str());
            return elle::Status::Error;
            // XXX
            //throw std::runtime_error(
            //    "Cannot load from '" + path.str() + "': " +
            //    std::string(err.what())
            //);
          }
        return elle::Status::Ok;
      }

  }
}

#endif


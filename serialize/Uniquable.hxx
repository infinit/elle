#ifndef  ELLE_SERIALIZE_UNIQUABLE_HXX
# define ELLE_SERIALIZE_UNIQUABLE_HXX

# include <elle/idiom/Close.hh>

# include <sstream>

# include <elle/serialize/Base64Archive.hh>

# include <elle/serialize/Uniquable.hh>

# include <elle/standalone/Log.hh>
# include <elle/standalone/Report.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace serialize
  {

    template<typename T, template<ArchiveMode> class DefaultArchive>
      template<template<ArchiveMode> class Archive>
      Status Uniquable<T, DefaultArchive>::Save(elle::io::Unique& out) const
      {
        T const& self = static_cast<T const&>(*this);
        std::ostringstream ss;
        try
          {
            Archive<ArchiveMode::Output>(ss, self);
          }
        catch (std::exception const& err)
          {
            escape(err.what());
          }

        out = ss.str();
        return Status::Ok;
      }

    template<typename T, template<ArchiveMode> class DefaultArchive>
      template<template<ArchiveMode> class Archive>
      Status Uniquable<T, DefaultArchive>::Restore(elle::io::Unique const& in)
      {
        T& self = static_cast<T&>(*this);
        std::istringstream ss(in);
        try
          {
            Archive<ArchiveMode::Input>(ss) >> self;
          }
        catch (std::exception const& err)
          {
            escape(err.what());
          }

        return Status::Ok;
      }

  }
}

# include <elle/idiom/Close.hh>

#endif


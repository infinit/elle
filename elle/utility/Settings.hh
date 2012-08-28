#ifndef ELLE_UTILITY_SETTINGS_HH
# define ELLE_UTILITY_SETTINGS_HH

# include <elle/format/ini/File.hh>

# include <elle/concept/Fileable.hh>

# include <elle/io/fwd.hh>

# include <elle/serialize/IniArchive.hh>

namespace elle
{
  namespace utility
  {

    ///
    /// this class provides functionalities for human-readable, hence editable,
    /// settings files.
    ///
    /// note that the class supports both basic types and compound i.e
    /// Uniquable classes through the Behaviour structure dispatcher.
    ///
    /// XXX[to remove, this class has become useless]
    class Settings:
      public elle::format::ini::File,
      public elle::concept::MakeFileable<Settings, elle::serialize::IniArchive>,
      public elle::io::Dumpable
    {
    public:
      Status Dump(Natural32 const indent = 0) const;
    };

  }
}

#endif

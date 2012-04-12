#ifndef ELLE_UTILITY_SETTINGSSERIALIZER_HXX
# define ELLE_UTILITY_SETTINGSSERIALIZER_HXX

# include <elle/format/ini.hxx>

# include "Settings.hh"

// The class `Settings' derives from `elle::format::ini::File`, so implicit
// conversion happens when it comes to serialize. It's not necessary to
// specialize elle::serialize::ArchiveSerializer here.

#endif

#ifndef ELLE_SERIALIZE_JSONARCHIVE_HH
# define ELLE_SERIALIZE_JSONARCHIVE_HH

# include <cassert>

# include <elle/format/json/fwd.hh>
# include <elle/format/json.hh>

# include <elle/serialize/BaseArchive.hh>
# include <elle/serialize/fwd.hh>

namespace elle
{
  namespace serialize
  {
    /// JSON archive convert an object to its json representation, or build
    /// objects from a JSON stream.
    ///
    /// XXX does not support containers
    /// XXX does not support type versionning
    template <ArchiveMode mode>
    class JSONArchive;

    typedef JSONArchive<ArchiveMode::output> OutputJSONArchive;
    typedef JSONArchive<ArchiveMode::input> InputJSONArchive;
  }
} // !namespace elle::serialize

# include "JSONArchive.hxx"

#endif /* ! JSONARCHIVE_HH */

#ifndef  ELLE_SERIALIZE_EXTRACT_HH
# define ELLE_SERIALIZE_EXTRACT_HH

# include "BinaryArchive.hh"

# include <iosfwd>
# include <memory>

namespace elle
{
  namespace serialize
  {
    /// @brief Input archive that owns a stream instance.
    ///
    /// It has the same interface of its template arguments (it derives from
    /// it) but is constructible from a unique_ptr<std::istream> rvalue
    /// reference. This class is not meant to be used directly outside of the
    /// serialize module.
    template <typename Archive>
    class Extractor;

    /// Create an input archive that deserialize from a file.
    template <typename Archive = InputBinaryArchive>
    Extractor<Archive>
    from_file(std::string const& path);

    /// Create an input archive that deserialize from a string.
    template <typename Archive = InputBinaryArchive>
    Extractor<Archive>
    from_string(std::string const& str);

  }
}

# include "extract.hxx"

#endif
